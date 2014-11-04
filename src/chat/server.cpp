#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "chat/message.hpp"

using boost::asio::tcp;

typedef std::deque<message> chat_message_queue;
