#include "HandleNewComment.h"

void handleNewComment(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		createPageHeader(fcgi, data);
		fcgi->out << "<div class='errorText'><i>Cannot reply in a subdatin that doesn't exist</i></div>";
		createPageFooter(fcgi, data);
	}
	else{
		std::string title;
		std::string name;
		bool postLocked;
		bool commentLocked;
		getSubdatinData(data->con, subdatinId, title, name, postLocked, commentLocked);
		
		if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId)) && commentLocked){
			handleNewCommentErrorPage(fcgi, data, subdatinId, "You cannot post a comment in a subdatin with locked comments");
			return;
		}
		
		std::string threadId = percentDecode(parameters[1]);
		
		if((data->userId == -1 && data->currentTime - data->lastPostTime < Config::anonPostingTimeout()) || 
			(data->userId != -1 && data->currentTime - data->lastPostTime < Config::userPostingTimeout())){
			handleNewCommentErrorPage(fcgi, data, subdatinId, "You are posting/reporting too much, wait a little longer before trying again");
			return;
		}
		
		std::string body;
		switch(getPostValue(fcgi->cgi, body, "body", Config::getMaxPostLength(), InputFlag::AllowNonNormal | InputFlag::AllowNewLine)){
		default:
			handleNewCommentErrorPage(fcgi, data, subdatinId, "Unknown Comment Error");
			return;
		case InputError::IsTooLarge:
			handleNewCommentErrorPage(fcgi, data, subdatinId, "Comment Too Long");
			return;
		case InputError::IsEmpty:
			handleNewCommentErrorPage(fcgi, data, subdatinId, "Comments Cannot Be Blank");
			return;
		case InputError::IsMalformed:
			handleNewCommentErrorPage(fcgi, data, subdatinId, "Comment Contains Invalid Characters");
			return;
		case InputError::NoError:
			break;
		}
		
		std::string authToken;
		if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
			|| authToken != data->authToken){
			handleNewCommentErrorPage(fcgi, data, subdatinId, "Invalid Authentication Token");
			return;
		}
		
		std::string parentId;
		if(getPostValue(fcgi->cgi, parentId, "parentId", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError){
			handleNewCommentErrorPage(fcgi, data, subdatinId, "Invalid Parent Comment Id");
			return;
		}
		
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT locked FROM threads WHERE id = ? AND subdatinId = ?"));
		prepStmt->setString(1, threadId);
		prepStmt->setInt64(2, subdatinId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		res->beforeFirst();
		
		if(!res->next()){
			handleNewCommentErrorPage(fcgi, data, subdatinId, "Cannot Reply In A Thread That Doesn't Exist");
			return;
		}
		bool threadLocked = res->getBoolean("locked");
		
		if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId)) && threadLocked){
			handleNewCommentErrorPage(fcgi, data, subdatinId, "Cannot Reply In a Thread With Comments Locked");
			return;
		}
		
		if(parentId != "-1"){
			std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM comments WHERE id = ? AND threadId = ? AND subdatinId = ?"));
			prepStmt->setString(1, parentId);
			prepStmt->setString(2, threadId);
			prepStmt->setInt64(3, subdatinId);
			std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
			res->beforeFirst();
			
			if(!res->next()){
				handleNewCommentErrorPage(fcgi, data, subdatinId, "Cannot Reply To A Comment That Doesn't Exist");
				return;
			}
		}
		
		prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement(
			"INSERT INTO comments (body, anonId, userId, posterIp, threadId, subdatinId, parentId) VALUES (?, ?, ?, ?, ?, ?, ?)"));
		
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
		prepStmt->setInt64(6, subdatinId);
		if(parentId == "-1"){
			prepStmt->setNull(7, 0);
		}
		else{
			prepStmt->setString(7, parentId);
		}
		
		prepStmt->execute();
		
		if(parentId == "-1"){//if we should bump the thread
			prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement("UPDATE threads SET lastBumpTime = CURRENT_TIMESTAMP WHERE id = ?"));
			prepStmt->setString(1, threadId);
			prepStmt->execute();
		}
		
		setLastPostTime(fcgi, data);
		
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/d/" + percentEncode(parameters[0]) + "/thread/" + threadId);
		finishHttpHeader(fcgi->out);
	}
}

void handleNewCommentErrorPage(FcgiData* fcgi, RequestData* data, int64_t subdatinId, std::string error){
	createPageHeader(fcgi, data, subdatinId);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}





























