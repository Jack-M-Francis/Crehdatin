#include "ReportsPage.h"

void createReportsPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!Config::hasDeletePermissions(data->userPosition)){
		createPageHeader(fcgi, data);
		fcgi->out << "<div class='errorText'>You do not have the correct permissions to view this page.</div>";
		createPageFooter(fcgi, data);
		return;
	}
	
	createPageHeader(fcgi, data);
	
	sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT id, reason, commentId, threadId, ip, userId FROM reports ORDER BY threadId, commentId");
	sql::ResultSet* res = prepStmt->executeQuery();
	
	res->beforeFirst();
	
	int64_t lastCommentId = -1;
	int64_t lastThreadId = -1;
	
	if(res->next()){
		do{
			int64_t reportId = res->getInt64("id");
			std::string reason = res->getString("reason");
			int64_t commentId = -1;
			if(!res->isNull("commentId")){
				commentId = res->getInt64("commentId");
			}
			int64_t threadId = res->getInt64("threadId");
			std::string ip = res->getString("ip");
			int64_t userId = -1;
			if(!res->isNull("userId")){
				userId = res->getInt64("userId");
			}
			
			if(lastCommentId != commentId || lastThreadId != threadId){
				if(lastThreadId != -1){
					fcgi->out << "</div>";
				}
				
				if(commentId == -1){
					createReportedThread(fcgi, data, threadId);
				}
				else{
					createReportedComment(fcgi, data, threadId, commentId);
				}
			}
			
			createReport(fcgi, data, reason, ip, userId);
			
			lastCommentId = commentId;
			lastThreadId = threadId;
		}while(res->next());
	}
	else{
		fcgi->out << "There are no pending reports.";
	}
	
	delete res;
	delete prepStmt;
	
	createPageFooter(fcgi, data);
}

void createReportedThread(FcgiData* fcgi, RequestData* data, int64_t threadId){
	sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT title, body, userId, posterIp FROM threads WHERE id = ?");
	prepStmt->setInt64(1, threadId);
	sql::ResultSet* res = prepStmt->executeQuery();
	
	res->beforeFirst();
	
	if(res->next()){
		std::string title = res->getString("title");
		std::string body = res->getString("body");
		int64_t userId = -1;
		if(!res->isNull("userId")){
			userId = res->getInt64("userId");
		}
		std::string posterIp = res->getString("posterIp");
		
		body = escapeHtml(body);
		formatUserPostBody(body);
		
		fcgi->out << "<div class='thread'><a href='https://" << Config::getDomain() << "/thread/" << std::to_string(threadId) << "'>"
			<< escapeHtml(title) << "</a><br><div class='extraPostInfo'>";
		
		fcgi->out << "<div class='dropDown'><div class='dropBtn'>Actions</div><ul><li><form method='post' action='https://" << Config::getDomain() << "/thread/" << std::to_string(threadId) << "/deleteThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='seeAlso' value='https://" << Config::getDomain() << "/reports'>"
			"<button type='submit' class='link-button'>"
			"Delete"
			"</button>"
			"</form></li>";
			
		fcgi->out << "<li><form method='post' action='https://" << Config::getDomain() << "/dismissReports' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='threadId' value='" << std::to_string(threadId) << "'>"
			"<input type='hidden' name='commentId' value='-1'>"
			"<button type='submit' class='link-button'>"
			"Dismiss"
			"</button>"
			"</form></li>";
			
		fcgi->out << "</ul></div>";
		if(userId != -1){
			std::string userName;
			std::string userPosition;
			
			getUserData(data->con, userId, userName, userPosition);
			
			fcgi->out << "user: " << userName << ", ";
		}
		
		fcgi->out << "ip: " << posterIp << 
			"</div>"
			"<div class='threadText'>" << body <<
			"</div>";
	}
	else{
		fcgi->out << "<div class='thread'><div class='threadText'><div class='errorText'>"
		"An error occurred while showing this thread</div></div>";
	}
	
	delete res;
	delete prepStmt;
}

void createReportedComment(FcgiData* fcgi, RequestData* data, int64_t threadId, int64_t commentId){
	sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT body, userId, posterIp FROM comments WHERE id = ? AND threadId = ?");
	prepStmt->setInt64(1, commentId);
	prepStmt->setInt64(2, threadId);
	sql::ResultSet* res = prepStmt->executeQuery();
	
	res->beforeFirst();
	
	if(res->next()){
		std::string body = res->getString("body");
		int64_t userId = -1;
		if(!res->isNull("userId")){
			userId = res->getInt64("userId");
		}
		std::string posterIp = res->getString("posterIp");
		
		body = escapeHtml(body);
		formatUserPostBody(body);
		
		fcgi->out << "<div class='commentEven'><div class='extraPostInfo'><a href='https://" << Config::getDomain() << "/thread/" << std::to_string(threadId) << "#" << std::to_string(commentId) << "'>comment</a>, "
			"<div class='dropDown'><div class='dropBtn'>Actions</div><ul><li><form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/deleteComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<input type='hidden' name='seeAlso' value='https://" << Config::getDomain() << "/reports'>"
			"<button type='submit' class='link-button'>"
			"Delete"
			"</button>"
			"</form></li>";
			
		fcgi->out << "<li><form method='post' action='https://" << Config::getDomain() << "/dismissReports' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='threadId' value='" << std::to_string(threadId) << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' class='link-button'>"
			"Dismiss"
			"</button>"
			"</form></li>";
			
		fcgi->out << "</ul></div>";
		if(userId != -1){
			std::string userName;
			std::string userPosition;
			
			getUserData(data->con, userId, userName, userPosition);
			
			fcgi->out << "user: " << userName << ", ";
		}
		
		fcgi->out << "ip: " << posterIp << 
			"</div>"
			"<div class='commentText'>" << body <<
			"</div>";
	}
	else{
		fcgi->out << "<div class='thread'><div class='threadText'><div class='errorText'>"
		"An error occurred while showing this thread</div></div>";
	}
	
	delete res;
	delete prepStmt;
}

void createReport(FcgiData* fcgi, RequestData* data, std::string& reason, std::string& ip, int64_t userId){
	fcgi->out << "<div class='commentOdd'><div class='commentText'>'" << reason << "' ";
	if(userId != -1){
		std::string userName;
		std::string userPosition;
		
		getUserData(data->con, userId, userName, userPosition);
		
		fcgi->out << "user: " << userName << ", ";
	}
	fcgi->out << "ip: " << ip <<
	 "</div></div>";
}














