#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
	std::time_t now = std::time(0);
	return std::ctime(&now);
}

int main()
{
	try {
		boost::asio::io_service io_service;

		tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 7000));

		while(1)
		{
			tcp::socket socket(io_service);

			acceptor.accept(socket);

			std::string message = make_daytime_string();

			boost::system::error_code ignored_error;

			boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
		}

	} catch(std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
