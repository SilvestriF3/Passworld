#include "saveHandler.h"

SaveHandler::SaveHandler(std::string& path) : m_filePath{path} {
	m_saveFile.open(path, std::ios_base::out); //write to file
}
SaveHandler::SaveHandler(const char* path) {
	m_saveFile.open(path, std::ios_base::out); //write to file, maybe implement append in the future
}

SaveHandler::~SaveHandler() {
	m_saveFile.close();
}
void SaveHandler::saveThreadStream(std::stringstream&& threadStream) {
	if (threadStream.rdbuf()->in_avail() == 0) { return; } //check if stream is empty
	m_fileLock.lock();
	m_saveFile << threadStream.rdbuf(); 
	m_saveFile.flush();
	m_fileLock.unlock();
}
