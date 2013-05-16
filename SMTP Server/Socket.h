#pragma once
#include <iostream>
#include "WinSock2.h"
#include <ws2tcpip.h>

using namespace std;

#define NI_MAXSERV    32
#define NI_MAXHOST  1025

const int STRLEN = 1000;
const string LOG_FILE = "serverlog.csv";

class Socket {
    protected:
        WSADATA wsaData;
        SOCKET serverSocket;
        SOCKET backupSocket;
        SOCKET listenSocket;
        hostent* localhost;
        sockaddr_in localip;
    public:
        Socket( );
        ~Socket( );
        bool SendData( string );
        int RecvData( string& );
        void CloseConnection( );
        std::string GetAndSendMessage( );
};

class ClientSocket : public Socket {
    public:
        bool ConnectToServer( const char*, int );
};
