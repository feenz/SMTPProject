Server:
	g++ -o Server.exe main.cpp Socket.cpp ThreadUtils.cpp Message.cpp Codes.cpp
Client:
	g++ -o Client.exe main.cpp Socket.cpp Codes.cpp