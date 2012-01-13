
#pragma once

#include <fstream>

using std::ofstream;
using std::endl;

class CLogger
{
	ofstream m_ofs;

public:
	CLogger(const char* file):m_ofs(file, std::ios_base::out | std::ios_base::trunc){}
	~CLogger(){ m_ofs.close();}

	void LogMessageLine(const char* msg){if(m_ofs) m_ofs << msg << endl;}
	void LogMessage(const char* msg){if(m_ofs) m_ofs << msg;}
	void LogFormatMessage(const char* fmt, const char* param);

	bool IsAvailable() { return (m_ofs != 0); }
};

inline void CLogger::LogFormatMessage(const char* fmt, const char* param)
{
	char szLogMsg[128] = {0};
	sprintf(szLogMsg, "\t<%s>", param);
	LogMessageLine(szLogMsg);
}
