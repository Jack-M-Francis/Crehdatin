#include "HandleDeleteComment.h"

void handleDeleteComment(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		handleDeleteCommentErrorPage(fcgi, data, "You cannot delete a comment from a subdatin that doesn't exist");
	}
	else{
		std::string authToken;
		if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
			|| authToken != data->authToken){
			handleDeleteCommentErrorPage(fcgi, data, "Invalid Authentication Token");
			return;
		}
		
		if(hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
			handleDeleteCommentErrorPage(fcgi, data, "You Do Not Have The Correct Permissions To Do This");
			return;
		}
		
		std::string commentId;
		if(getPostValue(fcgi->cgi, commentId, "commentId", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError){
			handleDeleteCommentErrorPage(fcgi, data, "Invalid Comment Id");
			return;
		}
		
		std::string threadId = percentDecode(parameters[1]);
		
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM comments WHERE id = ? AND threadId = ? AND subdatin = ?"));
		prepStmt->setString(1, commentId);
		prepStmt->setString(2, threadId);
		prepStmt->setInt64(3, subdatinId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		res->beforeFirst();
		
		if(res->next()){
			std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("DELETE FROM comments WHERE id = ?"));
			prepStmtB->setString(1, commentId);
			prepStmtB->execute();
			
			std::string seeAlso;
			InputError error = getPostValue(fcgi->cgi, seeAlso, "seeAlso", Config::getUniqueTokenLength(), InputFlag::AllowNonNormal);
			
			sendStatusHeader(fcgi->out, StatusCode::SeeOther);
			if(error != InputError::NoError || seeAlso.size() == 0){
				sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/d/" + parameters[0] + "/thread/" + threadId);
			}
			else{
				sendLocationHeader(fcgi->out, seeAlso);
			}
			finishHttpHeader(fcgi->out);
		}
		else{
			handleDeleteCommentErrorPage(fcgi, data, "This Comment Does Not Exist");
		}
	}
}

void handleDeleteCommentErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}