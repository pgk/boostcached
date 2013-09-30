#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <cache.hpp>


namespace po = boost::program_options;

int main(int argc, const char **argv)
{

	uint32 port_number;
	std::string log_file;

	po::options_description desc("Allowed options");
	po::variables_map vm;

	desc.add_options()
		("help,h", "show help message")
		("port,p", po::value<uint32>(&port_number)->default_value(8080), "port number")
		("log,l", po::value<std::string>(&log_file)->default_value(std::string("stdout")), "log file (currently only stdout)")
	;

	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}


	BOOST_LOG_TRIVIAL(info) << "Port set to " << port_number << "...";
	BOOST_LOG_TRIVIAL(info) << "Logging to " << log_file << "...";
	BOOST_LOG_TRIVIAL(info) << "boostcached started...";
	BoostCached::Cache cache;

	BOOST_LOG_TRIVIAL(info) << "preloading some demo keys...";
	cache.set(std::string("foo"), std::string("bar"));
	cache.set(std::string("bar"), std::string("baz"));

	return BoostCached::start_server(port_number, cache);
}
