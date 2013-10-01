cmake_minimum_required (VERSION 2.6)
project (boostcached)

set (boostcached_VERSION_MAJOR 0)
set (boostcached_VERSION_MINOR 1)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set (LIB_BOOST_PATH "/usr/local/lib")

configure_file (
	"${PROJECT_SOURCE_DIR}/include/boostcached.h.in"
	"${PROJECT_BINARY_DIR}/include/boostcached.h"
)

include_directories (
	"${PROJECT_BINARY_DIR}/include"
	"/usr/local/include"
)

set (
	EXTRA_LIBS 
	${EXTRA_LIBS} 
	"${LIB_BOOST_PATH}/libboost_regex-mt.a" 
	"${LIB_BOOST_PATH}/libboost_program_options-mt.a"
	"${LIB_BOOST_PATH}/libboost_log-mt.a"
	"${LIB_BOOST_PATH}/libboost_thread-mt.a"
	"${LIB_BOOST_PATH}/libboost_system-mt.a"
)

add_executable (boostcached src/main.cpp src/cache.cpp)
target_link_libraries (boostcached ${EXTRA_LIBS})
