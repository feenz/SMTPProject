#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

/**
 * @author Kyle Feeney (KDf5506)
 * This is a utilities file containing some comonly used functions
 * @version 03.07.2013
 *
 */

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <algorithm>
#include <sstream>
#include <time.h>

using namespace std;

/**
 * Function allows a string to be split on any set of delimiting characters
 * and adds them to a vector of string elements
 * @param _elements is a collection of resulting string elements
 * @param _content is the string to parse
 * @param _delimiter is the string delimiter to split on
 * @return 0 if not elements added to the vector, and 1 otherwise
 **/
inline int split ( vector<string>& _elements, string _content, string _delimiter ) {
    int start = 0;
    int position = 0;

	if ( _delimiter.empty( ) ) {
        _elements.push_back( _content );
        return 1;
	}

    for ( unsigned int index = 0; index < _content.length( )-1; index++ ) {
        position = _content.find( _delimiter, start );
        if ( position != std::string::npos ) {
            _elements.push_back( _content.substr( start, ( position - start ) ) );
            start = position + _delimiter.size( );
        } else {
            _elements.push_back( _content.substr( start ) );
            break;
        }
    }

    return _elements.size( );
};

/**
 * Function converts a IP address in dotted format to a unsigned long
 * @param ip is the IP address in dotted string format
 * @return the IP in long format
 **/
inline unsigned long convertIpToLong ( std::string _ip ) {
    unsigned long longIp;
    std::vector<string> elements;

    int result = split( elements, _ip, "." ); // split on (.)
    if ( result == 4 ) { // 4 octets
        longIp = (atoi(elements[3].c_str( )))                       /* octet 4 */
            + ( 256 * (atoi(elements[2].c_str( ))) )                /* octet 3 */
            + ( 256 * 256 * (atoi(elements[1].c_str( ))) )          /* octet 2 */
            + ( 256 * 256 * 256 * (atoi(elements[0].c_str( ))) );   /* octet 1 */
    }
    return longIp;
};

/**
 * Function converts a long to an IP address in dotted string format
 * @param longIp is the long
 * @return the IP in dotted string format
 **/
inline string convertLongToIp ( unsigned long _longIp ) {
    double temp;
    stringstream ss;
    int quadA, quadB, quadC, quadD;

    temp = ( _longIp / 256.0 );
    quadD = (int)( 256 * ( temp - (int)temp ));

    temp = (temp) / 256.0;
    quadC = (int)( 256 * ( temp - (int)(temp) ));

    temp = ( (int)(temp) ) / 256.0;
    quadB = (int)( 256 * ( temp - (int)(temp) ));

    quadA = (int)(temp);

    // convert back to dotted string format
    ss << quadA << "." << quadB << "." << quadC << "." << quadD;

    return ss.str( );
};

/**
 * Function sends message to console and waits for input
 * @param prompt is the message to display
 * @return input as a string
 **/
inline string readInput( string _prompt ) {
    string line;
    cout << _prompt;
    cin  >> line;
    while ( line.length() <= 0 )     {
        cout << "Invalid input - must have at least one character" << endl;
        cin.clear( );
        cin.ignore(INT_MAX, '\n');
        line = readInput( _prompt );
    }

    return line;
};

/**
 * Function prompts for a filename and attempts to open the
 * given file.
 * @param prompt is the prompt written to console
 * @param fin is a reference to an input file stream object
 **/
inline void openInputFile( string _prompt, ifstream& _fileObj ) {
    bool isOpen = false;
    string filename;

    while ( !isOpen ) {
        filename = readInput( _prompt );

        _fileObj.open( filename.c_str( ), ios::in );
        if ( _fileObj.is_open( ) ) {
            isOpen = true;
            cin.ignore( INT_MAX, '\n' );
        } else {
            cout << "Unable to open file " << filename << endl;
            cin.clear( );
            cin.ignore( INT_MAX, '\n' );
        }
    }
};

/**
 * Function prompts for a filename and attempts to open the
 * given file.
 * @param prompt is the prompt written to console
 * @param fin is a reference to an ouput file stream object
 **/
inline void openOutputFile( string _prompt, ofstream& _fileObj ) {
    bool isOpen = false;
    string filename;

    while ( !isOpen ) {
        filename = readInput( _prompt );

        _fileObj.open( filename.c_str( ), ios::out );
        if ( _fileObj.is_open( ) ) {
            isOpen = true;
            cin.ignore( INT_MAX, '\n' );
        } else {
            cout << "Unable to open file " << filename << endl;
            cin.clear( );
            cin.ignore( INT_MAX, '\n' );
        }
    }
};

/**
 * Function prompts for a filename and attempts to open the
 * given file, if it exists, the data will be appended.
 * @param prompt is the prompt written to console
 * @param fin is a reference to an ouput file stream object
 **/
inline void appendOutputFile( string _prompt, ofstream& _fileObj ) {
    bool isOpen = false;
    string filename;

    while ( !isOpen ) {
        filename = readInput( _prompt );

        _fileObj.open( filename.c_str( ), ios::out | ios::app );
        if ( _fileObj.is_open( ) ) {
            isOpen = true;
            cin.ignore( INT_MAX, '\n' );
        } else {
            cout << "Unable to open file " << filename << endl;
            cin.clear( );
            cin.ignore( INT_MAX, '\n' );
        }
    }
};

inline void openInputFile( ifstream& _fin, string _filename, const ios::openmode& _mode ) {
    _fin.open( _filename.c_str( ), _mode );
};

inline void openOutputFile ( ofstream& _fout, string _filename, const ios::openmode& _mode ) {
    _fout.open( _filename.c_str( ), _mode );
};

inline void appendOutputFile( ofstream& _fout, string _filename, const ios::openmode& _mode_out, const ios::openmode& _mode_app ) {
    _fout.open( _filename.c_str( ), _mode_out | _mode_app );
};

/**
 * A struct containing Date Time Message properties
 * @var currentDate is the current date passed in
 * @var currentTime is the current time passed in
 * @var textMessage is the text message passed in
 **/
struct DateTimeMsg {
    string userId;
    string currentDate;
    string currentTime;
    string textMessage;

    DateTimeMsg( string uid, string curDate, string curTime, string txtMsg ) : userId( uid ), currentDate( curDate ), currentTime( curTime ), textMessage( txtMsg ) { }
};

/**
 * Function takes the local time and sets the referenced parameters to
 * that portion returned by the ctime method
 * @param date is a reference to a string containing the Mmm dd yyyy
 * @param time is a reference to a string containing the hh:mm:ss
 **/
inline void getDateTime( string& _date, string& _time ) {
    time_t currentTime;
    time (&currentTime); // push current time into variable
    const string EMPTY = " ";

    vector<string> elements;
    string content = ctime( &currentTime );
    content.erase( std::remove( content.begin( ), content.end( ), '\n'), content.end( ) );
    split( elements, content, EMPTY );
    _date = elements.at(1) + EMPTY + elements.at(2) + EMPTY + elements.at(4);
    _time = elements.at(3);
};

/**
 * Function writes a DateTimeMsg object to file in a csv format
 * @param dtm is the DateTimeMsg struct
 * @param fileObj is the out file stream object containing the file
 **/
inline void writeData ( DateTimeMsg& _dtm, ofstream& _fileObj ) {
    stringstream ss;
    ss << _dtm.currentDate << "," << _dtm.currentTime << "," << _dtm.textMessage << "\n";
    _fileObj << ss.str( );
};

#endif // UTILS_H_INCLUDED
