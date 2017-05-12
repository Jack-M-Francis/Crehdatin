#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>
#include "UserData.h"
#include "ThreadPage.h"

#include "PageHeader.h"
#include "PageFooter.h"

void createReportsPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
void createReportedThread(FcgiData* fcgi, RequestData* data, int64_t threadId);
void createReportedComment(FcgiData* fcgi, RequestData* data, int64_t threadId, int64_t commentId);
void createReport(FcgiData* fcgi, RequestData* data, std::string& reason, std::string& ip, int64_t userId);