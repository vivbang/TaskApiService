#pragma once  
#include <string>  

struct Task  
{  
	int id; 
	std::string title;  
	std::string description;  
	bool completed = false; 
};