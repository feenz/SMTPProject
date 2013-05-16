//Socket.h
#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>
#include "WinSock2.h"

using namespace std;

const int STRLEN            = 1000;
const int ENCRYPTION_KEY    = 5;
const string COMMAND_DATA   = "DATA";
const string COMMAND_INBOX  = "INBOX";
const string COMMAND_HELO   = "HELO";
const string COMMAND_EOF    = "EOF";
const string COMMAND_QUIT   = "QUIT";

class Socket {
    protected:
        WSADATA     wsaData;
        SOCKET      mySocket;
        SOCKET      myBackup;
        SOCKET      acceptSocket;
        sockaddr_in myAddress;
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
        void Cryptor( string&, int, const char* );
};
