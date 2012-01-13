#include "HtmlScanner.h"

#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/regex.hpp>

#include <crypto++/md5.h>
#include <crypto++/hex.h>
#include <crypto++/files.h>

#define LOG_MSG_SIZE 1024

using std::cout;

CHtmlScanner::CHtmlScanner(const string& tag, const string& attribute, const string& regex, const string& path, const char* logfile, bool bCase)
#ifdef DEBUG_LOGGING
	:m_sTag(tag)
	,m_sAttribute(attribute)
	,m_sRegex(regex)
	,m_sPath(path)
	,m_bCaseInsensitive(bCase)
	,m_nScanned(0)
	,m_nFound(0)
	,m_nFailedToParse(0)
	,m_nFailedToOpen(0)
	,m_sMatch()
	,m_logger(logfile)
	,m_dbgLogger("debuglog.log")
#else
	:m_sTag(tag)
	,m_sAttribute(attribute)
	,m_sRegex(regex)
	,m_sPath(path)
	,m_bCaseInsensitive(bCase)
	,m_nScanned(0)
	,m_nFound(0)
	,m_nFailedToParse(0)
	,m_nFailedToOpen(0)
	,m_sMatch()
	,m_logger(logfile)
#endif // DEBUG_LOGGING
{
}

CHtmlScanner::~CHtmlScanner()
{
	xmlCleanupParser();
}

void CHtmlScanner::Scan()
{
	fs::path p(m_sPath);
    try
    {
        if(exists(p))
        {
			fs::recursive_directory_iterator end;

			for(fs::recursive_directory_iterator it(p); it != end; ++it)
			{
				if(is_regular_file(*it))
				{
					ProcessFile(it->path());
				}
			}
        }
        else
            std::cout << p << " does not exist\n";
    }
    catch(const fs::filesystem_error& ex)
    {
        std::cout << ex.what() << '\n';
    }
}

bool CHtmlScanner::ScanNodes(xmlNode* root_element)
{
    xmlNode *cur_node = NULL;
	bool res = false;

    for (cur_node = root_element; cur_node; cur_node = cur_node->next)
	{
        if (XML_ELEMENT_NODE == cur_node->type)
		{
#ifdef DEBUG_LOGGING
			m_dbgLogger.LogFormatMessage("\t<%s>", (const char*)cur_node->name);
#endif //DEBUG_LOGGING
			if(res = ScanNode(cur_node))
				break;
		}

		res = ScanNodes(cur_node->children);
    }

	return res;
}

bool CHtmlScanner::ScanDoc(htmlDocPtr doc)
{
	xmlNode *root_element = NULL;
	root_element = xmlDocGetRootElement(doc);

	if(root_element)
	{
		return ScanNodes(root_element);
	}

	return false;
}

bool CHtmlScanner::ScanNode(xmlNode * node)
{
	xmlAttr *attr = NULL;

	if(!node)
		return false;

	if (0 == strcmp((const char*)node->name, m_sTag.c_str()))
	{
		if(m_sAttribute.size())
		{
			for (attr = node->properties; attr; attr = attr->next)
			{
				if (XML_ATTRIBUTE_NODE == attr->type)
				{
					if (0 == strcmp((const char*)attr->name, m_sAttribute.c_str()))
					{
						return RegexSearch(attr->children);
					}
				}
			}
		}
		else
		{
			return RegexSearch(node->children);
		}
	}

	return false;
}

bool CHtmlScanner::RegexSearch(xmlNode* node)
{
    if(!node || !node->content)
		return false;

	boost::smatch what;
	string search((const char*)node->content);

	std::string::const_iterator start = search.begin();
	std::string::const_iterator end = search.end();

	bool res = false;

	/*As stated in Boost documentation, boost::regex_search throws
	std::runtime_error if the complexity of matching the expression
	against an N character string begins to exceed O(N2),
	or if the program runs out of stack space while matching the expression */
	try
	{
		res = boost::regex_search(start, end, what,
			boost::regex(m_sRegex.c_str(), m_bCaseInsensitive ?
				boost::regbase::icase : boost::regbase::normal));
	}
	catch(std::runtime_error& e)
	{
		cout << e.what() << "\"" << endl;
	}

	if(res)
		m_sMatch = string(what[0].first, what[0].second);

	return res;
}

ScanResult CHtmlScanner::ScanFile(string filename)
{
	m_nScanned++;
	ScanResult res = NotFound;

	htmlDocPtr doc = htmlReadFile(filename.c_str(), 0/*"cp1251"*/, HTML_PARSE_NOERROR);

#ifdef DEBUG_LOGGING
	m_dbgLogger.LogFormatMessage("file %s scanning...", filename.c_str());
#endif // DEBUG_LOGGING

	if(!doc)
	{
		m_nFailedToParse++;
		return ParseFailed;
	}

	if(ScanDoc(doc))
	{
		m_nFound++;
		res = Found;
		std::cout << "\t" << filename.c_str() << std::endl;
	}

	xmlFreeDoc(doc);

	return res;
}

void CHtmlScanner::ProcessFile(fs::path p)
{
	ScanResult res = ScanFile(p.string());

	if(NotFound == res)
		return;

	if(!m_logger.IsAvailable())
		return;

	char szLogMessage[LOG_MSG_SIZE] = {0};

	fs::path path_complete = fs::system_complete(p.string());

	if(Found == res)
	{
		sprintf(szLogMessage,
			"%s,%s,tag=%s,attribute=%s,regex=%s,match=%s",
			path_complete.string().c_str(),
			CalcMd5(path_complete).c_str(),
			m_sTag.c_str(),
			m_sAttribute.c_str(),
			m_sRegex.c_str(),
			m_sMatch.c_str()
		);
	}
	else if(ParseFailed == res)
	{
		sprintf(szLogMessage,
			"%s,%s,,,parse failed!",
			path_complete.string().c_str(),
			CalcMd5(path_complete).c_str()
		);
	}
	else if(OpenFailed == res)
	{
		sprintf(szLogMessage,
			"%s,%s,,,open failed!",
			path_complete.string().c_str(),
			CalcMd5(path_complete).c_str()
		);
	}

	m_logger.LogMessageLine(szLogMessage);

}

string CHtmlScanner::CalcMd5(fs::path p)
{
    CryptoPP::MD5 hash;
    string value;

    CryptoPP::FileSource(p.string().c_str(), true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(value), true
            )
        )
    );

    return value;
}

bool CHtmlScanner::ValidateRegex()
{
	try
	{
		boost::regex re(m_sRegex, m_bCaseInsensitive ?
				(boost::regbase::normal | boost::regbase::icase) : boost::regbase::normal);
	}
	catch(boost::regex_error& e)
	{
		cout << m_sRegex << " is not valid regular expression: \""
			<< e.what() << "\"" << endl;
		return false;
	}
	return true;
}
