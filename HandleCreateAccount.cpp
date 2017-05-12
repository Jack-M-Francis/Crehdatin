#include "HandleCreateAccount.h"

void handleCreateAccount(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId != -1){
		createLoginPage(fcgi, data, "", "You Are Already Logged In");
		return;
	}
	
	std::string captcha;
	if(getPostValue(fcgi->cgi, captcha, "g-recaptcha-response", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| !validateRecaptcha(captcha, fcgi->env->getRemoteAddr())){
		createLoginPage(fcgi, data, "", "Incorrect Captcha");
		return;
	}
	
	std::string userName;
	switch(getPostValue(fcgi->cgi, userName, "userName", Config::getMaxNameLength(), InputFlag::AllowStrictOnly)){
	default:
		createLoginPage(fcgi, data, "", "Unknown Username Error");
		return;
	case InputError::IsTooLarge:
		createLoginPage(fcgi, data, "", "Username Too Long");
		return;
	case InputError::IsEmpty:
		createLoginPage(fcgi, data, "", "Username Cannot Be Empty");
		return;
	case InputError::ContainsNonNormal: case InputError::ContainsNewLine:
		createLoginPage(fcgi, data, "", "Username Can Only Contain Letters, Numbers, Hyphen, and Underscore");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string password;
	switch(getPostValue(fcgi->cgi, password, "password", Config::getMaxPasswordLength(), InputFlag::DontCheckInputContents)){
	default:
		createLoginPage(fcgi, data, "", "Unknown Password Error");
		return;
	case InputError::IsTooLarge:
		createLoginPage(fcgi, data, "", "Password Too Long");
		return;
	case InputError::IsEmpty:
		createLoginPage(fcgi, data, "", "Password Cannot Be Empty");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string repeatPassword;
	switch(getPostValue(fcgi->cgi, repeatPassword, "repeatPassword", Config::getMaxPasswordLength(), InputFlag::DontCheckInputContents)){
	default:
		createLoginPage(fcgi, data, "", "Unknown Repeated Password Error");
		return;
	case InputError::IsTooLarge:
		createLoginPage(fcgi, data, "", "Repeated Password Too Long");
		return;
	case InputError::IsEmpty:
		createLoginPage(fcgi, data, "", "Repeated Password Cannot Be Empty");
		return;
	case InputError::NoError:
		break;
	}
	
	if(password != repeatPassword){
		createLoginPage(fcgi, data, "", "Passwords Do Not Match");
		return;
	}
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		createLoginPage(fcgi, data, "Invalid Authentication Token", "");
		return;
	}
	
	std::string salt = generateRandomToken();
	std::string hash = generateSecureHash(password + salt);
	
	sql::PreparedStatement* prepStmt = data->con->prepareStatement("INSERT INTO users "
				"(userName, passwordHash, passwordSalt) SELECT ?, ?, ? FROM DUAL WHERE NOT EXISTS "
				"(SELECT id FROM users WHERE userName=?)");
	prepStmt->setString(1, userName);
	prepStmt->setString(2, hash);
	prepStmt->setString(3, salt);
	prepStmt->setString(4, userName);
	prepStmt->execute();
	delete prepStmt;
	
	sql::ResultSet* res = data->stmt->executeQuery("SELECT ROW_COUNT()");
	
	res->first();
	
	int64_t rowCount = res->getInt64(1);
	
	delete res;
	
	if(rowCount == 0){
		createLoginPage(fcgi, data, "", "User With That Name Already Exists");
		return;
	}
	
	res = data->stmt->executeQuery("SELECT LAST_INSERT_ID()");
	
	res->first();
	
	int64_t userId = res->getInt64(1);
	
	delete res;
	
	prepStmt = data->con->prepareStatement("UPDATE sessions SET userId=?, shownId=NULL WHERE sessionToken=?");
	prepStmt->setInt64(1, userId);
	prepStmt->setString(2, data->sessionToken);
	prepStmt->execute();
	delete prepStmt;
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/");
	finishHttpHeader(fcgi->out);
}

void createCreateAccountPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/login");
	finishHttpHeader(fcgi->out);
}










