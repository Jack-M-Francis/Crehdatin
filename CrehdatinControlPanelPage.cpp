#include "CrehdatinControlPanelPage.h"

void createCrehdatinControlPanelPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId))){
		createPageHeader(fcgi, data);
		fcgi->out << "<div class='errorText'>You do not have the correct permissions to view this page</div>";
		createPageFooter(fcgi, data);
		return;
	}
	createCrehdatinControlPanelPage(fcgi, data);
}

void createCrehdatinControlPanelPage(FcgiData* fcgi, RequestData* data){
	createPageHeader(fcgi, data);
	
	fcgi->out << "<h1>Crehdatin Control Panel</h1>"
	"<div class='commentEven'><h2>Administrators</h2>";
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT userName FROM users WHERE userPosition = 'administrator'"));
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		{
			fcgi->out << "<div class='commentOdd'><div class='commentText'>"
			"<form method='post' action='https://" << Config::getDomain() << "/removeAdministrator' accept-charset='UTF-8'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='userName' value='" << escapeHtml(res->getString("userName")) << "'>"
			"<div class='postInfoElement'><button type='submit'>Remove</button></div>"
			"<a href=https://'" << Config::getDomain() << "/user/" << percentEncode(res->getString("userName")) << ">"
			<< escapeHtml(res->getString("userName")) <<
			"</a></div></div>"
			"</form>";
		}while(res->next());
	}
	else{
		fcgi->out << "<div class='commentOdd'><div class='commentText'><i>There are no administrators...</i></div></div>";
	}
	
	fcgi->out << 
	"<form method='post' action='https://" << Config::getDomain() << "/addAdministrator' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<div class='postInfoElement'><button type='submit'>Add New</button></div>"
	"<input type='text' name='userName'>"
	"</form>"
	"</div>";
	
	createPageFooter(fcgi, data);
}
