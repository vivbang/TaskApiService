#include "task_service.h"


TaskService::TaskService(std::shared_ptr<Db> taskDb) : next_id_(0), _db(std::move(taskDb)) {}

std::vector<Task> TaskService::get_all_task() {
	std::lock_guard<std::mutex> _lock(_taskMutex);
	std::vector<Task> tasks;
	for (auto t : _tasks) {
		tasks.push_back(t.second);
	}
	return tasks;
}

Task TaskService::get_task(int id) {
	std::lock_guard<std::mutex> _lock(_taskMutex);
	auto it = _tasks.find(id);
	if (it != _tasks.end()) {
		return it->second;
	}
	else
	{
		throw std::runtime_error("Task not found");
	}
}

	Task TaskService::add_task(const Task & task)
	{
		std::lock_guard<std::mutex> _lock(_taskMutex);
		Task new_task = task;
		new_task.id = next_id_++;
		new_task.title = task.title;
		new_task.description = task.description;
		new_task.completed = task.completed;
		_tasks[new_task.id] = new_task;

		return new_task;
	}

	void TaskService::delete_task(int id) {
		std::lock_guard<std::mutex> _lock(_taskMutex);
		_tasks.erase(id);
	}

	void TaskService::update_task(int id, const Task & task) {
		std::lock_guard<std::mutex> _lock(_taskMutex);
		if (_tasks.find(id) != _tasks.end()) {
			Task updated_task = task;
			updated_task.id = id;
			_tasks[id] = updated_task;
		}
		else {
			throw std::runtime_error("Task not found");
		}
	}

