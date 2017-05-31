#include "SubdatinControlPanelPage.h"

void createSubdatinControlPanelPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		createPageHeader(fcgi, data);
		fcgi->out << "<div class='errorText'>Cannot view a control panel for a subdatin that doesn't exist</div>";
		createPageFooter(fcgi, data);
	}
	else{
		if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
			createPageHeader(fcgi, data, subdatinId);
			fcgi->out << "<div class='errorText'>You do not have the correct permissions to view this page</div>";
			createPageFooter(fcgi, data);
			return;
		}
		createSubdatinControlPanelPage(fcgi, data, subdatinId);
	}
}

void createSubdatinControlPanelPage(FcgiData* fcgi, RequestData* data, int64_t subdatinId){
	createPageHeader(fcgi, data, subdatinId);
	
	fcgi->out << "<h1>Crehdatin Control Panel</h1>"
	"<div class='commentEven'><h2>Administrators</h2>";
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT userId, userPosition FROM users WHERE subdatinId = ?"));
	prepStmt->setInt64(1, subdatinId);
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
		fcgi->out << "<div class='commentOdd'><div class='commentText'><i>There are no subdatin officials...</i></div></div>";
	}
	
	fcgi->out << "</div>"
	
	createPageFooter(fcgi, data);
}
