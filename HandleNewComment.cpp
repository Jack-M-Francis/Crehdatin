#include "HandleNewComment.h"

void handleNewComment(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string threadId = percentDecode(parameters[0]);
	
	if((data->userId == -1 && data->currentTime - data->lastPostTime < Config::anonPostingTimeout()) || 
		(data->userId != -1 && data->currentTime - data->lastPostTime < Config::userPostingTimeout())){
		handleNewCommentErrorPage(fcgi, data, "You are posting/reporting too much, wait a little longer before trying again");
		return;
	}
	
	std::string body;
	switch(getPostValue(fcgi->cgi, body, "body", Config::getMaxPostLength(), InputFlag::AllowNonNormal | InputFlag::AllowNewLine)){
	default:
		handleNewCommentErrorPage(fcgi, data, "Unknown Comment Error");
		return;
	case InputError::IsTooLarge:
		handleNewCommentErrorPage(fcgi, data, "Comment Too Long");
		return;
	case InputError::IsEmpty:
		handleNewCommentErrorPage(fcgi, data, "Comments Cannot Be Blank");
		return;
	case InputError::IsMalformed:
		handleNewCommentErrorPage(fcgi, data, "Comment Contains Invalid Characters");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		handleNewCommentErrorPage(fcgi, data, "Invalid Authentication Token");
		return;
	}
	
	std::string parentId;
	if(getPostValue(fcgi->cgi, parentId, "parentId", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError){
		handleNewCommentErrorPage(fcgi, data, "Invalid Parent Comment Id");
		return;
	}
	
	if(parentId != "-1"){
		sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT id FROM comments WHERE id = ? AND threadId = ?");
		prepStmt->setString(1, parentId);
		prepStmt->setString(2, threadId);
		sql::ResultSet* res = prepStmt->executeQuery();
		res->beforeFirst();
		
		if(!res->next()){
			delete res;
			delete prepStmt;
			handleNewCommentErrorPage(fcgi, data, "Cannot Reply To A Comment That Doesn't Exist");
			return;
		}
		delete res;
		delete prepStmt;
	}
	else{
		sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT id FROM threads WHERE id = ?");
		prepStmt->setString(1, threadId);
		sql::ResultSet* res = prepStmt->executeQuery();
		res->beforeFirst();
		
		if(!res->next()){
			delete res;
			delete prepStmt;
			handleNewCommentErrorPage(fcgi, data, "Cannot Reply To A Thread That Doesn't Exist");
			return;
		}
		delete res;
		delete prepStmt;
	}
	
	sql::PreparedStatement* prepStmt = data->con->prepareStatement("INSERT INTO comments (body, anonId, userId, posterIp, threadId, parentId) VALUES (?, ?, ?, ?, ?, ?)");
	
	prepStmt->setString(1, body);
	if(data->userId == -1){
		prepStmt->setString(2, data->shownId);
		prepStmt->setNull(3, 0);
	}
	else{
		prepStmt->setNull(2, 0);
		prepStmt->setInt64(3, data->userId);
	}
	prepStmt->setString(4, fcgi->env->getRemoteAddr());
	prepStmt->setString(5, threadId);
	if(parentId == "-1"){
		prepStmt->setNull(6, 0);
	}
	else{
		prepStmt->setString(6, parentId);
	}
	
	prepStmt->execute();
	delete prepStmt;
	
	if(parentId == "-1"){//if we should bump the thread
		prepStmt = data->con->prepareStatement("UPDATE threads SET lastBumpTime = CURRENT_TIMESTAMP WHERE id = ?");
		prepStmt->setString(1, threadId);
		prepStmt->execute();
		delete prepStmt;
	}
	
	setLastPostTime(fcgi, data);
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/thread/" + threadId);
	finishHttpHeader(fcgi->out);
}

void handleNewCommentErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}





























