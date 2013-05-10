#include "message.h"

Message::Message ( ) { };

Message::~Message( ) { /* cleanup */ };

void Message::setFromAddress( std::string  from ) { this->_fromAddress=from; };
void Message::setToAddress( std::string to ) { this->_toAddress=to; };
void Message::setSubject( std::string subject ) { this->_subject=subject; };
void Message::setMessageBody( std::string body ) { this->_messageBody=body; };
void Message::setDate( std::string date, std::string time ) { this->_date=date; this->_time=time; };

std::string Message::getFromAddress( ) { return this->_fromAddress; };
std::string Message::getToAddress( ) { return this->_toAddress; };
std::string Message::getSubject( ) { return this->_subject; };
std::string Message::getMessageBody( ) { return this->_messageBody; };
std::string Message::getTimeStamp( ) { return this->_date + " " + this->_time; };


