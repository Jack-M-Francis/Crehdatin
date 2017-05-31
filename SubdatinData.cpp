#include "SubdatinData.h"

int64_t getSubdatinId(sql::Connection* con, std::string subdatinTitle){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT id FROM subdatins WHERE title = ?"));
	prepStmt->setString(1, subdatinTitle);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	int64_t id = -1;
	res->beforeFirst();
	if(res->next()){
		id = res->getInt64("id");
	}
	return id;
}

void getSubdatinData(sql::Connection* con, int64_t id, std::string& title, std::string& name, bool& postLocked, bool& commentLocked){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT title, name, postLocked, commentLocked FROM subdatins WHERE id = ?"));
	prepStmt->setInt64(1, id);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	if(res->next()){
		title = res->getString("title");
		name = res->getString("name");
		postLocked = res->getBoolean("postLocked");
		commentLocked = res->getBoolean("commentLocked");
	}
}