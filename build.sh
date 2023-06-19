#/bin/bash
#
# USE -DARM FOR COMPLING ON RASPBERRY PI 
#
#
MAIN_FILE="../main.cpp"
COMPILER="g++"
MACROS="-DCOMPILER_MSVC -DDEBUGGING=1 -DUSE_SSL=1 -DLINUX=1" #-DARM
CPP="../../lpop/nix_threading.cpp ../../lpop/queue.cpp ../../lpop/stringz.cpp ../../lpop/marray.cpp ../../lpop/token.cpp ../../lpop/required/memory.cpp ../../lpop/required/nix.cpp ../../lpop/bucket.cpp ../../lpop/cl_options.cpp ../../libpop/JsonParser.cpp ../../lpop/socket.cpp  ../../lpop/nix_socket.cpp ../../lpop/ssl.cpp ../../lpop/httpprotocol.cpp ../../lpop/htmlparser.cpp  ../../lpop/base64.cpp" 
FLAGS="-w -g -lssl -lcrypto  -lpthread -lz -fsanitize=address"
OUTPUT="test"

if [ -d "build" ]
then
	rm -rf build
fi

mkdir build
cd build
$COMPILER $MACROS $MAIN_FILE $CPP $FLAGS -o $OUTPUT
cd ..
