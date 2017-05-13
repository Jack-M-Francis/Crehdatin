#include "MainPage.h"

void createMainPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data);
	
	sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT id, title, anonId, userId FROM threads ORDER BY lastBumpTime DESC");
	sql::ResultSet* res = prepStmt->executeQuery();
	
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
			std::string userPosition;
			int64_t userId = -1;
			if(!res->isNull("userId")){
				userId = res->getInt64("userId");
				getUserData(data->con, userId, userName, userPosition);
			}
			
			fcgi->out << "<div class='thread'><a href='https://" << Config::getDomain() << "/thread/" << std::to_string(threadId) << "'><div class='threadTitle'>"
			<< escapeHtml(title) << "</div></a><div class='extraPostInfo'>" << getFormattedPosterString(data->con, anonId, userId) <<
			"</div></div>";
			
		}while(res->next());
	}
	
	delete res;
	delete prepStmt;
	
	createPageFooter(fcgi, data);
}
