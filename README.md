##BoostCached

As a means to study Boost C++, in particular `boost::asio`, here is a naive "Hello World" in-memory key/value store in the spirit of memcached. Also using `boost::log::trivial`, `boost::thread/mutex`, and `boost::program_options`.

###How to compile

* Tested only on OS X Lion, but should compile on a recent compiler (`clang/llvm` and probably `gcc/g++`)
* You will need `cmake` and `make`, with all their dependencies
* The assumption is that you have installed boost libraries and they live on `/usr/local/include` and `/usr/local/lib`

cd into the project directory and:

        cmake .
        make

then if all went well, you can run it with:

       ./bin/boostcached --port 8080

or get help with:

       ./bin/boostcached --help

then you can test it with:

	    telnet 127.0.0.1 8081
	    get foo
	    get bar
	    set MyKey xyz
	    etc...

note that this is not daemonized and that logging is on stdout.
