#pragma once
#include <fstream>
#include <mutex>
#include "wordlist.h"

class SaveHandler { //TO implement save file splitting
private:
	std::ofstream m_saveFile;
	//SaveConfig* m_config;
	std::string m_filePath;
	std::mutex m_fileLock;
public:
	//SaveHandler(SaveConfig* config);
	SaveHandler(std::string& path);
	SaveHandler(const char* path);
	~SaveHandler();
	void saveThreadStream(std::stringstream&&);
	//void setPath(std::string&);
};