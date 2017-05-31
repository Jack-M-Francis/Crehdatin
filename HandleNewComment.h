#pragma once

#include <memory>

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>
#include "RequestData.h"
#include "PageHeader.h"
#include "PageFooter.h"
#include "UserData.h"

#include "InputHandler.h"

void handleNewComment(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
void handleNewCommentErrorPage(FcgiData* fcgi, RequestData* data, int64_t subdatinId, std::string error);