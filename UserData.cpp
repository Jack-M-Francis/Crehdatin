#include "UserData.h"

std::string getUserName(sql::Connection* con, int64_t userId){
	std::string userName = "<removed>";
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT userName FROM users WHERE id = ?"));
	prepStmt->setInt64(1, userId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		userName = res->getString("userName");
	}
	
	return userName;
}

int64_t getUserId(sql::Connection* con, std::string userName){
	int64_t userId = -1;
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT id FROM users WHERE userName = ?"));
	prepStmt->setString(1, userName);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		userId = res->getInt64("id");
	}
	return userId;
}

void setLastPostTime(FcgiData* fcgi, RequestData* data){
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId))){
		if(data->userId == -1){
			std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement(
				"INSERT INTO ips (ip, lastPostTime) VALUES (?, CURRENT_TIMESTAMP) ON DUPLICATE KEY UPDATE lastPostTime = CURRENT_TIMESTAMP"));
			prepStmt->setString(1, fcgi->env->getRemoteAddr());
			prepStmt->execute();
		}
		else{
			std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE users SET lastPostTime = CURRENT_TIMESTAMP WHERE id = ?"));
			prepStmt->setInt64(1, data->userId);
			prepStmt->execute();
		}
	}
}

std::string getFormattedPosterString(sql::Connection* con, std::string anonId, int64_t userId, int64_t subdatinId){
	if(userId != -1){
		std::string userName = getUserName(con, userId);
		std::string userPosition = getEffectiveUserPosition(con, userId, subdatinId);
		
		if(userPosition == "senate"){
			return "user: <div class='senateTag'>" + escapeHtml(userName) + "[S]</div>";
		}
		else if(userPosition == "administrator"){
			return "user:  <div class='administratorTag'>" + escapeHtml(userName) + "[A]</div>";
		}
		else if(userPosition == "bureaucrat"){
			return "user:  <div class='bureaucratTag'>" + escapeHtml(userName) + "[B]</div>";
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

std::string getEffectiveUserPosition(sql::Connection* con, int64_t userId, int64_t subdatinId){
	std::string position;
	
	if(userId != -1){
		std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT userPosition FROM users WHERE id = ?"));
		prepStmt->setInt64(1, userId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		res->beforeFirst();
		if(res->next()){
			if(!res->isNull("userPosition")){
				position = res->getString("userPosition");
			}
		}
		
		if(position.size() == 0 && subdatinId != -1){
			prepStmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement("SELECT userPosition FROM userPositions WHERE userId = ? AND subdatinId = ?"));
			prepStmt->setInt64(1, userId);
			prepStmt->setInt64(2, subdatinId);
			res = std::unique_ptr<sql::ResultSet>(prepStmt->executeQuery());
			res->beforeFirst();
			if(res->next()){
				position = res->getString("userPosition");
			}
		}
	}
	
	return position;
}