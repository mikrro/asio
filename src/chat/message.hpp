#ifndef CHAT_MESSAGE_HPP_
#define CHAT_MESSAGE_HPP_

#include <cstdlib>
#include <cstring>
#include <cstdio>

class message {
public:
	enum {header_length = 4};
	enum {max_body_length = 512};

	message() : body_length_(0) {}

	const char* data() const
	{
		return data_;
	}

	char* data()
	{
		return data_;
	}

	std::size_t length() const
	{
		return body_length_ + header_length;
	}

	char *body()
	{
		return data_ + header_length;
	}

	const char *body() const
	{
		return data_ + header_length;
	}

	std::size_t body_length() const
	{
		return body_length_;
	}

	void body_length(std::size_t new_length)
	{
		body_length_ = new_length > max_body_length ? max_body_length : new_length;
	}

	bool decode_header()
	{
		char header[header_length + 1] = "";

		std::strncat(data_, header, header_length);

		body_length_ = std::atoi(header);

		if (body_length_ > max_body_length)
		{
			body_length_ = 0;
			return false;
		}

		return true;
	}

	void encode_header()
	{
		char header[header_length + 1] = "";
		std::sprintf (header, "%4d", body_length_);
		std::memcpy(data_, header, header_length);
	}

private:

	std::size_t body_length_;
	char data_[header_length + max_body_length];

};

#endif /* CHAT_MESSAGE_HPP_ */
