#include "HandleDeleteThread.h"

void handleDeleteThread(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		handleDeleteThreadErrorPage(fcgi, data, "Invalid Authentication Token");
		return;
	}
	
	if(!Config::hasDeletePermissions(data->userPosition)){
		handleDeleteThreadErrorPage(fcgi, data, "You Do Not Have The Correct Permissions To Do This");
		return;
	}
	
	std::string threadId = percentDecode(parameters[0]);
	
	sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT id FROM threads WHERE id = ?");
	prepStmt->setString(1, threadId);
	sql::ResultSet* res = prepStmt->executeQuery();
	res->beforeFirst();
	
	if(res->next()){
		delete res;
		delete prepStmt;
		
		sql::PreparedStatement* prepStmtB = data->con->prepareStatement("DELETE FROM threads WHERE id = ?");
		prepStmtB->setString(1, threadId);
		prepStmtB->execute();
		delete prepStmtB;
		
		std::string seeAlso;
		InputError error = getPostValue(fcgi->cgi, seeAlso, "seeAlso", Config::getUniqueTokenLength(), InputFlag::AllowNonNormal);
		
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		if(error != InputError::NoError || seeAlso.size() == 0){
			sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/");
		}
		else{
			sendLocationHeader(fcgi->out, seeAlso);
		}
		finishHttpHeader(fcgi->out);
	}
	else{
		delete res;
		delete prepStmt;
		handleDeleteThreadErrorPage(fcgi, data, "This Thread Does Not Exist");
	}
}

void handleDeleteThreadErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}