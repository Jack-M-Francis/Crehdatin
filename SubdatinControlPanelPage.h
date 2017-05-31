#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>

#include "PageHeader.h"
#include "PageFooter.h"

void createSubdatinControlPanelPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
void createSubdatinControlPanelPage(FcgiData* fcgi, RequestData* data, int64_t subdatinId);