#include <iostream>
#include <stdio.h>
#include <sstream>
#include <limits>
#include "Utils.h"
#include "Socket.h"
#include "Codes.h"

using namespace std;

int main( int argc, char* argv[] ) {
	int port;
	int status;
	int result;
	stringstream ss;
	hostent* localhost;
	ClientSocket client;
	string serverAddress;
	string recMessage = "";
    string sendMessage = "";
    
	if ( argc == 3 ) {
		port = atoi( argv[1] );
		serverAddress = argv[2];
		if ( port > 0 && port < 65536 ) {			
			// C: attempt to connect
			printf( "Attempting to connect to %s:%d...\r\n\n", serverAddress.c_str( ), port );
			bool connected = client.ConnectToServer( serverAddress.c_str( ), port );

			if ( connected ) {
				localhost = gethostbyname( "" );

				// S: 220 welcome from server
				result = client.RecvData( recMessage );
				if ( result == -1 ) {
					printf("[Error]: %s\n", recMessage.c_str( ) );
					client.CloseConnection( );
				}

				if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::SRVC_RUN_SUCCES ) { // 220 from server
					printf( "%s\r\n", recMessage.c_str( ) );
					sendMessage = ( "HELO " + string( localhost->h_name ) );
					client.SendData( sendMessage );
					printf( "%s\r\n", sendMessage.c_str( ) );

					// S: 250 hello from server
					result = client.RecvData( recMessage );
					if ( result == -1 ) {
						printf("[Error]: %s\n", recMessage.c_str( ) );
						client.CloseConnection( );
					}

					if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::REQUEST_SUCCESS ) { // 250 from server
						printf( "%s\r\n", recMessage.c_str( ) );

						while ( true ) {			
							string msg = client.GetAndSendMessage( );

							result = client.RecvData( recMessage );
							if ( result == -1 ) {
								printf("[Error]: %s\n", recMessage.c_str( ) );
								client.CloseConnection( );
							}
							printf( "%s\r\n", recMessage.c_str( ) );
							
							// client requested DATA - allow message before sending
							if ( msg.compare( "DATA" ) == 0 && atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::START_MAIL_DATA ) {
								string line;
								//cin.ignore( 10000, '\n' );
							    while ( line != "." ) {
	    							cout << ">";
	    							getline( cin, line );	
												    
							    	if ( line == "" )
							    		line = "\n";
							    		
									if ( line != "." )
										encrypt_decrypt( line, ENCRYPTION_KEY, "E" );
									client.SendData( line );
							    }

								result = client.RecvData( recMessage );
								if ( result == -1 ) {
									printf("[Error]: %s\n", recMessage.c_str( ) );
									client.CloseConnection( );
								}
								printf( "%s\r\n", recMessage.c_str( ) );
							} 
							
							if ( msg.compare( "INBOX" ) == 0 && atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::REQUEST_SUCCESS ) {
								int mailMsgCnt = 1;
								int mailLineCnt = 0;
								string buffer = "";
								

								result = client.RecvData( recMessage );
								if ( result == -1 ) {
									printf("[Error]: %s\n", recMessage.c_str( ) );
									client.CloseConnection( );
								}	

								while ( recMessage != "EOF" ) {
									if ( recMessage != "EOF" )
										client.SendData( string( ErrorCode::REQUEST_SUCCESS + " Ok" ) );
										
									mailLineCnt++;
									
									if ( recMessage == "." ) {
										printf( "\r\n--------------------------------------\r\nMessage [%d]\r\n--------------------------------------\r\n", mailMsgCnt ); 
										mailMsgCnt++;
										mailLineCnt = 0;
										
										printf( "%s\n", buffer.c_str( ) );
										buffer = "";
									} else {
										switch ( mailLineCnt ) {
											case 1:
												buffer += "Date: " + recMessage + "\n";
												break;
											case 2:
												buffer += "To: " + recMessage + "\n";
												break;
											case 3:
												buffer += "From: " + recMessage + "\n\n";
												break;
											default:
												encrypt_decrypt( recMessage, ENCRYPTION_KEY, "D" );
												buffer += recMessage;
												if ( recMessage != "\n" )
													buffer += "\n";
												break;
										}
									}
									result = client.RecvData( recMessage );
									if ( result == -1 ) {
										printf("[Error]: %s\n", recMessage.c_str( ) );
										client.CloseConnection( );
									}	
								}
							}	
							
							if ( msg.compare( "QUIT" ) == 0 ) {
								client.CloseConnection( );
								printf( "Connection closed.\n" );
								break; // client requested to quit
							}
						}
					} else { // not 250
						printf( "%d %s\r\n", atoi( recMessage.substr( 0, 3 ).c_str( ) ), ErrorCode::getCodeDescription( (ErrorCode::Code)atoi( recMessage.substr( 0, 3 ).c_str( ) ) ) );
					}
				} else { // not 220
					printf( "%d %s\r\n", atoi( recMessage.substr( 0, 3 ).c_str( ) ), ErrorCode::getCodeDescription( (ErrorCode::Code)atoi( recMessage.substr( 0, 3 ).c_str( ) ) ) );
				}
			} else {
				printf("Error connecting as relay server to %s on port %d\n", serverAddress.c_str( ), port );
			}
		} else {
			printf( "Invalid Port: %d.\n Provide the port to connect to (1-65535)...\r\n", port );
		}		
	} else {
		printf( "Provide the IPaddress (127.0.0.1 if local) and port to connect to (1-65535)...\r\n" );
	}
	return 0;
}
