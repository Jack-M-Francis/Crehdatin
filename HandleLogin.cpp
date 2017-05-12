#include "HandleLogin.h"

void handleLogin(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId != -1){
		createLoginPage(fcgi, data, "", "You Are Already Logged In");
		return;
	}
	
	std::string userName;
	switch(getPostValue(fcgi->cgi, userName, "userName", Config::getMaxNameLength(), InputFlag::AllowStrictOnly)){
	default:
		createLoginPage(fcgi, data, "Unknown Username Error", "");
		return;
	case InputError::IsTooLarge:
		createLoginPage(fcgi, data, "Username Too Long", "");
		return;
	case InputError::IsEmpty:
		createLoginPage(fcgi, data, "Username Cannot Be Empty", "");
		return;
	case InputError::ContainsNonNormal: case InputError::ContainsNewLine:
		createLoginPage(fcgi, data, "Username Can Only Contain Letters, Numbers, Hyphen, and Underscore", "");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string password;
	switch(getPostValue(fcgi->cgi, password, "password", Config::getMaxPasswordLength(), InputFlag::DontCheckInputContents)){
	default:
		createLoginPage(fcgi, data, "Unknown Password Error", "");
		return;
	case InputError::IsTooLarge:
		createLoginPage(fcgi, data, "Password Too Long", "");
		return;
	case InputError::IsEmpty:
		createLoginPage(fcgi, data, "Password Cannot Be Empty", "");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		createLoginPage(fcgi, data, "Invalid Authentication Token", "");
		return;
	}
	
	sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT passwordHash, passwordSalt, id FROM users WHERE userName = ?");
	prepStmt->setString(1, userName);
	
	sql::ResultSet* res = prepStmt->executeQuery();
	res->beforeFirst();
	
	if(!res->next()){
		createLoginPage(fcgi, data, "User Does Not Exist", "");
		return;
	}
	
	std::string passwordHash = res->getString("passwordHash");
	std::string passwordSalt = res->getString("passwordSalt");
	int64_t userId = res->getInt64("id");
	
	delete res;
	delete prepStmt;
	
	if(passwordHash != generateSecureHash(password + passwordSalt)){
		createLoginPage(fcgi, data, "Incorrect Password", "");
		return;
	}
	
	prepStmt = data->con->prepareStatement("UPDATE sessions SET userId=?, shownId=NULL WHERE sessionToken=?");
	prepStmt->setInt64(1, userId);
	prepStmt->setString(2, data->sessionToken);
	prepStmt->execute();
	delete prepStmt;
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/");
	finishHttpHeader(fcgi->out);
}












