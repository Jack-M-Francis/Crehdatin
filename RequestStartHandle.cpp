#include "RequestStartHandle.h"

bool requestStartHandle(FcgiData* fcgi, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::unique_ptr<sql::ResultSet> res(data->stmt->executeQuery("SELECT UNIX_TIMESTAMP(CURRENT_TIMESTAMP)"));
	
	res->first();
	
	data->currentTime = res->getInt64(1);
	
	data->sessionToken = "";
	data->userName = "";
	data->userId = -1;
	data->authToken = "";
	data->shownId = "";
	data->cssTheme = "dark";
	data->blocked = false;
	data->lastPostTime = 0;
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT " 
	"blocked, UNIX_TIMESTAMP(lastPostTime) "
	"FROM ips "
	"WHERE ip = ?"));
	prepStmt->setString(1, fcgi->env->getRemoteAddr());
	res = std::unique_ptr<sql::ResultSet>(prepStmt->executeQuery());
	res->beforeFirst();
	
	if(res->next()){
		data->blocked = res->getBoolean("blocked");
		data->lastPostTime = res->getInt64("UNIX_TIMESTAMP(lastPostTime)");
	}
	
	if(data->blocked){
		sendStatusHeader(fcgi->out, StatusCode::Ok);
		sendHtmlContentTypeHeader(fcgi->out);
		finishHttpHeader(fcgi->out);
		fcgi->out << "<html><body>You are blocked</body></html>";
		return false;
	}
	
	std::vector<cgicc::HTTPCookie> cookies = fcgi->env->getCookieList();
	std::string sessionToken;
	for(auto i = cookies.begin(); i != cookies.end(); i++){
		if(i->getName() == "sessionToken"){
			sessionToken = i->getValue();
			break;
		}
	}
	
	if(sessionToken.size() != 0){
		if(getUserRequestData(fcgi, data, sessionToken) == false){
			return false;
		}
	}
	
	if(data->sessionToken.size() == 0){
		createNewSession(data);
		sendCookieHeader(fcgi->out, "sessionToken", data->sessionToken);
	}
	return true;
}

bool getUserRequestData(FcgiData* fcgi, RequestData* data, std::string sessionToken){
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT " 
	"id, userId, authToken, shownId "
	"FROM sessions "
	"WHERE sessionToken = ?"));
	prepStmt->setString(1, sessionToken);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		data->sessionToken = sessionToken;
		if(!res->isNull("userId")){
			data->userId = res->getInt64("userId");
			
			std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("SELECT " 
			"userName, cssTheme, UNIX_TIMESTAMP(lastPostTime) "
			"FROM users "
			"WHERE id = ?"));
			
			prepStmtB->setInt64(1, data->userId);
			
			std::unique_ptr<sql::ResultSet> resB(prepStmtB->executeQuery());
			
			resB->beforeFirst();
			
			if(resB->next()){
				data->userName = resB->getString("userName");
				
				if(resB->isNull("cssTheme") == false){
					data->cssTheme = resB->getString("cssTheme");
				}
				
				if(resB->isNull("UNIX_TIMESTAMP(lastPostTime)") == false){
					data->lastPostTime = resB->getInt64("UNIX_TIMESTAMP(lastPostTime)");
				}
			}
			else{
				sendStatusHeader(fcgi->out, StatusCode::Ok);
				sendHtmlContentTypeHeader(fcgi->out);
				sendDeleteCookieHeader(fcgi->out, "sessionToken");
				finishHttpHeader(fcgi->out);
				fcgi->out << "<html><body>An unknown, internal server error occurred</body></html>";
				return false;
			}
		}
		data->authToken = res->getString("authToken");
		if(!res->isNull("shownId")){
			data->shownId = res->getString("shownId");
		}
	}
	return true;
}

void createNewSession(RequestData* data){
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("INSERT INTO sessions "
	"(sessionToken, authToken, shownId) SELECT ?, ?, ? FROM DUAL WHERE NOT EXISTS "
	"(SELECT id FROM sessions WHERE sessionToken=? OR authToken=? OR shownId=?)"));
	
	int64_t rowCount;
	
	do{
		data->sessionToken = generateRandomToken();
		data->authToken = generateRandomToken();
		data->shownId = generateRandomToken().substr(0, 12);
		
		prepStmt->setString(1, data->sessionToken);
		prepStmt->setString(2, data->authToken);
		prepStmt->setString(3, data->shownId);
		prepStmt->setString(4, data->sessionToken);
		prepStmt->setString(5, data->authToken);
		prepStmt->setString(6, data->shownId);
		prepStmt->execute();
		
		std::unique_ptr<sql::ResultSet> res(data->stmt->executeQuery("SELECT ROW_COUNT()"));
		
		res->first();
		
		rowCount = res->getInt64(1);
		
	}while(rowCount == 0);
}