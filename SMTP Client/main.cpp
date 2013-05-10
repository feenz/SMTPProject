#include <iostream>
#include <stdio.h>
#include <sstream>
#include <limits>
#include "Utils.h"
#include "Socket.h"
#include "Codes.h"

using namespace std;

void parseServerMsg( int &status, char (&message)[STRLEN] );
void sendMessageHelper( stringstream &ss, char (&sendMessage)[STRLEN] );

int main( int argc, char* argv[] ) {
	int port;
	int status;
	stringstream ss;
	hostent* localhost;
	ClientSocket client;
	string serverAddress;
	char recMessage[STRLEN];
	char encMessage[STRLEN];
    char sendMessage[STRLEN];
    
	if ( argc == 2 ) {
		port = atoi( argv[1] );
		if ( port > 0 && port < 65536 ) {
			// prompt for server address to contect to
			printf( "Enter the address of the SMPT server to connect: " );
			cin >> serverAddress;
			
			// C: attempt to connect
			printf( "Attempting to connect to %s:%d...\r\n", serverAddress.c_str( ), port );
			client.ConnectToServer( serverAddress.c_str( ), port );
			
			// S: 220 welcome from server
			client.RecvData( recMessage, STRLEN );
			parseServerMsg( status, recMessage );
			if ( status == ErrorCode::SRVC_RUN_SUCCES ) { // 220 from server
				printf( "%d %s\r\n", ErrorCode::SRVC_RUN_SUCCES, recMessage );
				
				// C: HELO to server
				localhost = gethostbyname( "" );
				ss << "HELO " << localhost->h_name;
				sendMessageHelper( ss, sendMessage );
				client.SendData( sendMessage );
				
				memset( &recMessage[0], '\0', sizeof( recMessage ) ); //clear buf
				memset( &sendMessage[0], '\0', sizeof( sendMessage ) ); //clear buf
				
				// S: 250 hello from server
				client.RecvData( recMessage, STRLEN );
				parseServerMsg( status, recMessage );
				if ( status == ErrorCode::REQUEST_SUCCESS ) { // 250 from server
					printf( "%d %s\r\n", status, recMessage );
					
					while ( true ) {					
						string msg = client.GetAndSendMessage( );
						memset( &recMessage[0], '\0', sizeof( recMessage ) ); //clear buf
						client.RecvData( recMessage, STRLEN );
						parseServerMsg( status, recMessage );
						printf( "%d %s\r\n", status, recMessage );
						
						// client requested DATA - allow message before sending
						if ( msg.compare( "DATA" ) == 0 && status == ErrorCode::START_MAIL_DATA ) {
							string line;
							cin.ignore( 10000, '\n' );
						    while ( line != "." ) {
    							cout << ">";
    							getline( cin, line );	
											    
						    	if ( line == "" )
						    		line = "\n";
						    		
								if ( line != "." )
									encrypt_decrypt( line, ENCRYPTION_KEY, "E" );
						    	client.SendData( const_cast<char*>( line.c_str( ) ) );
						    }
							memset( &recMessage[0], '\0', sizeof( recMessage ) ); //clear buf
							client.RecvData( recMessage, STRLEN );
							parseServerMsg( status, recMessage );
							printf( "%d %s\r\n", status, recMessage );
						} 
						
						if ( msg.compare( "INBOX" ) == 0 && status == ErrorCode::REQUEST_SUCCESS ) {
							int mailMsgCnt = 0;
							int mailLineCnt = 0;
							string buffer = "";
							
							memset( &recMessage[0], '\0', sizeof( recMessage ) );
							client.RecvData( recMessage, STRLEN );	
							string strRecv( recMessage );
							while ( strRecv != "EOF" ) {
								if ( strRecv != "EOF" )
									client.SendData( (char*)"Ok" );
									
								mailLineCnt++;
								
								if ( strRecv == "." ) {
									if ( mailMsgCnt > 0 )
										printf( "Message [%d]\r\n-------------------\r\n", mailMsgCnt ); 
									mailMsgCnt++;
									mailLineCnt = 0;
									
									printf( "%s\n", buffer.c_str( ) );
									buffer = "";
								} else {
									switch ( mailLineCnt ) {
										case 1:
											buffer += "Timestamp: " + strRecv + "\n";
											break;
										case 2:
											buffer += "To: " + strRecv + "\n";
											break;
										case 3:
											buffer += "From: " + strRecv + "\n\n";
											break;
										default:
											encrypt_decrypt( strRecv, ENCRYPTION_KEY, "D" );
											buffer += strRecv;
											if ( strRecv != "\n" )
												buffer += "\n";
											break;
									}
								}
								memset( &recMessage[0], '\0', sizeof( recMessage ) );
								client.RecvData( recMessage, STRLEN );
								strRecv = recMessage;
							}
						}	
						
						if ( msg.compare( "QUIT" ) == 0 )
							break; // client requested to quit
					}
					client.CloseConnection( );
					printf( "Connection closed.\n" );
				} else { // not 250
					printf( "%d %s\r\n", status, ErrorCode::getCodeDescription( (ErrorCode::Code)status ) );
				}
			} else { // not 220
				printf( "%d %s\r\n", status, ErrorCode::getCodeDescription( (ErrorCode::Code)status ) );
			}
		} else {
			printf( "Invalid Port: %d.\n Provide the port to connect to (1-65535)...\r\n", port );
		}		
	} else {
		printf( "Provide the port to connect to (1-65535)...\r\n" );
	}
	return 0;
}

void parseServerMsg( int &status, char (&message)[STRLEN] ) {
	string msg( message );			// covert to string
	char temp[sizeof( message )];	// holds onto entire message
	status = atoi( msg.substr( 0, 3 ).c_str( ) ); // get the error or status number
	string m = msg.substr( 4, msg.find( '\0' ) ); // get the message portion
	// copy msg to temp array, clear message buffer, copy back to original message array
	strcpy( temp, m.c_str( ) );
	memset( &message[0], '\0', sizeof( message ) );
	strcpy( message, temp );
}

void sendMessageHelper( stringstream &_stream, char (&sndbuf)[STRLEN] ) {
	string str( _stream.str( ) );
	strcpy( sndbuf, const_cast<char*>( str.c_str( ) ) );
	printf( "%s\n", sndbuf );
	_stream.clear( );
	_stream.str( std::string( ) );
}
