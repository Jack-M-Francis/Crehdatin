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

#include "RequestStartHandle.h"

#include "Recaptcha.h"

int main(int argc, char** argv){
	
	std::cout << generateSecureHash("smoke", "sodiumChloride") << std::endl;
	
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
			
			data->stmt = data->con->createStatement();
			
			data->con->setSchema(Config::getSqlDatabaseName());								
			data->stmt->execute("SET NAMES utf8mb4");
			
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
		
		WebsiteFramework::setRequestStartHandle(requestStartHandle);
		
		WebsiteFramework::setRequestEndHandle([](FcgiData* fcgi, void* _data){
			
			
		});
		
		WebsiteFramework::setError404Handle([](FcgiData* fcgi, void* _data){
			RequestData* data = (RequestData*)_data;
			createPageHeader(fcgi, data);
			fcgi->out << "<div class='errorText'>This Page Does Not Exist</div>";
			createPageFooter(fcgi, data);
		});
		
		WebsiteFramework::setExceptionHandle([](void* _data, std::exception* e){
				std::cout << "std::exception.what()" << e->what() << "\n";
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

















