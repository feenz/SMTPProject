#include <vector>
#include <string>

class Message {
	public:
		Message ( );
		~Message( );
		// ---------- SETTERS ----------
		void setFromAddress( std::string );
		void setToAddress( std::string );
		void setSubject( std::string );
		void setMessageBody( std::string );
		void setDate( std::string, std::string );

		// ---------- GETTERS ----------
		std::string getFromAddress( void );
		std::string getToAddress( void );
		std::string getSubject( void );
		std::string getMessageBody( void );
		std::string getTimeStamp( void );

	private:
		std::string _fromAddress;
		std::string _toAddress;
		std::string _subject;
		std::string _messageBody;
		std::string _date;
		std::string _time;
};
