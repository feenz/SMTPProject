Server:
	g++ -o Server.exe main.cpp Socket.cpp ThreadUtils.cpp Message.cpp Codes.cpp -lws2_32
Client:
	g++ -o Client.exe main.cpp Socket.cpp Codes.cpp -lws2_32