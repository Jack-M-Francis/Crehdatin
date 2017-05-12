#include "Config.h"

std::string Config::getSqlAddress(){
	return "127.0.0.1:3306";
}

std::string Config::getSqlUserName(){
	return "Crehdatin";
}

std::string Config::getSqlPassword(){
	return "password";
}

std::string Config::getSqlDatabaseName(){
	return "Crehdatin";
}

std::string Config::getDomain(){
	return "crehdatin.karagory.com";
	//return "website.cloud.karagory.com";
}

std::size_t Config::getUniqueTokenLength(){
	return 512;
}

std::size_t Config::getMaxNameLength(){
	return 64;
}

std::size_t Config::getMaxPasswordLength(){
	return 512;
}

std::size_t Config::getMaxPostLength(){
	return 2048;
}

std::size_t Config::getMaxTitleLength(){
	return 64;
}

int64_t Config::anonPostingTimeout(){
	return 15;
}

int64_t Config::userPostingTimeout(){
	return 15;
}

int64_t Config::anonReportingTimeout(){
	return 15;
}

int64_t Config::userReportingTimeout(){
	return 15;
}

std::size_t Config::getMaxReportLength(){
	return 100;
}

bool Config::hasDeletePermissions(std::string& position){
	if(position == "moderator" || position == "admin" || position == "senate"){
		return true;
	}
	else{
		return false;
	}
}