#pragma once

#include <memory>

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

#include "UserData.h"

bool hasRainbowTextPermissions(std::string position);
bool hasModerationPermissions(std::string position);
bool hasSubdatinControlPermissions(std::string position);
bool hasAdministrationControlPermissions(std::string position);