# common
This is a cross-platform C++ library that supports Windows, Linux, macOS, Emscripten, Android, iOS, including the following functions:
IO
communication
configuration
crypto
data
database
diag
driver
exception
http
json
microservice
net
rpc
system
thread
xml
yml

Compile with CMake, easy to compile.

How to build.

1. build debug version:
mkdir -p out/debug
cd out/debug
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr/local ../../ -DCOMMON_STATIC_STDLIB=OFF -DCOMMON_BUILD_NOTIFICATION=OFF -DCOMMON_BUILD_MQTTCLIENT=OFF -DCOMMON_BUILD_REDISCLIENT=OFF -DCOMMON_BUILD_MYSQL=ON -DCOMMON_BUILD_MYSQL_STATIC=OFF -DCOMMON_BUILD_ORACLE=OFF -DCOMMON_BUILD_OPCUACLIENT=OFF
make -j4

2. build release version:
mkdir -p out/release
cd out/release
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ../../ -DCOMMON_STATIC_STDLIB=ON -DCOMMON_BUILD_NOTIFICATION=OFF -DCOMMON_BUILD_MQTTCLIENT=OFF -DCOMMON_BUILD_REDISCLIENT=OFF -DCOMMON_BUILD_MYSQL=ON -DCOMMON_BUILD_MYSQL_STATIC=OFF -DCOMMON_BUILD_ORACLE=OFF -DCOMMON_BUILD_OPCUACLIENT=OFF
make -j4

