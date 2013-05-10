#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include "Codes.h"
#include "Utils.h"
#include "Socket.h"
#include "Message.h"
#include "ThreadUtils.h"


using namespace std;

string serverIpAddr;
string serverHostname;
HANDLE mailQueueMutex;
const int SERVER_PORT = 31000;
static int msgcnt = 1;
static std::queue<Message*> messageQueue;
const char* const ids[] = { "kfeeney", "alozado", "tbacon" };

DWORD WINAPI receive_cmds( LPVOID lpParam ) {
	bool failthesebitches = false;
	int result = 0;
	string sndMessage = "";
	string recMessage = "";
	DWORD dwWaitResult;
	string username;
	bool guestAccount = false;
	string client_hostname;
	Message mailMessage;
	bool emailComplete[4] = { 
		false, /* mail from */
		false, /* rcpt to */  
		false, /* date */
		false  /* data */
	};
	
	ThreadUtils connectedClient;
	SOCKET sock = *(SOCKET *)lpParam;
	connectedClient.setSocket( sock );
	
	printf( "Command thread created...\r\n" );
	
	// 220
	connectedClient.sendResponse( ErrorCode::SRVC_RUN_SUCCES, string( serverHostname + " SMTP Server ready" ) );

	result = connectedClient.recvData( recMessage );
	printf( "Received: %s\r\n", recMessage.c_str( ) );
	
	bool good = result != -1 ? true : false;  
		
	if ( good ) {
		while ( recMessage != "QUIT" ) {		
			if ( recMessage.substr( 0, 4 ) == "HELO" ) {
				connectedClient.sendResponse( ErrorCode::REQUEST_SUCCESS, string( "Hello " + client_hostname + ", I am glad to meet you!" ) );
			} else if ( recMessage.substr( 0, 9 ) == "MAIL FROM" ) {
				int beg = recMessage.find_first_of( "<" );
			  	int end = recMessage.find_first_of( ">" );
			  	if ( beg == std::string::npos || end == std::string::npos ) {
			  		connectedClient.sendResponse( ErrorCode::CMD_SYNTAX_EROR, ErrorCode::getCodeDescription( ErrorCode::CMD_SYNTAX_EROR ) );
				} else {
					connectedClient.sendResponse( ErrorCode::REQUEST_SUCCESS, "Ok" );
					mailMessage.setFromAddress( recMessage.substr( beg+1, ((end-1)-beg) ) );
					emailComplete[0] = true; // mail from completed
				}
			} else if ( recMessage.substr( 0, 4) == "VRFY" ) {
				string user = recMessage.substr( 5 );
				for ( int i = 0; i < sizeof( ids )/sizeof(const char*); i++ ) {
					if ( user.compare( ids[i] ) == 0 ) {
						username = user;
						guestAccount = false; 
						break;
					} else {
						username = "guest";
						guestAccount = true;  
					}
				}
				connectedClient.sendResponse( ErrorCode::REQUEST_SUCCESS, string( "<" + username + "@" + serverIpAddr + ">" ) ); 
			} else if ( recMessage.substr( 0, 7 ) == "RCPT TO" ) {
				int beg = recMessage.find_first_of( "<" );
			  	int end = recMessage.find_first_of( ">" );
			  	if ( beg == std::string::npos || end == std::string::npos ) {
			  		connectedClient.sendResponse( ErrorCode::CMD_SYNTAX_EROR, ErrorCode::getCodeDescription( ErrorCode::CMD_SYNTAX_EROR ) );
				} else {
					connectedClient.sendResponse( ErrorCode::REQUEST_SUCCESS, "Ok" );
					mailMessage.setToAddress( recMessage.substr( beg+1, ((end-1)-beg) ) );
					emailComplete[1] = true; // rcpt to completed
				}
			} else if ( recMessage.substr( 0, 5 ) == "INBOX" ) {
				if ( !guestAccount ) { // valid user
					ifstream inFile( string( username + ".txt").c_str( ) );
					if ( !inFile.is_open( ) ) {
						connectedClient.sendResponse( ErrorCode::INBOX_UNAVAILBL, "Inbox is empty..." );
					} else {
						connectedClient.sendResponse( ErrorCode::REQUEST_SUCCESS, string ( "Retreiving " + username + "'s Inbox..." ) );
						
						getline( inFile, sndMessage );
						while ( !inFile.eof( ) ) {
							if ( sndMessage == "" )
								sndMessage = "\n";
							connectedClient.sendData( sndMessage );
							printf( "Sent: %s\n", sndMessage.c_str( ) );
							result = connectedClient.recvData( recMessage );
							if ( result == -1 )
								break;
								
							getline( inFile, sndMessage );
						}
						connectedClient.sendData( "EOF" );
						inFile.close( );
					}
				} else {
					connectedClient.sendResponse( ErrorCode::INBOX_UNAVAILBL, "Inbox unavailable under guest account." );
				}
			} else if ( recMessage.substr( 0, 4 ) == "DATA" ) {
				connectedClient.sendResponse( ErrorCode::START_MAIL_DATA, "End data with <CR><LF>.<CR><LF>" );
				
				result = connectedClient.recvData( recMessage );
				string body;
				while ( true ) {
					if ( result == -1 )
						break;
					printf( "Received: %s\r\n", recMessage.c_str( ) );
					if ( recMessage == "." )
						break;
					body.append( recMessage + "\n" );

					result = connectedClient.recvData( recMessage );
				}

				printf( "Data complete...\n" );
				
				mailMessage.setMessageBody( body );
				emailComplete[3] = true; // data
				
				// date time stamp
				string date, time;
				getDateTime( date, time );
				mailMessage.setDate( date, time );
				emailComplete[2] = true; // timestamp
				
				bool emailDone = true; // verify all information is complete
				for ( int index = 0; index < 4; index++ ) {
					if ( !emailComplete[index] )
						emailDone = false;
				}
				
				if ( emailDone ) {
					dwWaitResult = WaitForSingleObject( mailQueueMutex, INFINITE );
					
					if ( dwWaitResult == WAIT_OBJECT_0 ) {
						Message *pMsg;
						pMsg = &mailMessage;
						messageQueue.push( pMsg ); // push the message into the queue
						
						ReleaseMutex( mailQueueMutex );
					}
					connectedClient.sendResponse( ErrorCode::REQUEST_SUCCESS, string( "Ok: Message Queued " + msgcnt ) );
					msgcnt++; // not really doing anything with this right now... Is it neccessary???
				} else {
					connectedClient.sendResponse( ErrorCode::CMD_SYNTAX_EROR, ErrorCode::getCodeDescription( ErrorCode::CMD_SYNTAX_EROR ) );
				}
			} else {
				connectedClient.sendResponse( ErrorCode::CMD_SYNTAX_EROR, ErrorCode::getCodeDescription( ErrorCode::CMD_SYNTAX_EROR ) );
			}
			
			recMessage.clear( );
			result = connectedClient.recvData( recMessage );
			if ( recMessage == "QUIT" || result == -1 ) {
				connectedClient.sendResponse( ErrorCode::SERVICE_CLOSING, "Goodbye!" );
				break;
			}
				
			printf( "Received: %s\r\n", recMessage.c_str( ) );
		}					
	}

	
	if ( recMessage == "QUIT" ) {
		connectedClient.sendResponse( ErrorCode::SERVICE_CLOSING, "Goodbye!" );
	}

	printf( "Client <%s> disconnected...\r\n", client_hostname.c_str( ) );
	printf( "\r\nCommand thread closed...\r\n" );
}

DWORD WINAPI check_msg_queue( LPVOID lpParam ) {
	printf( "\r\nMessage thread created...\r\n" );
	
	DWORD dwWaitResult;
	string recMessage;
	string sndMessage;
	
	while ( true ) {
		dwWaitResult = WaitForSingleObject( mailQueueMutex, INFINITE );
		
		if ( dwWaitResult == WAIT_OBJECT_0 ) {
			if ( !messageQueue.empty( ) ) {
				printf( "Queue size %d\r\n", messageQueue.size( ) );
				Message *pMsg;
				pMsg = messageQueue.front( );
				
				// check if the message is for a user on this server
				bool relayUser = false;
				string toAddress = pMsg->getToAddress( );
				string user = toAddress.substr( 0, toAddress.find( "@" ) );
				string ip = toAddress.substr( toAddress.find( "@" )+1 );
				for ( int i = 0; i < sizeof( ids )/sizeof(const char*); i++ ) {
					if ( user.compare( ids[i] ) == 0 && serverIpAddr.compare( ip ) == 0 ) {
						string ofilename = user+".txt";
						cout << "Message added to Inbox: " << ofilename << endl; 
						// create or append to the users inbox
						ofstream oFile;
						appendOutputFile( oFile, ofilename, std::ios::out, std::ios::app );
						if ( oFile.is_open( ) ) {
							oFile << pMsg->getTimeStamp( ) << "\n";
							oFile << pMsg->getToAddress( ) << "\n";
							oFile << pMsg->getFromAddress( ) << "\n";
							oFile << pMsg->getMessageBody( );
							oFile << "." << "\n";
						}
						oFile.close( );
						relayUser = false;
						break;
					} else {
						relayUser = true;
					}
				}
				
				if ( relayUser ) {
					ClientSocket relayClient;
					
					int result = 0;
					string relayIp = toAddress.substr( toAddress.find( "@" ) + 1); // get the ip address

					relayClient.ConnectToServer( relayIp.c_str( ), SERVER_PORT );
					
					printf( "Connected to server @ %s:%d\r\n", relayIp.c_str( ), SERVER_PORT );
					printf( "Begin relay...\r\n" );
					
					// 220 from server
					result = relayClient.RecvData( recMessage );
					if ( result == -1 ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
						relayClient.CloseConnection( );
					} 
						
					if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::SRVC_RUN_SUCCES ) {
						relayClient.SendData( string( "HELO " + serverHostname ) );
					}
					
					result = relayClient.RecvData( recMessage );
					if ( result == -1 ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
						relayClient.CloseConnection( );
					}
					
					if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::REQUEST_SUCCESS ) {
						relayClient.SendData( "VRFY guest" );
					}
					
					result = relayClient.RecvData( recMessage );
					if ( result == -1 ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
						relayClient.CloseConnection( );
					}
					
					if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::CMD_SYNTAX_EROR || atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::INBOX_UNAVAILBL ) {
						printf( "Invalid account...\n" );
						relayClient.CloseConnection( );
					}
					
					relayClient.SendData( string( "MAIL FROM:<" + pMsg->getFromAddress( ) + ">" ) );
					
					result = relayClient.RecvData( recMessage );
					if ( result == -1 ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
						relayClient.CloseConnection( );
					}
					
					if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) != ErrorCode::REQUEST_SUCCESS ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
					}
					
					relayClient.SendData( string( "RCPT TO:<" + pMsg->getToAddress( ) + ">" ) );
					
					result = relayClient.RecvData( recMessage );
					if ( result == -1 ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
						relayClient.CloseConnection( );
					}
					
					if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) != ErrorCode::REQUEST_SUCCESS ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
					}
					
					relayClient.SendData( "DATA" );
					
					result = relayClient.RecvData( recMessage );
					if ( result == -1 ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
						relayClient.CloseConnection( );
					}
					
					if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) != ErrorCode::START_MAIL_DATA ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
					}
					
					relayClient.SendData( pMsg->getMessageBody( ) );
					
					result = relayClient.RecvData( recMessage );
					if ( result == -1 ) {
						printf( "Error: %s\n", recMessage.c_str( ) );
						relayClient.CloseConnection( );
					}
					
					if ( atoi( recMessage.substr( 0, 3 ).c_str( ) ) == ErrorCode::REQUEST_SUCCESS ) {
						printf( "Message successfully sent.\n" );
					} else {
						printf( "Error sending message...\n" );
					}
					
					if ( result == -1 )
						printf( "Error sending message...\n" );
					
					relayClient.CloseConnection( );
				}
				
				messageQueue.pop( );
				printf( "\r\nQueue size %d\r\n", messageQueue.size( ) );
			}
			ReleaseMutex( mailQueueMutex );
		}
	}
}

int main( ) {
	DWORD thread; 				// for our thread
	WSADATA wsaData;			// winsock
	char* localIp;				// local ip of server
	hostent* localhost;			// local hostname of server
	SOCKET listenSocket;		// our masterSocket(socket that listens for connections)
	struct sockaddr_in server;	// server struct
	SOCKET client;				// socket that we sendrecv data on
	
	
	// start winsock
    int ret = WSAStartup( 0x101 , &wsaData ); // use highest version of winsock avalible
	if( ret != 0 )
		return 0;
		
	// create listening socket		
	listenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( listenSocket == INVALID_SOCKET )
		return 0;
	
	// get loclahost information
	localhost = gethostbyname( "" );
	localIp = inet_ntoa( *(struct in_addr *) *localhost->h_addr_list );
	
	serverIpAddr = localIp;
	serverHostname = localhost->h_name;
       
    // fill in winsock struct ...
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( SERVER_PORT ); 
	 
    printf( "Starting up multi-threaded TCP server\n" );
     
    // bind our socket to a port(port 123)
    printf( "Binding...\n" );
    if( bind( listenSocket, (sockaddr*)&server, sizeof( server ) ) != 0 )
		return 0;
       
    // listen for a connection 
    printf( "Listening...\n" );
    if( listen( listenSocket, 5 ) != 0 )
		return 0;
		
	mailQueueMutex = CreateMutex ( NULL, FALSE, NULL );
	
	CreateThread( NULL, 0, check_msg_queue, (LPVOID)NULL, 0, &thread );
	usleep( 20000 );
	
	struct sockaddr_in from;
    int fromlen = sizeof( from );
    printf( "Accepting connections @ %s:%d\n", localIp, SERVER_PORT );
    while( true ) { // loop forever -- accept connections
    	client = accept( listenSocket, (struct sockaddr*)&from, &fromlen );
    	char* caddr = inet_ntoa( from.sin_addr ); // get connected ip address
     	printf( "\r\nClient connected @ %s:%d\r\n", caddr, SERVER_PORT );
     	    	
     	// create our recv_cmds thread and pass client socket as a parameter
     	CreateThread( NULL, 0, receive_cmds, (LPVOID)&client, 0, &thread ); 
	}
    
	WSACleanup( );
	CloseHandle( mailQueueMutex );
    return 0; // exit
}
