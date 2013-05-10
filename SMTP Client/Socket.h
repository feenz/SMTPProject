//Socket.h
#pragma once
#include <iostream>
#include "WinSock2.h"

using namespace std;

const int STRLEN = 1000;
const int ENCRYPTION_KEY = 5;

class Socket {
    protected:
        WSADATA wsaData;
        SOCKET mySocket;
        SOCKET myBackup;
        SOCKET acceptSocket;
        sockaddr_in myAddress;
    public:
        Socket( );
        ~Socket( );
        bool SendData( char* );
        bool RecvData( char*, int );
        void CloseConnection( );
        std::string GetAndSendMessage( );
};

class ClientSocket : public Socket {
    public:
        void ConnectToServer( const char*, int );
};
