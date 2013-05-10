#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

class ThreadUtils {
	public:
		ThreadUtils( );
		~ThreadUtils( );
		bool sendData( string );
		bool sendResponse( int, string );
		int recvData( string& );
		void setSocket( SOCKET );
	private:
		SOCKET _socket;
};

