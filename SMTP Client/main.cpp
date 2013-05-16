#include <iostream>
#include <stdio.h>
#include "Socket.h"
#include "Codes.h"

using namespace std;

int main( int argc, char* argv[] ) {
	int port;					// port to connect on passed on command line			
	int result;					// result of RecvData command (-1 for error)
	hostent* localhost;			// stores information about the host for which the app is running on
	ClientSocket client;		// the main socket used for interaction with a server
	string serverAddress;		// the address of the server we connect too
	string recMessage = "";		// used for receiving messages from the server
    string sendMessage = "";	// used for sending messages to the server
   
	if ( argc == 3 ) { // Client.exe <port> <ip address>
		port = atoi( argv[1] );	// get the port from cli
		serverAddress = argv[2]; // get the ip from cli
		if ( port > 0 && port < 65536 ) { // verify port

			// C: attempt to connect
			printf( "Attempting to connect to %s:%d...\r\n\n", serverAddress.c_str( ), port );
			bool connected = client.ConnectToServer( serverAddress.c_str( ), port );

			if ( connected ) { // successfully connected to the server
				localhost = gethostbyname( "" ); // populate hostent structure

				// S: 220 welcome from server
				result = client.RecvData( recMessage );
				if ( result == -1 ) {
					printf("[Error]: %s\n", recMessage.c_str( ) );
					client.CloseConnection( );
				}

				if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::SRVC_RUN_SUCCES ) { // 220 from server
					printf( "%s\r\n", recMessage.c_str( ) );
					sendMessage = ( COMMAND_HELO + " " + string( localhost->h_name ) );
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
							if ( msg.compare( COMMAND_DATA ) == 0 && atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::START_MAIL_DATA ) { // 354 from server
								string line;
							    while ( line != "." ) {
	    							cout << ">";
	    							getline( cin, line );	
												    
							    	if ( line == "" )
							    		line = "\n";
							    		
									if ( line != "." )
										client.Cryptor( line, ENCRYPTION_KEY, "E" ); // encrypt the message and send to server
									client.SendData( line );
							    }

								result = client.RecvData( recMessage );
								if ( result == -1 ) {
									printf("[Error]: %s\n", recMessage.c_str( ) );
									client.CloseConnection( );
								}
								printf( "%s\r\n", recMessage.c_str( ) );
							} 
							
							// client requested INBOX - server will attempt to retrieve INBOX for verified user
							if ( msg.compare( COMMAND_INBOX ) == 0 && atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::REQUEST_SUCCESS ) { // 250 from server
								int mailMsgCnt = 1;		// tracks inbox message count
								int mailLineCnt = 0;	// tracks line per message
								string buffer = "";		// holds message to display
								
								// receive first line of inbox
								result = client.RecvData( recMessage );
								if ( result == -1 ) {
									printf("[Error]: %s\n", recMessage.c_str( ) );
									client.CloseConnection( );
								}	

								// EOF - determined as the standard to notify client that receipt of inbox is complete
								while ( recMessage != COMMAND_EOF ) {
									if ( recMessage != COMMAND_EOF )
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
												client.Cryptor( recMessage, ENCRYPTION_KEY, "D" );
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
							
							if ( msg.compare( COMMAND_QUIT ) == 0 ) {
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
