#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

#include <WebsiteFramework/WebsiteFramework.h>
#include "RequestData.h"
#include "InputHandler.h"

void getUserData(sql::Connection* con, int64_t userId, std::string& userName, std::string& userPosition);
void setLastPostTime(FcgiData* fcgi, RequestData* data);
std::string getFormattedPosterString(sql::Connection* con, std::string anonId, int64_t userId);