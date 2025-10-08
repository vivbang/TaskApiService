#pragma once
#include <task.h>
#include <vector>
#include <optional>

class Db {
public :
	Db(const std::string& dbFile);
	void init();

	std::vector<Task> getAllTask();
	Task createTask(const Task& task);
	std::optional<Task> getTask(int id);
	bool updateTask(int id, const Task& task);
	bool deleteTask(int id);

private:	
	std::string _dbFile;
};