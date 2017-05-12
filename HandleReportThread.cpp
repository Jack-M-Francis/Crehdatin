#include "HandleReportThread.h"

void handleReportThread(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		handleReportThreadErrorPage(fcgi, data, "Invalid Authentication Token");
		return;
	}
	
	if((data->userId == -1 && data->currentTime - data->lastPostTime < Config::anonReportingTimeout()) || 
		(data->userId != -1 && data->currentTime - data->lastPostTime < Config::userReportingTimeout())){
		handleReportThreadErrorPage(fcgi, data, "You are posting/reporting too much, wait a little longer before trying again");
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
		
		std::string reason;
		switch(getPostValue(fcgi->cgi, reason, "reason", Config::getMaxReportLength(), InputFlag::AllowNonNormal)){
		default:
			handleReportThreadErrorPage(fcgi, data, "Unknown Report Error");
			return;
		case InputError::IsTooLarge:
			handleReportThreadErrorPage(fcgi, data, "Report Too Long");
			return;
		case InputError::IsEmpty:
			handleReportThreadErrorPage(fcgi, data, "Report Cannot Be Empty");
			return;
		case InputError::ContainsNewLine:
			handleReportThreadErrorPage(fcgi, data, "Report Cannot Contain Newlines");
			return;
		case InputError::NoError:
			break;
		}
		
		sql::PreparedStatement* prepStmtB = data->con->prepareStatement("INSERT INTO reports (reason, threadId, ip, userId) VALUES (?, ?, ?, ?)");
		prepStmtB->setString(1, reason);
		prepStmtB->setString(2, threadId);
		prepStmtB->setString(3, fcgi->env->getRemoteAddr());
		if(data->userId == -1){
			prepStmtB->setNull(4, 0);
		}
		else{
			prepStmtB->setInt64(4, data->userId);
		}
		prepStmtB->execute();
		delete prepStmtB;
		
		setLastPostTime(fcgi, data);
		
		createPageHeader(fcgi, data);
		fcgi->out << "Thank You For the Report! <a href='https://" << Config::getDomain() << "/thread/" << threadId << "'>Back to the Thread</a>";
		createPageFooter(fcgi, data);
	}
	else{
		delete res;
		delete prepStmt;
		handleReportThreadErrorPage(fcgi, data, "This Thread Does Not Exist");
	}
}

void handleReportThreadErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}