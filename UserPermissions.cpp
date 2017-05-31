#include "UserPermissions.h"

bool hasRainbowTextPermissions(std::string position){
	if(position == "senate" || position == "administrator" || position == "bureaucrat" || position == "moderator"){
		return true;
	}
	return false;
}

bool hasModerationPermissions(std::string position){
	if(position == "senate" || position == "administrator" || position == "bureaucrat" || position == "moderator"){
		return true;
	}
	return false;
}

bool hasSubdatinControlPermissions(std::string position){
	if(position == "senate" || position == "bureaucrat"){
		return true;
	}
	return false;
}

bool hasAdministrationControlPermissions(std::string position){
	if(position == "senate"){
		return true;
	}
	return false;
}