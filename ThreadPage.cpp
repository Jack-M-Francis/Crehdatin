#include "ThreadPage.h"

void formatUserPostBody(std::string& body){
	replaceAll(body, "\n", "<br>");
	replaceAll(body, "\r", "");
	trimString(body);
	for(int linkCharNumber=0; linkCharNumber <= body.length; linkCharNumber++){
		if(body[linkCharNumber]=='h'){
			if(linkCharNumber!=body.length&&body[linkCharNumber + 1]=='t'&&body[linkCharNumber + 2]=='t'&&body[linkCharNumber + 3]=='p'){
				for(string link;body[linkCharNumber] <= body.length - 1;linkCharNumber++){
					link=link+body[linkCharNumber];
				}
			}
		}
	}
	link="<a href='"+link+"'>" + link + "</a>";
}

void createThreadPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string threadId = percentDecode(parameters[0]);
	
	createPageHeader(fcgi, data);
	
	sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT title, body, anonId, userId FROM threads WHERE id = ?");
	prepStmt->setString(1, threadId);
	sql::ResultSet* res = prepStmt->executeQuery();
	res->beforeFirst();
	
	if(res->next()){
		std::string title = res->getString("title");
		std::string body = res->getString("body");
		std::string anonId;
		if(!res->isNull("anonId")){
			anonId = res->getString("anonId");
		}
		std::string userName;
		std::string userPosition;
		int userId = -1;
		if(!res->isNull("userId")){
			userId = res->getInt("userId");
			getUserData(data->con, userId, userName, userPosition);
		}
		
		body = escapeHtml(body);
		formatUserPostBody(body);
		
		fcgi->out << "<div class='thread'><div class='threadTitle'>"
			<< escapeHtml(title) << "</div><div class='extraPostInfo'>";
		createReplyMenu(fcgi, data, threadId);
		createReportMenu(fcgi, data, threadId);
		fcgi->out << getFormattedPosterString(data->con, anonId, userId) << 
			"</div>"
			"<div class='threadText'>" << body
			<< "</div></div>";
			
			
			/*
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/newComment' accept-charset='UTF-8'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='parentId' value='-1'>"
			"<textarea name='body'></textarea><br>"
			"<button type='submit' name='submit_param'>"
			"Submit"
			"</button>" 
			"</form>";
			*/
		createCommentLine(fcgi, data, threadId);
	}
	else{
		fcgi->out << "<div class='errorText'>This Thread Does Not Exist</div>";
	}
	createPageFooter(fcgi, data);
	
	delete res;
	delete prepStmt;
}

void createCommentLine(FcgiData* fcgi, RequestData* data, std::string& threadId, int64_t layer, int64_t parentId){
	sql::PreparedStatement* prepStmt;
		
	if(parentId == -1){
		prepStmt = data->con->prepareStatement("SELECT id, body, anonId, userId FROM comments WHERE parentId IS NULL AND threadId = ? ORDER BY createdTime DESC");
		prepStmt->setString(1, threadId);
	}
	else{
		prepStmt = data->con->prepareStatement("SELECT id, body, anonId, userId FROM comments WHERE parentId = ? AND threadId = ? ORDER BY createdTime DESC");
		prepStmt->setInt64(1, parentId);
		prepStmt->setString(2, threadId);
	}
	
	sql::ResultSet* res = prepStmt->executeQuery();
	res->beforeFirst();
	
	while(res->next()){
		int64_t commentId = res->getInt64("id");
		std::string body = res->getString("body");
		std::string anonId;
		if(!res->isNull("anonId")){
			anonId = res->getString("anonId");
		}
		std::string userName;
		std::string userPosition;
		int64_t userId = -1;
		if(!res->isNull("userId")){
			userId = res->getInt64("userId");
			getUserData(data->con, userId, userName, userPosition);
		}
		
		body = escapeHtml(body);
		formatUserPostBody(body);
		
		fcgi->out << "<a name='" << std::to_string(commentId) << "'>"
		<< (layer%2==0?"<div class='commentEven'>":"<div class='commentOdd'>") << 
		"<div class='extraPostInfo'>";
		
		createReplyMenu(fcgi, data, threadId, commentId);
		createReportMenu(fcgi, data, threadId, commentId);
			
		fcgi->out << getFormattedPosterString(data->con, anonId, userId)
			<< "</div><div class='commentText'>"
			<< body << "</div>";
				
		createCommentLine(fcgi, data, threadId, layer + 1, commentId);
		
		fcgi->out << "</div>";
	}
	
	delete res;
	delete prepStmt;
	
}

void createReportMenu(FcgiData* fcgi, RequestData* data, std::string& threadId, int64_t commentId){
	
	fcgi->out <<
		"<div class='dropDown'>"
		"<div class='dropBtn'>"
		"Report"
		"</div>"
		"<ul>";
	
	if(commentId == -1){
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Illegal' class='link-button'>"
			"Illegal"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Spam' class='link-button'>"
			"Spam"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Copyrighted' class='link-button'>"
			"Copyrighted"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Obscene' class='link-button'>"
			"Obscene"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Inciting Violence' class='link-button'>"
			"Inciting Violence"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' class='link-button'>"
			"Other, specify:"
			"</button>"
			"<input type='text' name='reason' class='inline'>"
			"</form>"
			"</li>";
		if(Config::hasDeletePermissions(data->userPosition)){
			fcgi->out <<
				"<li>"
				"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/deleteThread' class='inline'>"
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
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Illegal' class='link-button'>"
			"Illegal"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Spam' class='link-button'>"
			"Spam"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Copyrighted' class='link-button'>"
			"Copyrighted"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Obscene' class='link-button'>"
			"Obscene"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' name='reason' value='Inciting Violence' class='link-button'>"
			"Inciting Violence"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' class='link-button'>"
			"Other, specify:"
			"</button>"
			"<input type='text' name='reason' class='inline'>"
			"</form>"
			"</li>";
		if(Config::hasDeletePermissions(data->userPosition)){
			fcgi->out <<
				"<li>"
				"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/deleteComment' class='inline'>"
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
		"</div>";
	
}

void createReplyMenu(FcgiData* fcgi, RequestData* data, std::string& threadId, int64_t commentId){
	fcgi->out << "<div class='dropDown'>"
	"<div class='dropBtn'>"
	"Reply"
	"</div>"
	"<ul>"
	"<li>"
	"<form method='post' action='https://" << Config::getDomain() << "/thread/" << threadId << "/newComment' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='hidden' name='parentId' value='" << std::to_string(commentId) << "'>"
	"<textarea cols='25' name='body'></textarea><br>"
	"<button type='submit' name='submit_param'>"
	"Create Comment"
	"</button>"
	"</li>"
	"</ul>"
	"</form>"
	"</div>";
}



