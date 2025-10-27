#include "Log.h"


#ifdef _DEBUG
BlackBoxEngine::Logger::~Logger()
{
	if(m_logFile.is_open())
		m_logFile.close();
}

void BlackBoxEngine::Logger::AppendToLogFile(const char* str)
{
	if (!m_logFile.is_open())
		m_logFile.open(logFilePath, std::ios::out); // Creates file for first time.
	assert(m_logFile.is_open());

	m_logFile << str << '\n';
}

void BlackBoxEngine::Logger::OpenLog()
{
    m_logFile.open( logFilePath, std::ios::out ); // Creates file for first time.;
}
#endif