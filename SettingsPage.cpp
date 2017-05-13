#include "SettingsPage.h"

void createSettingsPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		createPageHeader(fcgi, data);
		fcgi->out << 
		"<p><div class='errorText'>"
		"You can only do this if you are logged in."
		"</div></p>";
		createPageFooter(fcgi, data);
		return;
	}
	
	createSettingsPage(fcgi, data, "", "");
}

void createSettingsPage(FcgiData* fcgi, RequestData* data, std::string cssError, std::string passwordError){
	createPageHeader(fcgi, data);
	
	fcgi->out << "<h1>Settings</h1>"
	"<h3>Username: " << escapeHtml(data->userName) << "</h3>"
	"<h3>Current Theme: ";
	if(data->cssTheme == "dark"){
		fcgi->out << "Dark";
	}
	else if(data->cssTheme == "light"){
		fcgi->out << "Light";
	}
	else if(data->cssTheme == "aesthicc"){
		fcgi->out << "Aesthicc";
	}
	else if(data->cssTheme == "anime"){
		fcgi->out << "Animu";
	}
	else{
		fcgi->out << "ERROR";
	}
	
	fcgi->out << "</h3>"
	"<form method='post' action='https://" << Config::getDomain() << "/setCssTheme' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<select name='theme'>"
	"<option value='dark'>Dark</option>"
	"<option value='light'>Light</option>"
	"<option value='aesthicc'>Aesthicc</option>"
	"<option value='anime'>Animu</option>"
	"</select>"
	"<button type='submit' name='submit_param'>"
	"Change Theme"
	"</button>"
	"</form>";
	if(cssError != ""){
		fcgi->out << 
		"<p><div class='errorText'>"
		<< cssError <<
		"</div></p>";
	}
	fcgi->out << "<h3>Change Password</h3>"
	"<form method='post' action='https://" << Config::getDomain() << "/changePassword' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='password' name='oldPassword'>Old Password<br>"
	"<input type='password' name='newPassword'>New Password<br>"
	"<input type='password' name='repeatNewPassword'>Repeat New Password<br>"
	"<button type='submit' name='submit_param'>"
	"Change Password"
	"</button>"
	"</form>";
	if(passwordError != ""){
		fcgi->out << 
		"<p><div class='errorText'>"
		<< passwordError <<
		"</div></p>";
	}
	
	createPageFooter(fcgi, data);
}
