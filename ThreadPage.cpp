#include "ThreadPage.h"

void createThreadPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
		
	if(subdatinId == -1){
		createPageHeader(fcgi, data);
		fcgi->out << "<div class='errorText'><i>This subdatin does not exist...</i></div>";
		createPageFooter(fcgi, data);
	}
	else{
		std::string threadId = percentDecode(parameters[1]);
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT title, body, anonId, userId, locked FROM threads WHERE id = ? AND subdatinId = ?"));
		prepStmt->setString(1, threadId);
		prepStmt->setInt64(2, subdatinId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		res->beforeFirst();
		
		if(res->next()){
			createPageHeader(fcgi, data, subdatinId);
			std::string title = res->getString("title");
			std::string body = res->getString("body");
			std::string anonId;
			if(!res->isNull("anonId")){
				anonId = res->getString("anonId");
			}
			std::string userName;
			int userId = -1;
			if(!res->isNull("userId")){
				userId = res->getInt("userId");
				userName = getUserName(data->con, userId);
			}
			bool canReply = !res->getBoolean("locked") || hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId));
			
			body = formatUserPostBody(escapeHtml(body), getEffectiveUserPosition(data->con, userId, subdatinId));
			
			fcgi->out << "<div class='thread'><div class='threadTitle'>"
				<< escapeHtml(title) << "</div><div class='extraPostInfo'>";
			if(canReply){
				createReplyMenu(fcgi, data, threadId, parameters[0]);
			}
			createReportMenu(fcgi, data, threadId, parameters[0]);
			if(res->getBoolean("locked")){
				fcgi->out << "<div class='postInfoElement'>Locked</div>";
			}
			fcgi->out << "<div class='postInfoElement'>"
				<< getFormattedPosterString(data->con, anonId, userId, subdatinId) << 
				"</div>"
				"</div>"
				"<div class='threadText'>" << body
				<< "</div></div>";
				
			createCommentLine(fcgi, data, threadId, subdatinId, canReply, parameters[0]);
		}
		else{
			createPageHeader(fcgi, data, subdatinId);
			fcgi->out << "<div class='errorText'>This Thread Does Not Exist</div>";
			createPageFooter(fcgi, data);
		}
	}
}

void createCommentLine(FcgiData* fcgi, RequestData* data, std::string& threadId, int64_t subdatinId, bool canReply, std::string& subdatinTitle, int64_t layer, int64_t parentId){
	std::unique_ptr<sql::PreparedStatement> prepStmt;
		
	if(parentId == -1){
		prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement(
			"SELECT id, body, anonId, userId FROM comments WHERE parentId IS NULL AND threadId = ? AND subdatinId = ? ORDER BY createdTime DESC"));
		prepStmt->setString(1, threadId);
		prepStmt->setInt64(2, subdatinId);
	}
	else{
		prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement(
			"SELECT id, body, anonId, userId FROM comments WHERE parentId = ? AND threadId = ? AND subdatinId = ? ORDER BY createdTime DESC"));
		prepStmt->setInt64(1, parentId);
		prepStmt->setString(2, threadId);
		prepStmt->setInt64(3, subdatinId);
	}
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	
	while(res->next()){
		int64_t commentId = res->getInt64("id");
		std::string body = res->getString("body");
		std::string anonId;
		if(!res->isNull("anonId")){
			anonId = res->getString("anonId");
		}
		std::string userName;
		int64_t userId = -1;
		if(!res->isNull("userId")){
			userId = res->getInt64("userId");
			userName = getUserName(data->con, userId);
		}
		
		body = formatUserPostBody(escapeHtml(body), getEffectiveUserPosition(data->con, userId, subdatinId));
		
		fcgi->out << "<a name='" << std::to_string(commentId) << "'></a>"
		<< (layer%2==0?"<div class='commentEven'>":"<div class='commentOdd'>") << 
		"<div class='extraPostInfo'>";
		if(canReply){
			createReplyMenu(fcgi, data, threadId, subdatinTitle, commentId);
		}
		createReportMenu(fcgi, data, threadId, subdatinTitle, commentId);
			
		fcgi->out << getFormattedPosterString(data->con, anonId, userId, subdatinId)
			<< "</div><div class='commentText'>"
			<< body << "</div>";
				
		createCommentLine(fcgi, data, threadId, subdatinId, canReply, subdatinTitle, layer + 1, commentId);
		
		fcgi->out << "</div>";
	}
}

void createReportMenu(FcgiData* fcgi, RequestData* data, std::string& threadId, std::string& subdatinTitle, int64_t commentId){
	
	int64_t subdatinId = getSubdatinId(data->con, subdatinTitle);
	
	fcgi->out <<
		"<div class='postInfoElement'>"
		"<div class='dropDown'>"
		"<div class='dropBtn'>"
		"Report"
		"</div>"
		"<ul>";
	
	if(commentId == -1){
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Illegal' class='link-button'>"
			"Illegal"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Spam' class='link-button'>"
			"Spam"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Copyrighted' class='link-button'>"
			"Copyrighted"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Obscene' class='link-button'>"
			"Obscene"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Inciting Violence' class='link-button'>"
			"Inciting Violence"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' class='link-button'>"
			"Other, specify:"
			"</button>"
			"<input type='text' name='reason' class='inline'>"
			"</form>"
			"</li>";
		if(hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
			fcgi->out <<
				"<li>"
				"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/deleteThread' class='inline'>"
				"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
				"<button type='submit' class='link-button'>"
				"Delete"
				"</button>"
				"</form>"
				"</li>";
		}
	}
	else{
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Illegal' class='link-button'>"
			"Illegal"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Spam' class='link-button'>"
			"Spam"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Copyrighted' class='link-button'>"
			"Copyrighted"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Obscene' class='link-button'>"
			"Obscene"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Inciting Violence' class='link-button'>"
			"Inciting Violence"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' class='link-button'>"
			"Other, specify:"
			"</button>"
			"<input type='text' name='reason' class='inline'>"
			"</form>"
			"</li>";
		if(hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
			fcgi->out <<
				"<li>"
				"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/deleteComment' class='inline'>"
				"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
				"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
				"<button type='submit' class='link-button'>"
				"Delete"
				"</button>"
				"</form>"
				"</li>";
		}
	}
	
	fcgi->out << 
		"</ul>"
		"</div>"
		"</div>";
	
}

void createReplyMenu(FcgiData* fcgi, RequestData* data, std::string& threadId, std::string& subdatinTitle, int64_t commentId){
	fcgi->out << "<div class='postInfoElement'>"
	"<div class='dropDown'>"
	"<div class='dropBtn'>"
	"Reply"
	"</div>"
	"<ul>"
	"<li>"
	"<form method='post' action='https://" << Config::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << threadId << "/newComment' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='hidden' name='parentId' value='" << std::to_string(commentId) << "'>"
	"<textarea cols='25' name='body'></textarea><br>"
	"<button type='submit' name='submit_param'>"
	"Create Comment"
	"</button>"
	"</li>"
	"</ul>"
	"</form>"
	"</div>"
	"</div>";
}



