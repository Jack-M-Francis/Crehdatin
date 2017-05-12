#include "HandleDismissReports.h"

void handleDismissReports(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		handleDismissReportsErrorPage(fcgi, data, "Invalid Authentication Token");
		return;
	}
	
	if(!Config::hasDeletePermissions(data->userPosition)){
		handleDismissReportsErrorPage(fcgi, data, "You Do Not Have The Correct Permissions To Do This");
		return;
	}
	
	std::string threadId;
	if(getPostValue(fcgi->cgi, threadId, "threadId", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError){
		handleDismissReportsErrorPage(fcgi, data, "Invalid Thread Id");
		return;
	}
	
	std::string commentId;
	if(getPostValue(fcgi->cgi, commentId, "commentId", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError){
		handleDismissReportsErrorPage(fcgi, data, "Invalid Comment Id");
		return;
	}
	
	if(commentId == "-1"){
		sql::PreparedStatement* prepStmt = data->con->prepareStatement("DELETE FROM reports WHERE threadId = ? AND commentId IS NULL");
		prepStmt->setString(1, threadId);
		prepStmt->execute();
		delete prepStmt;
	}
	else{
		sql::PreparedStatement* prepStmt = data->con->prepareStatement("DELETE FROM reports WHERE threadId = ? AND commentId = ?");
		prepStmt->setString(1, threadId);
		prepStmt->setString(2, commentId);
		prepStmt->execute();
		delete prepStmt;
	}
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/reports");
	finishHttpHeader(fcgi->out);
}

void handleDismissReportsErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}