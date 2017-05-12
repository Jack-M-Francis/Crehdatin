#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

#include <WebsiteFramework/WebsiteFramework.h>
#include "UserData.h"

#include "InputHandler.h"
#include "StringHelper.h"

#include "PageHeader.h"
#include "PageFooter.h"

void formatUserPostBody(std::string& body);
void createThreadPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
void createCommentLine(FcgiData* fcgi, RequestData* data, std::string& threadId, int64_t layer = 0, int64_t parentId = -1);
void createReportMenu(FcgiData* fcgi, RequestData* data, std::string& threadId, int64_t commentId = -1);
void createReplyMenu(FcgiData* fcgi, RequestData* data, std::string& threadId, int64_t commentId = -1);


