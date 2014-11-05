#include <cstdlib>
#include <iostream>
#include <deque>
#include <thread>
#include <boost/asio.hpp>
#include "message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<message> message_queue;

class chat_client
{
public:
	chat_client(boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iterator)
	: io_service_(io_service), socket_(io_service)
	{
		do_connect(endpoint_iterator);
	}

	void write(const message& msg)
	{
		io_service_.post(
			[this, msg] ()
			{
				bool write_in_progress = !write_msgs_.empty();
				write_msgs_.push_back(msg);
				if (!write_in_progress)
					do_write();
			});
	}

	void close()
	{
		io_service_.post(
			[this]()
			{
				socket_.close();
			});
	}

private:
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	message read_message_;
	message_queue write_msgs_;

	void do_connect(tcp::resolver::iterator endpoint_iterator)
	{
		boost::asio::async_connect(socket_, endpoint_iterator,
				[this] (boost::system::error_code ec, tcp::resolver::iterator)
				{
					if(!ec)
						do_read_header();
				});
	}

	void do_read_header()
	{
		boost::asio::async_read(socket_,
				boost::asio::buffer(read_message_.data(), message::header_length),
				[this](boost::system::error_code ec, std::size_t /*length*/)
				{
					if(!ec && read_message_.decode_header())
					{
						do_read_body();
					}
					else
						socket_.close();
				});
	}

	void do_read_body()
	{
		boost::asio::async_read(socket_,
				boost::asio::buffer(read_message_.body(), read_message_.body_length()),
				[this](boost::system::error_code ec, std::size_t /*length*/)
				{
					if(!ec)
					{
						std::cout << "[client::do_read_message] " << read_message_.body() << "\n";
			            std::cout.write(read_message_.body(), read_message_.body_length());
						std::cout << "\n";
						do_read_header();
					}
					else
						socket_.close();
				}
				);
	}

	void do_write()
	{
		boost::asio::async_write(socket_,
				boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
				[this](boost::system::error_code ec, std::size_t /*length*/)
				{
					if(!ec)
					{
						std::cout << "[client::do_write] " << write_msgs_.front().body() << "\n";
						write_msgs_.pop_front();
						if(!write_msgs_.empty())
							do_write();
					}
					else
					{
						socket_.close();
					}
				});
	}
};

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		std::cerr << "Usage: <host> <port>\n";
		return 1;
	}

	try {

		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);

		auto endpoint_iterator = resolver.resolve({argv[1], argv[2]});

		chat_client c(io_service, endpoint_iterator);

		std::thread t([&io_service] () {io_service.run();});

		char line[message::max_body_length +1];

		while(std::cin.getline(line, message::max_body_length+1))
		{
			message msg;
			msg.body_length(std::strlen(line));
			std::memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			c.write(msg);
		}
		t.join();
		c.close();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception " << e.what() << "\n";
	}
	return 0;
}
























