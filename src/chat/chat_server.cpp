#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<message> message_queue;

class participant
{
public:
	virtual ~participant() {}
	virtual void deliver( const message& msg ) = 0;
};

typedef std::shared_ptr<participant> participant_ptr;

class chat_room
{
public:
	void join(participant_ptr participant)
	{
		participants_.insert(participant);
		for (auto msg: recent_msgs_)
			participant->deliver(msg);
	}

	void leave(participant_ptr participant)
	{
		participants_.erase(participant);
	}

	void deliver(const message& msg)
	{
		recent_msgs_.push_back(msg);
		while(recent_msgs_.size() > max_recent_messages)
			recent_msgs_.pop_front();

		for (auto participant : participants_)
			participant->deliver(msg);
	}

private:
	std::set<participant_ptr> participants_;
	enum {max_recent_messages = 100};
	message_queue recent_msgs_;
};

class chat_session : public participant,
public std::enable_shared_from_this<chat_session>
{
public:
	chat_session(tcp::socket socket, chat_room& room)
	: socket_(std::move(socket)), room_(room)
	{}

	void start()
	{
		room_.join(shared_from_this());
		read_header();
	}

	void deliver(const message& msg)
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
		  do_write();
		}
	}

private:
	tcp::socket socket_;
	chat_room& room_;
	message read_message_;
	message_queue write_msgs_;

	void read_header()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
		boost::asio::buffer(read_message_.data(), message::header_length),
		[this, self] (boost::system::error_code ec, std::size_t /*length*/)
		{
			std::cout << "[server::read_header] " << read_message_.body() << "\n";

			if(!ec && read_message_.decode_header())
				read_body();
			else
				room_.leave(shared_from_this());
		});
	}

	void read_body()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
		boost::asio::buffer(read_message_.data(), read_message_.length()),
		[this, self] (boost::system::error_code ec, std::size_t /*lenght*/)
		{
			if (!ec)
			{
				std::cout << "[server::read_body] " << read_message_.body() << "\n";
				room_.deliver(read_message_);
				read_header();
			}
			else
				room_.leave(shared_from_this());
		});
	}

	void do_write()
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_,
		boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
		[this, self] (boost::system::error_code ec, std::size_t /*length*/)
		{
			if(!ec)
			{
				std::cout << "[server::do_write]" << write_msgs_.front().body() << "\n";
				write_msgs_.pop_front();
				if (!write_msgs_.empty())
					do_write();
				else
					room_.leave(shared_from_this());
			}
		});
	}
};

class chat_server
{
public:

	chat_server(boost::asio::io_service& io_service, int port)
	: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), socket_(io_service)
	{
		do_accept();
	}

private:

	tcp::acceptor acceptor_;
	tcp::socket socket_;
	chat_room room_;

	void do_accept()
	{
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec)
			{
				if (!ec)
				{
					std::make_shared<chat_session>(std::move(socket_), room_)->start();

				}
				do_accept();
			});
	}
};

int main(int argc, char** argv)
{

	if (argc < 2)
	{
		std::cerr << "Usage: chat_server <port> [<port> ... ] \n";
		return 1;
	}

	try {
		boost::asio::io_service io_service;

		std::list<chat_server> servers;

		for (int i=1; i < argc; ++i)
		{
			servers.emplace_back(io_service, std::atoi(argv[i]));
			// TODO what's the hell that?
		}

		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}


















