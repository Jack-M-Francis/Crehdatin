#include "HandleDismissReports.h"

void handleDismissReports(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		handleDismissReportsErrorPage(fcgi, data, "Cannot dismiss a report in a subdatin that doesn't exist");
	}
	else{
		std::string authToken;
		if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
			|| authToken != data->authToken){
			handleDismissReportsErrorPage(fcgi, data, "Invalid Authentication Token");
			return;
		}
		
		if(hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
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
			std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("DELETE FROM reports WHERE subdatinId = ? AND threadId = ? AND commentId IS NULL"));
			prepStmt->setInt64(1, subdatinId);
			prepStmt->setString(2, threadId);
			prepStmt->execute();
		}
		else{
			std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("DELETE FROM reports WHERE subdatinId = ? AND threadId = ? AND commentId = ?"));
			prepStmt->setInt64(1, subdatinId);
			prepStmt->setString(2, threadId);
			prepStmt->setString(3, commentId);
			prepStmt->execute();
		}
		
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/reports");
		finishHttpHeader(fcgi->out);
	}
}

void handleDismissReportsErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}