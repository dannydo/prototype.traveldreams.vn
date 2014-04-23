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
	static CSWordTable* getInstance();
	bool init();
	CSWordTable();
	static void releaseInstance();

	std::vector<CSWordInfo>& getAllCSWordsForPackage(const std::string& sPackageId);
	CSWordInfo getCSWordInfoOnPackage(const std::string& sPackageId, const std::string& sCSWordId);

	bool updateCSWord(const CSWordInfo& csWordInfo);

private:
	void fetchCSWordsForPackage(const std::string& sPackageId);

	static CSWordTable* m_CSWordTable;
	std::vector<CSWordInfo> m_PackageCSWords;
	std::string m_sCurrentPackageId;
};
#endif