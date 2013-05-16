#include <fstream>
#include "Socket.h"
#include "Utils.h"
#include "ThreadUtils.h"


using namespace std;

Socket::Socket( ) {
    if( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR )
        cerr << "Socket Initialization: Error with WSAStartup" << endl;
    //Create a socket
    serverSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( serverSocket == INVALID_SOCKET )
        cerr << "Socket Initialization: Error creating socket" << endl;
    backupSocket = serverSocket;
}

Socket::~Socket( ) {
    WSACleanup( );
}

void Socket::CloseConnection( ) {
    closesocket( serverSocket );
    serverSocket = backupSocket;
}

bool Socket::SendData( string data ) {
	const char* buffer = data.c_str( );
    send( serverSocket, buffer, strlen( buffer ), 0 );
    return true;
}

int Socket::RecvData( string& data ) {
	char buffer[STRLEN];
    int ret = recv( serverSocket, buffer, STRLEN, 0 );
    buffer[ret] = '\0';
    data = buffer;
    return ret;
}

std::string Socket::GetAndSendMessage( ) {
	string message;
    cout << ">";
    getline( cin, message );
    SendData( message );
    return message;
}

bool ClientSocket::ConnectToServer( const char *ipAddress, int port ) {
    localip.sin_family = AF_INET;
    localip.sin_addr.s_addr = inet_addr( ipAddress );
    localip.sin_port = htons( port );
    
    if ( connect( serverSocket, (SOCKADDR*) &localip, sizeof( localip ) ) == SOCKET_ERROR ) {
        cerr << "ClientSocket: Failed to connect" << endl;
        return false;
    }
    return true; 
}
