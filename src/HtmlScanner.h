
#pragma once

#include <string>
#include <boost/filesystem.hpp>
#include <libxml/HTMLparser.h>
#include "Logger.h"

//#define DEBUG_LOGGING  // this option stores all the parsed tags to log file

using std::string;
namespace fs = boost::filesystem;

enum ScanResult
{
	Found		= 0,
	NotFound	= 1,
	ParseFailed = 2,
	OpenFailed	= 3
};

class CHtmlScanner
{
	string m_sTag;
	string m_sAttribute;
	string m_sRegex;
	string m_sPath;

	bool m_bCaseInsensitive;

	int m_nScanned;
	int m_nFound;
	int m_nFailedToParse;
	int m_nFailedToOpen;

	string m_sMatch;

	CLogger m_logger;

#ifdef DEBUG_LOGGING
	CLogger m_dbgLogger;
#endif // DEBUG_LOGGING

public:
	CHtmlScanner(const string& tag, const string& attribute, const string& regex, const string& path, const char* logfile, bool bCase);
	~CHtmlScanner();
	void Scan();

	const int GetNumScanned()const {return m_nScanned;}
	const int GetNumFound()const {return m_nFound;}
	const int GetNumFailedToParse()const {return m_nFailedToParse;}
	const int GetNumFailedToOpen()const {return m_nFailedToOpen;}

	bool ValidateRegex();
private:
	ScanResult ScanFile(string filename);
	bool ScanNode(xmlNode * node);
	bool ScanDoc(htmlDocPtr doc);
	bool ScanNodes(xmlNode* root_element);
	void ProcessFile(fs::path p);

	string CalcMd5(fs::path p);

	bool RegexSearch(xmlNode* node);
};

