#include <cache.hpp>

	
BoostCached::Cache::Cache() :
	_values(),
	nil("")
{
}

std::string &BoostCached::Cache::get(std::string key)
{
	try {
		return _values[key];
	} catch (std::exception &e) {
		BOOST_LOG_TRIVIAL(info) << "Cache miss for key " << key << ". Returning empty string";
		return nil;
	}
}

void BoostCached::Cache::set(std::string key, std::string value)
{
	boost::lock_guard<Cache> guard(*this);
	_values[key] = value;
}



BoostCached::Connection::~Connection()
{
	BOOST_LOG_TRIVIAL(info) << "Closing Connection<" << this << ">";
}

BoostCached::Connection::pointer BoostCached::Connection::create(boost::asio::io_service &io_service, Cache &cache)
{
	return pointer(new Connection(io_service, cache));
}

tcp::socket &BoostCached::Connection::socket()
{
	return _socket;
}

void BoostCached::Connection::start()
{
	BOOST_LOG_TRIVIAL(info) << "Starting new thread for Connection<" << this << ">";
	t = boost::thread(boost::bind(&Connection::thread_callable, shared_from_this()));
	t.join();
}

void BoostCached::Connection::thread_callable()
{
	_socket.async_read_some(boost::asio::buffer(_data, max_buffer_size),
		boost::bind(&Connection::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

BoostCached::Connection::Connection(boost::asio::io_service &io_service, Cache &cache)
	: _socket(io_service),
	_cache(cache)
{
	BOOST_LOG_TRIVIAL(info) << "Opening Connection<" << this << ">";
}

void BoostCached::Connection::handle_write(const boost::system::error_code& err, size_t bytesTransferred)
{
	if (!err) {
		BOOST_LOG_TRIVIAL(info) << "[DEBUG] " << bytesTransferred << " bytes written";
		_socket.async_read_some(boost::asio::buffer(_data, max_buffer_size),
			boost::bind(&Connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
}

void BoostCached::Connection::handle_read(const boost::system::error_code& err, size_t bytesTransferred)
{
	if (!err)
	{
		std::string what_we_got = std::string(_data).substr(0, bytesTransferred);

		std::string action = what_we_got.substr(0, 3);
		BOOST_LOG_TRIVIAL(info) << "[DEBUG] thread<" << boost::this_thread::get_id() << ">: " << bytesTransferred << " bytes read.";


		if (action == "get" || action == "GET")
		{
			std::string key = boost::replace_all_copy(boost::trim_copy(what_we_got.substr(3)), "\r\n", "");
			BOOST_LOG_TRIVIAL(info) << "[DEBUG] GET " << key << ":";
			_message = _cache.get(key);
		} else if (action == "set" || action == "SET")
		{
			std::vector<std::string> sv;
			std::string key_value_with_enter = boost::trim_right_copy(what_we_got.substr(4));
			std::string key_value = boost::replace_all_copy(key_value_with_enter, "\r\n", "");

			boost::algorithm::split(sv, key_value, boost::algorithm::is_space());
			std::string key = sv.front();
			std::string value = sv.back();
			BOOST_LOG_TRIVIAL(info) << "[DEBUG]\tSET "<< key << ": " << value;
			_cache.set(key, value);
			_message = "OK";
		} else {
			_message = "";
		}

		boost::asio::async_write(_socket, boost::asio::buffer(_message + "\n"),
			boost::bind(&Connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
}




BoostCached::Server::Server(boost::asio::io_service &io_service, uint32 port, Cache &cache) :
	_acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
	_cache(cache)
{
	start_accept();
}


void BoostCached::Server::start_accept()
{
	Connection::pointer new_connection =
		Connection::create(_acceptor.get_io_service(), _cache);

	_acceptor.async_accept(new_connection->socket(),
		boost::bind(&Server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void BoostCached::Server::handle_accept(Connection::pointer new_connection,
	const boost::system::error_code &err)
{
	if (!err) {
		new_connection->start();
	}

	start_accept();
}

int BoostCached::start_server(uint32 port, Cache &cache)
{
	try {
		boost::asio::io_service io_service;
		Server server(io_service, port, cache);
		io_service.run();
	} catch (std::exception &e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
	}

	return 0;
}
