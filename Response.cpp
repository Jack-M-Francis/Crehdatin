#include "Response.h"

void sendStatusHeader(std::ostream& fcgiOut, StatusCode code){
	fcgiOut << "Status: " << (std::size_t)code << "\r\n";
}

void sendCookieHeader(std::ostream& fcgiOut, std::string cookieName, std::string cookieValue){
	fcgiOut << "Set-Cookie: " << cookieName << '=' << cookieValue << "; Secure; Path=/; SameSite=Strict; Domain=" << Config::getDomain() << ";max-age=" << 31536000 << "\r\n";
}

void sendDeleteCookieHeader(std::ostream& fcgiOut, std::string cookieName){
	fcgiOut << "Set-Cookie: " << cookieName << "=; Secure; Path=/; SameSite=Strict; Domain=" << Config::getDomain() << ";expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n";
}

void sendLocationHeader(std::ostream& fcgiOut, std::string location){
	fcgiOut << "Location: " << location << "\r\n";
}

void sendHtmlContentTypeHeader(std::ostream& fcgiOut){
	fcgiOut << "Content-type: text/html; charset=utf-8\r\n";
}

void finishHttpHeader(std::ostream& fcgiOut){
	fcgiOut << "\r\n";
}