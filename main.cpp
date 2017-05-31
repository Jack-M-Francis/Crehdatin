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
#include "SubdatinPage.h"
#include "LoginPage.h"
#include "SettingsPage.h"
#include "NewThreadPage.h"
#include "ThreadPage.h"
#include "ReportsPage.h"
#include "UserPage.h"
#include "SubdatinControlPanelPage.h"
#include "CrehdatinControlPanelPage.h"

#include "HandleCreateAccount.h"
#include "HandleLogin.h"
#include "HandleLogout.h"
#include "HandleNewThread.h"
#include "HandleNewComment.h"
#include "HandleSetCssTheme.h"
#include "HandleChangePassword.h"

#include "HandleAddAdministrator.h"
#include "HandleRemoveAdministrator.h"

#include "HandleReportThread.h"
#include "HandleDeleteThread.h"
#include "HandleReportComment.h"
#include "HandleDeleteComment.h"
#include "HandleDismissReports.h"

#include "RequestStartHandle.h"

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
		WebsiteFramework::addGetHandleMap("/controlPanel", createCrehdatinControlPanelPageHandle);
		WebsiteFramework::addGetHandleMap("/d/*", createSubdatinPage);
		WebsiteFramework::addGetHandleMap("/d/*/newThread", createNewThreadPageHandle);
		WebsiteFramework::addGetHandleMap("/d/*/thread/*", createThreadPage);
		WebsiteFramework::addGetHandleMap("/d/*/reports", createReportsPage);
		WebsiteFramework::addGetHandleMap("/d/*/controlPanel", createSubdatinControlPanelPageHandle);
		WebsiteFramework::addGetHandleMap("/user/*", createUserPage);
		
		WebsiteFramework::addPostHandleMap("/createAccount", handleCreateAccount);
		WebsiteFramework::addPostHandleMap("/login", handleLogin);
		WebsiteFramework::addPostHandleMap("/logout", handleLogout);
		WebsiteFramework::addPostHandleMap("/setCssTheme", handleSetCssTheme);
		WebsiteFramework::addPostHandleMap("/changePassword", handleChangePassword);
		WebsiteFramework::addPostHandleMap("/d/*/newThread", handleNewThread);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/newComment", handleNewComment);
		
		WebsiteFramework::addPostHandleMap("/addAdministrator", handleAddAdministrator);
		WebsiteFramework::addPostHandleMap("/removeAdministrator", handleRemoveAdministrator);
		
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/reportThread", handleReportThread);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/deleteThread", handleDeleteThread);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/reportComment", handleReportComment);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/deleteComment", handleDeleteComment);
		WebsiteFramework::addPostHandleMap("/d/*/dismissReports", handleDismissReports);
		
		WebsiteFramework::run(":8222", std::thread::hardware_concurrency() * 16);
		
		std::cout << "Shutting down...\n";
	}
	
    return 0;
}

















