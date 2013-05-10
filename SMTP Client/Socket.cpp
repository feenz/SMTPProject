//Socket.cpp
#include "Socket.h"

Socket::Socket( ) {
    if( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR ) {
        cerr << "Socket Initialization: Error with WSAStartup" << endl;
        system( "pause" );
        WSACleanup( );
        exit( 10 );
    }
    //Create a socket
    mySocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( mySocket == INVALID_SOCKET ) {
        cerr << "Socket Initialization: Error creating socket" << endl;
        system( "pause" );
        WSACleanup( );
        exit( 11 );
    }
    myBackup = mySocket;
}

Socket::~Socket( ) {
    WSACleanup( );
}

bool Socket::SendData( char *buffer ) {
    send( mySocket, buffer, strlen( buffer ), 0 );
    return true;
}

bool Socket::RecvData( char *buffer, int size ) {
    int i = recv( mySocket, buffer, size, 0 );
    buffer[i] = '\0';
    return true;
}

void Socket::CloseConnection( ) {
    closesocket( mySocket );
    mySocket = myBackup;
}

std::string Socket::GetAndSendMessage( ) {
	char message[STRLEN];
    cin.ignore( ); // without this, it gets the return char from the last cin and ignores the following one!
    cout << ">";
    cin.get( message, STRLEN );
    SendData( message );
    string msg( message );
    return msg;
}

void ClientSocket::ConnectToServer( const char *ipAddress, int port ) {
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = inet_addr( ipAddress );
    myAddress.sin_port = htons( port );
    
    if ( connect( mySocket, (SOCKADDR*) &myAddress, sizeof( myAddress ) ) == SOCKET_ERROR ) {
        cerr << "ClientSocket: Failed to connect" << endl;
        system( "pause" );
        WSACleanup( );
        exit( 13 );
    } 
}
