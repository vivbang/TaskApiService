// TaskApiService.cpp : Defines the entry point for the application.
//

#include<cpprest/http_listener.h>
#include<cpprest/json.h>
#include<iostream>
#include <task_controller.h>
#include <db.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace std;

int main()
{
	auto db = std::make_shared<Db>("tasks.db");
	TaskService taskService(db);
	
	TaskController taskController(U("http://localhost:8080/api/tasks"), taskService);
	taskController.open();

	std::string line;
	std::getline(std::cin, line);

	taskController.close();
	return 0;
}
