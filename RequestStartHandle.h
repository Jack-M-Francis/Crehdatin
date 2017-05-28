#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "Response.h"
#include "RequestData.h"
#include "Cryptography.h"

bool requestStartHandle(FcgiData* fcgi, void* _data);
bool getUserRequestData(FcgiData* fcgi, RequestData* data, std::string sessionToken);
void createNewSession(RequestData* data);