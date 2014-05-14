#ifndef __CS_WORD_TABLE_H__
#define __CS_WORD_TABLE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

struct CSWordInfo
{
	std::string sCSWordId;
	std::string sPackageId;
	int iCollectedCount;

	CSWordInfo()
	{
		sCSWordId = "";
		sPackageId = "";
		iCollectedCount = 0;
	}
};

class CSWordTable
{
public:
	static void getAllCSWordsForPackage(std::vector<CSWordInfo>& packageCSWords, const std::string& sPackageId);
	static CSWordInfo getCSWordInfoOnPackage(const std::string& sPackageId, const std::string& sCSWordId);
	static bool updateCSWord(const CSWordInfo& csWordInfo);

private:
	
};
#endif