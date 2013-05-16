//Socket.cpp
#include "Socket.h"

Socket::Socket( ) {
    if( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR ) 
        cerr << "Socket Initialization: Error with WSAStartup" << endl;
    //Create a socket
    mySocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( mySocket == INVALID_SOCKET )
        cerr << "Socket Initialization: Error creating socket" << endl;
    myBackup = mySocket;
}

Socket::~Socket( ) {
    WSACleanup( );
}

bool Socket::SendData( string data ) {
    const char* buffer = data.c_str( );
    send( mySocket, buffer, strlen( buffer ), 0 );
    return true;
}

int Socket::RecvData( string& data ) {
    char buffer[STRLEN];
    int ret = recv( mySocket, buffer, STRLEN, 0 );
    buffer[ret] = '\0';
    data = buffer;
    return ret;
}

void Socket::CloseConnection( ) {
    closesocket( mySocket );
    mySocket = myBackup;
}

std::string Socket::GetAndSendMessage( ) {
    string message;
    cout << ">";
    getline( cin, message );
    SendData( message );
    return message;
}

bool ClientSocket::ConnectToServer( const char *ipAddress, int port ) {
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = inet_addr( ipAddress );
    myAddress.sin_port = htons( port );
    
    if ( connect( mySocket, (SOCKADDR*) &myAddress, sizeof( myAddress ) ) == SOCKET_ERROR ) {
        cerr << "ClientSocket: Failed to connect" << endl;
        return false;
    } 
    return true;
}

/**
 * Function takes in a string and either encrypts or decrypts based on a given key and action. 
 * @param message is a reference to the string which the action is performed on
 * @param key is the encryption key used for encryption or decryption
 * @param action is encrypt (E) or decrypt (D)
 * @return the referenced message parameter contains either the cipher or plain text depending on the action 
 */
void ClientSocket::Cryptor( string &message, int key, const char* action ) {
    for ( int index = 0; index < message.length( ); index++  ) {    // loop through each character
        int decValue;       
        if ( strcmp( action, "E" ) == 0 ) { 
            if ( message[index] != '\n' ) {
                decValue = ( int( message[index] ) - 32 ) + key;
                if ( decValue > 95 )
                    decValue -= 95;
                decValue += 32;
            }
        } else if ( strcmp( action, "D" ) == 0 ) {
            if ( message[index] != '\n' ) {
                decValue = ( int( message[index] ) - 32 ) - key;
                if ( decValue < 0 )
                    decValue += 95;
                decValue += 32; 
            }
        }
        message[index] = decValue;
    }
};
