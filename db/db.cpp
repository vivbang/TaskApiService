#include <Db.h>
#include <sqlite3.h>
#include <sqlite_modern_cpp.h>
#include <vector>

using namespace sqlite;

Db::Db(const std::string& dbFile) :_dbFile(dbFile) {}

void Db::init() {
	database db(_dbFile);
    db <<
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT,"
        "description TEXT,"
        "completed INTEGER"
        ");";
}

std::vector<Task> Db::getAllTask()    
{
    database db(_dbFile);
    std::vector<Task> tasks;

    db << "SELECT id, title, description, completed FROM tasks;" >> 
        [&](int id, const std::string& title, const std::string& description, int completed)
        {
			tasks.emplace_back(id, title, description, static_cast<bool>(completed));
		};
	return tasks;
}

Task Db::createTask(const Task& task) {
    database db(_dbFile);
    db << "INSERT INTO tasks(title, description, completed ) VALUES (?,?,?);"
        << task.title
        << task.description
        << static_cast<int>(task.completed);
	int id = db.last_insert_rowid();
	return Task{ id, task.title, task.description, task.completed };
}

std::optional<Task> Db::getTask(int taskId) {
	database db(_dbFile);
	std::optional<Task> result;
    db << "SELECT id, title, description, completed FROM task WHERE id == ?;"
		<< taskId
		>> [&](int id, const std::string& title, const std::string& description, int completed) {
		result = Task{ id, title, description, static_cast<bool>(completed) };
		};
		
        return result;
}

bool Db::updateTask(int id, const Task& task) {
    database db(_dbFile);
    std::optional<Task> result;

    db << "Update tasks Set title = ?, description = ?, completed = ? WHERE id = ?;" << id
        << task.title
        << task.description
        << static_cast<int>(task.completed)
        << id;
    return db.rows_modified() > 0;
}


bool Db::deleteTask(int id) {
    database db(_dbFile);
    db << "DELTE FROM tasks WHERE ID = ?;" << id;

	// Check if any row modified after delete operation
    return db.rows_modified() > 0;
}

