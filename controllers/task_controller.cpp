#include "task_controller.h"  
#include "task_service.h"
#include <iostream>  

TaskController::TaskController(const utility::string_t& url, TaskService taskService) :_listener(url), _taskService(std::move(taskService))
{
	// Set the listener to handle different HTTP methods
	_listener.support(web::http::methods::OPTIONS, [](http_request request) {
		request.reply(web::http::status_codes::OK);
		});
	// Handle GET, POST, PUT, DELETE requests
	/*In more detail:
	std::bind(&TaskController::handle_get, this, std::placeholders::_1) creates a callable object (functor) that,
	when called, will internally do this->handle_get(X), where X is whatever argument you pass to that callable.
	
	The this binds the object instance (so it knows on which TaskController to call handle_get).
	The std::placeholders::_1 means: “take the first argument passed to this bound callable, and
	forward it into handle_get as its first argument.”*/

	_listener.support(web::http::methods::GET, std::bind(&TaskController::handle_get, this, std::placeholders::_1));
	_listener.support(web::http::methods::POST, std::bind(&TaskController::handle_post, this, std::placeholders::_1));
	_listener.support(web::http::methods::PUT, std::bind(&TaskController::handle_put, this, std::placeholders::_1));
	_listener.support(web::http::methods::DEL, std::bind(&TaskController::handle_delete, this, std::placeholders::_1));
}

void TaskController::open()
{
	_listener
		.open()
		.then([this]() { std::wcout << L"Starting to listen at: " << _listener.uri().to_string() << std::endl; })
		.wait();
}
void TaskController::close()
{
	_listener
		.close() // close return a pplx::task, ".then" is used to set a continuation task
		.then([this]() { std::wcout << L"Stopping to listen at: " << _listener.uri().to_string() << std::endl; })
		.wait();// wait() is used to block the current thread until the task is completed
}

// Term	Value
// Absolute URI	http ://localhost:8080/api/tasks/42?verbose=true
// Listener base path / api
// Relative URI / tasks / 42 ? verbose = true
void TaskController::handle_get(http_request request)
{	
	auto paths = uri::split_path(uri::decode(request.relative_uri().path()));
	if (paths.empty())
	{
		auto tasks = _taskService.get_all_task();
		json::value result = json::value::array();
		int index = 0;
		for (const auto& t : tasks)
		{
			json::value task;
			task[U("id")] = json::value::number(t.id);
			task[U("title")] = json::value::string(utility::conversions::to_string_t(t.title)); // Convert std::string to utility::string_t (platform independent string)
			task[U("description")] = json::value::string(utility::conversions::to_string_t(t.description)); // Convert std::string to utility::string_t
			task[U("completed")] = json::value::boolean(t.completed);
			result[index++] = task;
		}
		request.reply(web::http::status_codes::OK, result);
		return;
	}
	else
	{
		try {
			auto id = std::stoi(paths[0]);
			auto task = _taskService.get_task(id);

			json::value result;
			result[U("id")] = json::value::number(task.id);
			result[U("title")] = json::value::string(utility::conversions::to_string_t(task.title)); // Convert std::string to utility::string_t (platform independent string)
			result[U("description")] = json::value::string(utility::conversions::to_string_t(task.description)); // Convert std::string to utility::string_t
			result[U("completed")] = json::value::boolean(task.completed);
			request.reply(web::http::status_codes::OK, result);
		}
		catch (...)
		{
			request.reply(web::http::status_codes::BadRequest, U("Id not found"));
		}
	}
}

void TaskController::handle_post(http_request request)  
{  
	request.extract_json().then([request, this](json::value body)
		{
			Task t{ -1,
			utility::conversions::to_utf8string(body[U("title")].as_string()), // Convert utility::string_t to std::string  
			utility::conversions::to_utf8string(body[U("description")].as_string()), // Convert utility::string_t to std::string  
			body[U("completed")].as_bool()
		};
		auto createdTask = _taskService.add_task(t);
		json::value result;
		result[U("id")] = json::value::number(createdTask.id);
		result[U("title")] = json::value::string(utility::conversions::to_string_t(createdTask.title)); // Convert std::string to utility::string_t (platform independent string)
		result[U("description")] = json::value::string(utility::conversions::to_string_t(createdTask.description)); // Convert std::string to utility::string_t
		result[U("completed")] = json::value::boolean(createdTask.completed);

		request.reply(web::http::status_codes::Created, result);
       });
}

void TaskController::handle_put(http_request request)
{
	auto paths = uri::split_path(uri::decode(request.relative_uri().path()));
	if (paths.empty())
	{
		request.reply(web::http::status_codes::BadRequest, U("Id not found"));
		return;
	}

	request.extract_json().then([=](json::value body) {
		if (body.is_null())
		{
			request.reply(web::http::status_codes::BadRequest, U("Invalid JSON"));
			return;
		}

		try {
			Task t{ -1,
				utility::conversions::to_utf8string(body[U("title")].as_string()), // Convert utility::string_t to std::string  
				utility::conversions::to_utf8string(body[U("description")].as_string()), // Convert utility::string_t to std::string  
				body[U("completed")].as_bool()
			};
			_taskService.update_task(std::stoi(paths[0]), t);

			json::value result;
			result[U("id")] = json::value::number(t.id);
			result[U("title")] = json::value::string(utility::conversions::to_string_t(t.title)); // Convert std::string to utility::string_t (platform independent string)
			result[U("description")] = json::value::string(utility::conversions::to_string_t(t.description)); // Convert std::string to utility::string_t
			result[U("completed")] = json::value::boolean(t.completed);
			request.reply(web::http::status_codes::OK, result);
		}
		catch (...)
		{
			request.reply(web::http::status_codes::BadRequest, U("Id not found"));
		}
		});
}

void TaskController::handle_delete(http_request request)
{
	auto paths = uri::split_path(uri::decode(request.relative_uri().path()));
	if (paths.empty())
	{
		request.reply(web::http::status_codes::BadRequest, U("Id not found"));
		return;
	}
	auto id = std::stoi(paths[0]);
	try {
		_taskService.delete_task(id);
	}
	catch (...)
	{
		request.reply(web::http::status_codes::BadRequest, U("Id not found"));
		return;
	}
}
