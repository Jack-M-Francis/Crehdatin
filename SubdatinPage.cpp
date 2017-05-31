#include "SubdatinPage.h"

void createSubdatinPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		createPageHeader(fcgi, data);
		fcgi->out << "<div class='errorText'><i>This subdatin does not exist...</i></div>";
		createPageFooter(fcgi, data);
	}
	else{
		createPageHeader(fcgi, data, subdatinId);
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id, title, anonId, userId, locked FROM threads WHERE subdatinId = ? ORDER BY lastBumpTime DESC"));
		prepStmt->setInt64(1, subdatinId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		
		res->beforeFirst();
		
		if(!res->next()){
			fcgi->out << "<div class='errorText'><i>There don't appear to be any posts here...</i></div>";
		}
		else{
			do{
				int64_t threadId = res->getInt64("id");
				std::string title = res->getString("title");
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
				
				fcgi->out << "<div class='thread'><a href='https://" << Config::getDomain() << "/d/" << percentEncode(parameters[0]) << "/thread/" << std::to_string(threadId) << "'><div class='threadTitle'>"
				<< escapeHtml(title) << "</div></a><div class='extraPostInfo'><div class='postInfoElement'>" << getFormattedPosterString(data->con, anonId, userId, subdatinId) << "</div>";
				if(res->getBoolean("locked")){
					fcgi->out << "<div class='postInfoElement'>Locked</div>";
				}
				fcgi->out << "</div></div>";
				
			}while(res->next());
		}
		createPageFooter(fcgi, data);
	}
}
