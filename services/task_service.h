#pragma once
#include "../models/task.h"
#include <unordered_map>	
#include <vector>
#include <mutex>
#include <memory>	
#include <db.h>


class TaskService {
public:
	TaskService(std::shared_ptr<Db> taskDb);
	
	std::vector<Task> get_all_task();
	Task get_task(int id);
	Task add_task(const Task& task);
	void delete_task(int id);
	void update_task(int id, const Task& task);

private:	
	std::mutex _taskMutex;
	int next_id_;
	std::shared_ptr<Db> _db;
};