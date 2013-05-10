#include "Codes.h"

namespace ErrorCode {
	// Return a detailed description given a Code
	char* getCodeDescription( ErrorCode::Code _code ) {
		switch ( _code ) {
			case ErrorCode::CANNOT_OPEN_CON: /* 101 errors - usually point to port in use or non existent */
				return const_cast<char *>("Cannot open connection");
				break;
			case ErrorCode::CONNECT_REFUSED: /* 111 errors - usually point to network misconfiguration */
				return const_cast<char *>("Connection refused");
				break;
			case ErrorCode::SYSTEM_HLP_RPLY: /* 211 errors - are usually used for requesting status or help from server */
				return const_cast<char *>("System help reply");
				break;
			case ErrorCode::SRVC_RUN_SUCCES: /* 220 status - initial message from server after connection <server name> <message> */
				return const_cast<char *>("Service running");
				break;
			case ErrorCode::SERVICE_CLOSING: /* 221 errors - server is ending the mail session and going back into listen mode */
				return const_cast<char *>("Service closing");
				break;
			case ErrorCode::AUTH_SUCCESSFUL: /* 235 status - client successfully authenticated with server */
				return const_cast<char *>("Authentication successful");
				break;
			case ErrorCode::REQUEST_SUCCESS: /* 250 status - server has successfuly processed and transmitted the message request */
				return const_cast<char *>("Requested process successfully complete");
				break;
			case ErrorCode::USRNOTLCL_RELAY: /* 251 status - user not local, the server will relay your message */
				return const_cast<char *>("User is not local, will forward");
				break;
			case ErrorCode::CANT_VERIFY_USR: /* 252 status - the user cannot be verified, server will attempt to deliver */
				return const_cast<char *>("Cannot verify user address");
				break;
			case ErrorCode::START_MAIL_DATA: /* 354 status - normally in response to DATA - message notifies to end with <CR><LF>.<CR><LF> */
				return const_cast<char *>("Begin DATA, <CR><LF>.<CR><LF> to end");
				break;
			case ErrorCode::SERVC_NOT_AVABL: /* 421 errors - usually becuase of to many concurrent streams OR transient envent */
				return const_cast<char *>("Service not available");
				break;
			case ErrorCode::OUT_MSG_TIMEOUT: /* 447 errors - the outgoing message timed out becuase of recipient */
				return const_cast<char *>("Outgoing message timed out");
				break;
			case ErrorCode::CMD_SYNTAX_EROR: /* 500 errors - the command sent was not recognized as a valid SMTP or ESMTP command */
				return const_cast<char *>("Command not recognized");
				break;
			case ErrorCode::AUTH_UNSUCESFUL: /* 504 errors - client failed to quthenticate */
				return const_cast<char *>("Unrecognized authentication type");
				break;
		}
	}
}
