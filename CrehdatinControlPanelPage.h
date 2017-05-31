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

void createCrehdatinControlPanelPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
void createCrehdatinControlPanelPage(FcgiData* fcgi, RequestData* data);