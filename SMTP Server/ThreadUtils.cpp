#include "ThreadUtils.h"

ThreadUtils::ThreadUtils( ) { }

ThreadUtils::~ThreadUtils( ) { }

void ThreadUtils::setSocket( SOCKET sock ) {
	this->_socket = sock;
}

bool ThreadUtils::sendData( string data ) {
	const char* buffer = data.c_str( );
	send( this->_socket, buffer, strlen( buffer ), 0 );
	return true;
}

bool ThreadUtils::sendResponse( int code, string response ) {
	stringstream ss;
	ss << code << " " << response;
	this->sendData( ss.str( ) );
	ss.clear( );
	ss.str( std::string( ) );
	return true;
}

int ThreadUtils::recvData( string &data ) {
	char buffer[1000];
	int result = recv( this->_socket, buffer, 1000, 0 );
	buffer[result] = '\0';
	data = buffer;
	return result;
}
