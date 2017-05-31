#include "HandleReportComment.h"

void handleReportComment(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		handleReportCommentErrorPage(fcgi, data, "Cannot report a comment in a subdatin that doesn't exist</i></div>");
	}
	else{
		std::string authToken;
		if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
			|| authToken != data->authToken){
			handleReportCommentErrorPage(fcgi, data, "Invalid Authentication Token");
			return;
		}
		
		if((data->userId == -1 && data->currentTime - data->lastPostTime < Config::anonReportingTimeout()) || 
			(data->userId != -1 && data->currentTime - data->lastPostTime < Config::userReportingTimeout())){
			handleReportCommentErrorPage(fcgi, data, "You are posting/reporting too much, wait a little longer before trying again");
			return;
		}
		
		std::string commentId;
		if(getPostValue(fcgi->cgi, commentId, "commentId", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError){
			handleReportCommentErrorPage(fcgi, data, "Invalid Comment Id");
			return;
		}
		
		std::string threadId = percentDecode(parameters[1]);
		
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM comments WHERE id = ? AND threadId = ? AND subdatinId = ?"));
		prepStmt->setString(1, commentId);
		prepStmt->setString(2, threadId);
		prepStmt->setInt64(3, subdatinId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		res->beforeFirst();
		
		if(res->next()){
			std::string reason;
			switch(getPostValue(fcgi->cgi, reason, "reason", Config::getMaxReportLength(), InputFlag::AllowNonNormal)){
			default:
				handleReportCommentErrorPage(fcgi, data, "Unknown Report Error");
				return;
			case InputError::IsTooLarge:
				handleReportCommentErrorPage(fcgi, data, "Report Too Long");
				return;
			case InputError::IsEmpty:
				handleReportCommentErrorPage(fcgi, data, "Report Cannot Be Empty");
				return;
			case InputError::ContainsNewLine:
				handleReportCommentErrorPage(fcgi, data, "Report Cannot Contain Newlines");
				return;
			case InputError::NoError:
				break;
			}
			
			std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement(
				"INSERT INTO reports (reason, subdatinId, threadId, commentId, ip, userId) VALUES (?, ?, ?, ?, ?, ?)"));
			prepStmtB->setString(1, reason);
			prepStmtB->setInt64(2, subdatinId);
			prepStmtB->setString(3, threadId);
			prepStmtB->setString(4, commentId);
			prepStmtB->setString(5, fcgi->env->getRemoteAddr());
			if(data->userId == -1){
				prepStmtB->setNull(6, 0);
			}
			else{
				prepStmtB->setInt64(6, data->userId);
			}
			prepStmtB->execute();
			
			setLastPostTime(fcgi, data);
			
			createPageHeader(fcgi, data);
			fcgi->out << "Thank You For the Report! <a href='https://" << Config::getDomain() << "/d/" << parameters[0] << "/thread/" << threadId << "#" << commentId << "'>Back to the Comment</a>";
			createPageFooter(fcgi, data);
		}
		else{
			handleReportCommentErrorPage(fcgi, data, "This Comment Does Not Exist");
		}
	}
}

void handleReportCommentErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}