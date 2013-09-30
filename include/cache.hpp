#pragma once

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/log/trivial.hpp>

using boost::asio::ip::tcp;
typedef unsigned int uint32;

namespace BoostCached {

	class Cache
	: public boost::basic_lockable_adapter<boost::mutex>
	{
	public:
		Cache();

		std::string &get(std::string key);
		void set(std::string key, std::string value);

	private:
		std::map<std::string, std::string> _values;
		std::string nil;
	};



	class Connection
		: public boost::enable_shared_from_this<Connection>
	{
	public:
		typedef boost::shared_ptr<Connection> pointer;

		~Connection();

		static boost::shared_ptr<Connection> create(boost::asio::io_service &io_service, Cache &cache);
		tcp::socket &socket();

		void start();
		void thread_callable();

	private:
		tcp::socket _socket;
		std::string _message;
		Cache &_cache;
		enum { max_buffer_size = 1024 };
		char _data[max_buffer_size];
		boost::thread t;

		Connection(boost::asio::io_service &io_service, Cache &cache);

		void handle_write(const boost::system::error_code& err, size_t bytesTransferred);
		void handle_read(const boost::system::error_code& err, size_t bytesTransferred);
	};



	class Server
	{
	public:
		Server(boost::asio::io_service &io_service, uint32 port, Cache &cache);

	private:
		void start_accept();

		void handle_accept(Connection::pointer new_connection, const boost::system::error_code &err);

		tcp::acceptor _acceptor;
		Cache &_cache;
	};

	int start_server(uint32 port, Cache &cache);
};
