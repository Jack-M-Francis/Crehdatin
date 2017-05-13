#include "NewThreadPage.h"

void createNewThreadPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createNewThreadPage(fcgi, data, "");
}

void createNewThreadPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	
	fcgi->out << "<h1>New Thread</h1>"
	"<h3>Posting with " << (data->userId == -1?"id:":"user:") << (data->userId == -1?data->shownId:escapeHtml(data->userName)) << "</h3>"
	"<form method='post' action='https://" << Config::getDomain() << "/newThread' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='text' name='title'>Title<br>"
	"<textarea name='body'></textarea><br>";
	if(error != ""){
		fcgi->out << 
		"<p><div class='errorText'>"
		<< error <<
		"</div></p>";
	}
	fcgi->out <<
	"<button type='submit' name='submit_param'>"
	"Create Thread"
	"</button>"
	"</form>";
	
	createPageFooter(fcgi, data);
}