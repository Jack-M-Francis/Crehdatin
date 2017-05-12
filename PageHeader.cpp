#include "PageHeader.h"

void createPageHeader(FcgiData* fcgi, RequestData* data){
	sendStatusHeader(fcgi->out, StatusCode::Ok);
	sendHtmlContentTypeHeader(fcgi->out);
	finishHttpHeader(fcgi->out);
	
	fcgi->out << 
	"<html>"
	"<head>"
	"<link rel='stylesheet' type='text/css' href='https://" << Config::getDomain() << "/static/style.css'>"
	"<meta charset='UTF-8'>"
	"<title>"
	"Creh-Datin"
	"</title>"
	"<script src='https://www.google.com/recaptcha/api.js'></script>"
	"</head>"
	"<body>"
	"<header>"
	"<div id='headerText'>"
	"<a href='https://" << Config::getDomain() << "/'>"
	"Creh-Datin"
	"</a>"
	"</div>"
	"<div id='toolbar'>";
	if(data->userId == -1){
		fcgi->out << 
		"<div class='toolbarEntry'>"
		"<a href='https://" << Config::getDomain() << "/login'>Login/Create Account</a>"
		"</div>"
		"<div class='toolbarEntry'>"
		"<a href='https://" << Config::getDomain() << "/newThread'>New Thread</a>"
		"</div>"
		"<div class='toolbarEntry'>"
		"Welcome, Anon! Id: " << data->shownId << 
		"</div>";
	}
	else{
		fcgi->out << 
		"<form method='post' action='https://" << Config::getDomain() << "/logout' class='logout'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<button type='submit' name='submit_param' class='logout-button'>"
		"Logout"
		"</button>"
		"</form>";
		if(Config::hasDeletePermissions(data->userPosition)){
			fcgi->out << "<div class='toolbarEntry'>"
			"<a href='https://" << Config::getDomain() << "/reports'>Reports</a>"
			"</div>";
		}
		fcgi->out << "<div class='toolbarEntry'>"
		"<a href='https://" << Config::getDomain() << "/newThread'>New Thread</a>"
		"</div>"
		"<div class='toolbarEntry'>"
		"Welcome back, " << escapeHtml(data->userName) << "!"
		"</div>";
	}
	fcgi->out << 
	"</div>"
	"</header>"
	"<div id='content'>";
}
