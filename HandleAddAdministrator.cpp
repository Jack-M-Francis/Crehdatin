#include "HandleAddAdministrator.h"

void handleAddAdministrator(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		createAddAdministratorErrorPage(fcgi, data, "Invalid Authentication Token");
		return;
	}
	
	if(!hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId))){
		createAddAdministratorErrorPage(fcgi, data, "You do not have the correct permissions to do this");
		return;
	}
	
	std::string userName;
	switch(getPostValue(fcgi->cgi, userName, "userName", Config::getMaxNameLength(), InputFlag::AllowStrictOnly)){
	default:
		createAddAdministratorErrorPage(fcgi, data, "Unknown Username Error");
		return;
	case InputError::IsTooLarge:
		createAddAdministratorErrorPage(fcgi, data, "Username Too Long");
		return;
	case InputError::IsEmpty:
		createAddAdministratorErrorPage(fcgi, data, "Username Cannot Be Empty");
		return;
	case InputError::ContainsNonNormal: case InputError::ContainsNewLine:
		createAddAdministratorErrorPage(fcgi, data, "Username Can Only Contain Letters, Numbers, Hyphen, and Underscore");
		return;
	case InputError::NoError:
		break;
	}
	
	int64_t userId = getUserId(data->con, userName);
	
	if(userId == -1){
		createAddAdministratorErrorPage(fcgi, data, "The specified user does not exist");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("UPDATE users SET userPosition = 'administrator' WHERE id = ?"));
	prepStmt->setInt64(1, userId);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/controlPanel");
	finishHttpHeader(fcgi->out);
}

void createAddAdministratorErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	std::cout << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}









