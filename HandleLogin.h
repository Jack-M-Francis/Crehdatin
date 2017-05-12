#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>

#include "LoginPage.h"
#include "InputHandler.h"
#include "Cryptography.h"

void handleLogin(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);