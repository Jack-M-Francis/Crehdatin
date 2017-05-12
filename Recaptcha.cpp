#include "Recaptcha.h"

bool validateRecaptcha(std::string response, std::string ip){
	
	curlpp::Cleanup cleaner;
	curlpp::Easy request;
	
	request.setOpt(new curlpp::options::Url("https://www.google.com/recaptcha/api/siteverify")); 
    
    {
		// Forms takes ownership of pointers!
		curlpp::Forms formParts;
		formParts.push_back(new curlpp::FormParts::Content("secret", "6LeYjR4UAAAAAI8mgCXgA87Mi2_AFSU5b6aV9Wod"));
		formParts.push_back(new curlpp::FormParts::Content("response", response));
		formParts.push_back(new curlpp::FormParts::Content("remoteip", ip));
		
		request.setOpt(new curlpp::options::HttpPost(formParts)); 
    }
    
    std::stringstream outputStream;
    outputStream << request;
    std::string output = outputStream.str();
    
    return output.find("\"success\": true,") != std::string::npos;
}
