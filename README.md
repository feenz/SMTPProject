Compile Project
---------------
Server: g++ -o Server main.cpp Socket.cpp ThreadUtils.cpp Message.cpp Codes.cpp -lws2_32
Client: g++ -o Client main.cpp Socket.cpp Codes.cpp -lws2_32
