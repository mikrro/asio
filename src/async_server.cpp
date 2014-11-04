#include <iostream>
#include <ctime>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
	std::time_t now = std::time(0);
	return std::ctime(&now);
}

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_service& io_service)
	{
		return pointer(new tcp_connection(io_service));
	}

	tcp::socket& socket()
	{
		return _socket;
	}

	void start()
	{
		m_message = make_daytime_string();

		boost::asio::async_write(_socket, boost::asio::buffer(m_message),
				boost::bind(&tcp_connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
	}

private:
	tcp::socket _socket;
	std::string m_message;

	tcp_connection(boost::asio::io_service& io_service) : _socket(io_service) {	}

	void handle_write( const boost::system::error_code& /*error*/, size_t /*bytes_transfered*/)
	{
	}

};

class tcp_server
{
public:
	tcp_server(boost::asio::io_service& io_service)
	: m_port(7000), _acceptor(io_service, tcp::endpoint(tcp::v4(), m_port))
	{
		std::cout << "Listen on " << m_port << std::endl;
		start_accept();
	}

private:
	unsigned short m_port;
	tcp::acceptor _acceptor;

	void start_accept()
	{
		tcp_connection::pointer new_connection = tcp_connection::create(_acceptor.get_io_service());

		_acceptor.async_accept(new_connection->socket(),
				boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));


	}

	void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
	{
		if (!error)
			new_connection->start();

		start_accept();
	}
};

int main()
{

	try {

		boost::asio::io_service io_service;

		tcp_server server(io_service);

		io_service.run();

	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}





