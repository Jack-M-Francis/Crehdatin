#include "HandleChangePassword.h"

void handleChangePassword(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		createSettingsPage(fcgi, data, "", "You Must be Logged In");
		return;
	}
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		createSettingsPage(fcgi, data, "", "Invalid Authentication Token");
		return;
	}
	
	std::string oldPassword;
	switch(getPostValue(fcgi->cgi, oldPassword, "oldPassword", Config::getMaxPasswordLength(), InputFlag::DontCheckInputContents)){
	default:
		createSettingsPage(fcgi, data, "", "Unknown Old Password Error");
		return;
	case InputError::IsTooLarge:
		createSettingsPage(fcgi, data, "", "Old Password Too Long");
		return;
	case InputError::IsEmpty:
		createSettingsPage(fcgi, data, "", "Old Password Cannot Be Empty");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string newPassword;
	switch(getPostValue(fcgi->cgi, newPassword, "newPassword", Config::getMaxPasswordLength(), InputFlag::DontCheckInputContents)){
	default:
		createSettingsPage(fcgi, data, "", "Unknown New Password Error");
		return;
	case InputError::IsTooLarge:
		createSettingsPage(fcgi, data, "", "New Password Too Long");
		return;
	case InputError::IsEmpty:
		createSettingsPage(fcgi, data, "", "New Password Cannot Be Empty");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string repeatNewPassword;
	switch(getPostValue(fcgi->cgi, repeatNewPassword, "repeatNewPassword", Config::getMaxPasswordLength(), InputFlag::DontCheckInputContents)){
	default:
		createSettingsPage(fcgi, data, "", "Unknown Repeated Password Error");
		return;
	case InputError::IsTooLarge:
		createSettingsPage(fcgi, data, "", "Repeated Password Too Long");
		return;
	case InputError::IsEmpty:
		createSettingsPage(fcgi, data, "", "Repeated Password Cannot Be Empty");
		return;
	case InputError::NoError:
		break;
	}
	
	if(newPassword != repeatNewPassword){
		createSettingsPage(fcgi, data, "", "Passwords Do Not Match");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT passwordHash, passwordSalt FROM users WHERE id = ?"));
	prepStmt->setInt64(1, data->userId);
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	
	{
		std::string passwordHash = res->getString("passwordHash");
		std::string passwordSalt = res->getString("passwordSalt");
		
		if(passwordHash != generateSecureHash(oldPassword, passwordSalt)){
			createSettingsPage(fcgi, data, "", "Incorrect Current Password");
			return;
		}
	}
	
	std::string newSalt = generateRandomToken();
	std::string newHash = generateSecureHash(newPassword, newSalt);
	
	prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement("UPDATE users SET passwordHash = ?, passwordSalt = ? WHERE id = ?"));
	prepStmt->setString(1, newHash);
	prepStmt->setString(2, newSalt);
	prepStmt->setInt64(3, data->userId);
	prepStmt->execute();
	
	createPageHeader(fcgi, data);
	fcgi->out << "<p>Successfully changed password!</p><a href='https://" << Config::getDomain() << "/settings'>Back to settings</a>";
	createPageFooter(fcgi, data);
}









