
rm -rf build
mkdir build

gcc -I. -o build/server sctp_server.c -lsctp 
gcc -I. -o build/client sctp_client.c -lsctp 
