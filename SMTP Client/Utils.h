#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdio.h>
#include <string.h>

using namespace std;

void encrypt_decrypt( string &message, int key, const char* action ) {
	//printf( "%s message %s\n", ( ( strcmp( action, "E" ) == 0 ) ? "Encrypting" : "Decrypting" ), message.c_str( ) );
    for ( int index = 0; index < message.length( ); index++  ) {	// loop through each character
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

#endif // UTILS_H_INCLUDED
