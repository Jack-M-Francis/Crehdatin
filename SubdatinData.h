#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>
#include <memory>

#include <WebsiteFramework/WebsiteFramework.h>
#include "RequestData.h"
#include "InputHandler.h"

int64_t getSubdatinId(sql::Connection* con, std::string subdatinTitle);
void getSubdatinData(sql::Connection* con, int64_t id, std::string& title, std::string& name, bool& postLocked, bool& commentLocked);