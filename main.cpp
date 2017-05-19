#include <iostream>
#include <thread>
#include <vector>
#include <fcgio.h>

#include <sys/socket.h>
#include <csignal>

#include <WebsiteFramework/WebsiteFramework.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "mysql_driver.h" 

#include "Database.h"
#include "Cryptography.h"
#include "RequestData.h"
#include "Response.h"

#include "MainPage.h"
#include "LoginPage.h"
#include "SettingsPage.h"
#include "NewThreadPage.h"
#include "ThreadPage.h"

#include "HandleCreateAccount.h"
#include "HandleLogin.h"
#include "HandleLogout.h"
#include "HandleNewThread.h"
#include "HandleNewComment.h"
#include "HandleSetCssTheme.h"

#include "HandleReportThread.h"
#include "HandleDeleteThread.h"
#include "HandleReportComment.h"
#include "HandleDeleteComment.h"
#include "ReportsPage.h"
#include "HandleDismissReports.h"

#include "Recaptcha.h"

int main(int argc, char** argv){
	
	if(argc > 1){
		if(argc == 2){
			if(argv[1] == std::string("--install")){
				Database::createDatabase();
			}
			else if(argv[1] == std::string("--uninstall")){
				Database::deleteDatabase();
			}
			else{
				std::cout << "Unknown argument \"" << argv[1] << "\"\n";
			}
		}
		else{
			std::cout << "Only accepts one argument\n";
		}
		return 0;
	}
	else{
		sql::Driver* driver = sql::mysql::get_driver_instance();
		
		WebsiteFramework::setThreadStartHandle([driver]()->void*{
			RequestData* data = new RequestData;
			
			data->con = driver->connect(Config::getSqlAddress(), Config::getSqlUserName(), Config::getSqlPassword());
			
			static bool reconnect = true;
			
			data->con->setClientOption("MYSQL_OPT_RECONNECT", &reconnect);
			data->con->setClientOption("OPT_RECONNECT", &reconnect);

			data->stmt = data->con->createStatement();
			
			//data->con->setSchema(Config::getSqlDatabaseName());								
			//data->stmt->execute("SET NAMES utf8mb4");
			
			return (void*)data;
		});
		
		WebsiteFramework::setThreadEndHandle([](void* _data){
			RequestData* data = (RequestData*)_data;
			
			delete data->stmt;
			data->stmt = nullptr;
			delete data->con;
			data->con = nullptr;
			delete data;
		});
		
		WebsiteFramework::setRequestStartHandle([](FcgiData* fcgi, void* _data)->bool{
			RequestData* data = (RequestData*)_data;
			
			sql::ResultSet* res = data->stmt->executeQuery("SELECT UNIX_TIMESTAMP(CURRENT_TIMESTAMP)");
			
			res->first();
			
			data->currentTime = res->getInt64(1);
			
			delete res;
			
			data->con->setSchema(Config::getSqlDatabaseName());								
			data->stmt->execute("SET NAMES utf8mb4");
			
			data->sessionToken = "";
			data->userName = "";
			data->userId = -1;
			data->authToken = "";
			data->shownId = "";
			data->userPosition = "";
			data->cssTheme = "dark";
			data->blocked = false;
			data->lastPostTime = 0;
			
			sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT " 
			"blocked, UNIX_TIMESTAMP(lastPostTime) "
			"FROM ips "
			"WHERE ip = ?");
			prepStmt->setString(1, fcgi->env->getRemoteAddr());
			res = prepStmt->executeQuery();
			res->beforeFirst();
			
			if(res->next()){
				data->blocked = res->getBoolean("blocked");
				data->lastPostTime = res->getInt64("UNIX_TIMESTAMP(lastPostTime)");
			}
			
			delete res;
			delete prepStmt;
			
			std::vector<cgicc::HTTPCookie> cookies = fcgi->env->getCookieList();
			for(auto i = cookies.begin(); i != cookies.end(); i++){
				if(i->getName() == "sessionToken"){
					sql::PreparedStatement* prepStmt = data->con->prepareStatement("SELECT " 
					"id, userId, authToken, shownId "
					"FROM sessions "
					"WHERE sessionToken = ?");
					prepStmt->setString(1, i->getValue());
					res = prepStmt->executeQuery();
					res->beforeFirst();
					if(res->next()){
						data->sessionToken = i->getValue();
						if(!res->isNull("userId")){
							data->userId = res->getInt64("userId");
							
							sql::PreparedStatement* prepStmtB = data->con->prepareStatement("SELECT " 
							"userName, userPosition, cssTheme, UNIX_TIMESTAMP(lastPostTime) "
							"FROM users "
							"WHERE id = ?");
							
							prepStmtB->setInt64(1, data->userId);
							
							sql::ResultSet* resB = prepStmtB->executeQuery();
							
							resB->beforeFirst();
							
							if(resB->next()){
								data->userName = resB->getString("userName");
								
								if(resB->isNull("userPosition") == false){
									data->userPosition = resB->getString("userPosition");
								}
								
								if(resB->isNull("cssTheme") == false){
									data->cssTheme = resB->getString("cssTheme");
								}
								
								if(resB->isNull("UNIX_TIMESTAMP(lastPostTime)") == false){
									data->lastPostTime = resB->getInt64("UNIX_TIMESTAMP(lastPostTime)");
								}
								
								delete resB;
								delete prepStmtB;
							}
							else{
								delete resB;
								delete prepStmtB;
								delete res;
								delete prepStmt;
								sendStatusHeader(fcgi->out, StatusCode::Ok);
								sendHtmlContentTypeHeader(fcgi->out);
								sendDeleteCookieHeader(fcgi->out, "sessionToken");
								finishHttpHeader(fcgi->out);
								fcgi->out << "<html><body>An unknown, internal server error occurred</body></html>";
								return false;
							}
						}
						data->authToken = res->getString("authToken");
						if(!res->isNull("shownId")){
							data->shownId = res->getString("shownId");
						}
					}
					delete res;
					delete prepStmt;
					break;
				}
			}
			
			if(data->blocked){
				sendStatusHeader(fcgi->out, StatusCode::Ok);
				sendHtmlContentTypeHeader(fcgi->out);
				finishHttpHeader(fcgi->out);
				fcgi->out << "<html><body>You are blocked</body></html>";
				return false;
			}
			
			if(data->sessionToken.size() == 0){
				sql::PreparedStatement* prepStmt = data->con->prepareStatement("INSERT INTO sessions "
				"(sessionToken, authToken, shownId) SELECT ?, ?, ? FROM DUAL WHERE NOT EXISTS "
				"(SELECT id FROM sessions WHERE sessionToken=? OR authToken=? OR shownId=?)");
				
				int64_t rowCount;
				
				do{
					data->sessionToken = generateRandomToken();
					data->authToken = generateRandomToken();
					data->shownId = generateRandomToken().substr(0, 12);
					
					prepStmt->setString(1, data->sessionToken);
					prepStmt->setString(2, data->authToken);
					prepStmt->setString(3, data->shownId);
					prepStmt->setString(4, data->sessionToken);
					prepStmt->setString(5, data->authToken);
					prepStmt->setString(6, data->shownId);
					prepStmt->execute();
					
					res = data->stmt->executeQuery("SELECT ROW_COUNT()");
					
					res->first();
					
					rowCount = res->getInt64(1);
					
					delete res;
					
				}while(rowCount == 0);
				
				delete prepStmt;
				
				sendCookieHeader(fcgi->out, "sessionToken", data->sessionToken);
			}
			return true;
		});
		
		WebsiteFramework::setRequestEndHandle([](FcgiData* fcgi, void* _data){
			
			
		});
		
		WebsiteFramework::setError404Handle([](FcgiData* fcgi, void* _data){
			RequestData* data = (RequestData*)_data;
			createPageHeader(fcgi, data);
			fcgi->out << "<div class='errorText'>This Page Does Not Exist</div>";
			createPageFooter(fcgi, data);
		});
		
		WebsiteFramework::setExceptionHandle([](void* _data, std::exception* e){
			if(dynamic_cast<sql::SQLException*>(e) != nullptr){
				std::cout << "sql::SQLException::what()" << e->what() << "\n";
			}
			else{
				throw *e;
			}
		});
		
		WebsiteFramework::addGetHandleMap("/", createMainPage);
		WebsiteFramework::addGetHandleMap("/createAccount", createCreateAccountPageHandle);
		WebsiteFramework::addGetHandleMap("/login", createLoginPageHandle);
		WebsiteFramework::addGetHandleMap("/settings", createSettingsPageHandle);
		WebsiteFramework::addGetHandleMap("/newThread", createNewThreadPageHandle);
		WebsiteFramework::addGetHandleMap("/thread/*", createThreadPage);
		WebsiteFramework::addGetHandleMap("/reports", createReportsPage);
		
		WebsiteFramework::addPostHandleMap("/createAccount", handleCreateAccount);
		WebsiteFramework::addPostHandleMap("/login", handleLogin);
		WebsiteFramework::addPostHandleMap("/logout", handleLogout);
		WebsiteFramework::addPostHandleMap("/setCssTheme", handleSetCssTheme);
		WebsiteFramework::addPostHandleMap("/newThread", handleNewThread);
		WebsiteFramework::addPostHandleMap("/thread/*/newComment", handleNewComment);
		
		WebsiteFramework::addPostHandleMap("/thread/*/reportThread", handleReportThread);
		WebsiteFramework::addPostHandleMap("/thread/*/deleteThread", handleDeleteThread);
		WebsiteFramework::addPostHandleMap("/thread/*/reportComment", handleReportComment);
		WebsiteFramework::addPostHandleMap("/thread/*/deleteComment", handleDeleteComment);
		WebsiteFramework::addPostHandleMap("/dismissReports", handleDismissReports);
		
		WebsiteFramework::run(":8222", std::thread::hardware_concurrency() * 16);
		
		std::cout << "Shutting down...\n";
	}
	
    return 0;
}

















