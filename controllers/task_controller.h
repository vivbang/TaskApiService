#pragma once
#include <cpprest/http_listener.h>
#include <task_service.h>

using namespace web;
using namespace http;
using namespace http::experimental::listener;

class TaskController {
public:
	TaskController(const utility::string_t& url, TaskService taskService);

	void open();
	void close();	

private:
	http_listener _listener;
	TaskService _taskService;

	void handle_get(http_request request);
	void handle_post(http_request request);
	void handle_put(http_request request);
	void handle_delete(http_request request);
};