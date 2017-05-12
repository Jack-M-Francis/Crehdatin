#include "UserData.h"

void getUserData(sql::Connection* con, int64_t userId, std::string& userName, std::string& userPosition){
	userName = "<removed>";
	userPosition = "";
	
	sql::PreparedStatement* prepStmt = con->prepareStatement("SELECT userName, userPosition FROM users WHERE id = ?");
	prepStmt->setInt64(1, userId);
	sql::ResultSet* resB = prepStmt->executeQuery();
	resB->beforeFirst();
	if(resB->first()){
		userName = resB->getString("userName");
		if(!resB->isNull("userPosition")){
			userPosition = resB->getString("userPosition");
		}
	}
	
	delete resB;
	delete prepStmt;
}

void setLastPostTime(FcgiData* fcgi, RequestData* data){
	if(!Config::hasDeletePermissions(data->userPosition)){
		if(data->userId == -1){
			sql::PreparedStatement* prepStmt = data->con->prepareStatement("INSERT INTO ips (ip, lastPostTime) VALUES(?, CURRENT_TIMESTAMP) "
			"ON DUPLICATE KEY UPDATE lastPostTime = CURRENT_TIMESTAMP");
			prepStmt->setString(1, fcgi->env->getRemoteAddr());
			prepStmt->execute();
			delete prepStmt;
		}
		else{
			sql::PreparedStatement* prepStmt = data->con->prepareStatement("UPDATE users SET lastPostTime = CURRENT_TIMESTAMP WHERE id = ?");
			prepStmt->setInt64(1, data->userId);
			prepStmt->execute();
			delete prepStmt;
		}
	}
}

std::string getFormattedPosterString(sql::Connection* con, std::string anonId, int64_t userId){
	if(userId != -1){
		std::string userPosition;
		std::string userName;
		
		getUserData(con, userId, userName, userPosition);
		
		if(userPosition == "senate"){
			return "user: <div class='senateTag'>" + escapeHtml(userName) + "[S]</div>";
		}
		else if(userPosition == "admin"){
			return "user:  <div class='adminTag'>" + escapeHtml(userName) "+[A]</div>";
		}
		else if(userPosition == "moderator"){
			return "user: <div class='moderatorTag'>" + escapeHtml(userName) + "[M]</div>";
		}
		else{
			return "user: " + escapeHtml(userName);
		}
	}
	else{
		return "id: " + anonId;
	}
}
