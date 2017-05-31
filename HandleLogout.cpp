#include "HandleLogin.h"

void handleLogout(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/");
		finishHttpHeader(fcgi->out);
		return;
	}
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError){
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/");
		finishHttpHeader(fcgi->out);
		return;
	}
	
	if(data->authToken != authToken){
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/");
		finishHttpHeader(fcgi->out);
		return;
	}
	
	std::string shownId = generateRandomToken().substr(0, 12);
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE sessions SET userId=NULL, shownId=? WHERE sessionToken=?"));
	prepStmt->setString(1, shownId);
	prepStmt->setString(2, data->sessionToken);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/");
	finishHttpHeader(fcgi->out);
}












