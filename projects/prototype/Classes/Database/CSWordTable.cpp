#include "CSWordTable.h"
#include "InitDatabase.h"

USING_NS_CC; 
USING_NS_CC_EXT;

CSWordTable* CSWordTable::m_CSWordTable = NULL;

CSWordTable::CSWordTable()
{

}

void CSWordTable::releaseInstance()
{
	if (m_CSWordTable == NULL)
	{
		delete m_CSWordTable;
	}
	m_CSWordTable = NULL;	
}

CSWordTable* CSWordTable::getInstance()
{
	if (m_CSWordTable == NULL) {
		m_CSWordTable = new CSWordTable();
		m_CSWordTable->init();
	}

	return m_CSWordTable;
}

bool CSWordTable::init()
{
	m_sCurrentPackageId = "";
	return true;
}

void CSWordTable::fetchCSWordsForPackage(const std::string& sPackageId)
{
	while(!m_PackageCSWords.empty())
	{
		m_PackageCSWords.pop_back();
	}

	char **re;
	int nRow, nColumn;

	String sql = "select * from CSWords where PackageId=";
	sql.appendWithFormat("'%s'", sPackageId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		CSWordInfo csWordInfo;
		csWordInfo.sCSWordId = re[iRow*nColumn+0];
		csWordInfo.sPackageId = re[iRow*nColumn+1];
		csWordInfo.iCollectedCount = int(strtod(re[iRow*nColumn+2], 0));

		m_PackageCSWords.push_back(csWordInfo);
	}

	sqlite3_free_table(re);
}

std::vector<CSWordInfo>& CSWordTable::getAllCSWordsForPackage(const std::string& sPackageId)
{
	if(m_sCurrentPackageId != sPackageId)
	{
		this->fetchCSWordsForPackage(sPackageId);
	}

	return m_PackageCSWords;
}

CSWordInfo CSWordTable::getCSWordInfoOnPackage(const std::string& sPackageId, const std::string& sCSWordId)
{	
	if(m_sCurrentPackageId != sPackageId)
	{
		this->fetchCSWordsForPackage(sPackageId);
		m_sCurrentPackageId = sPackageId;
	}

	for(int iIndex=0; iIndex<m_PackageCSWords.size(); iIndex++)
	{
		if(m_PackageCSWords[iIndex].sCSWordId == sCSWordId && m_PackageCSWords[iIndex].sPackageId == sPackageId)
		{
			return m_PackageCSWords[iIndex];
		}
	}

	return CSWordInfo();
}

bool CSWordTable::updateCSWord(const CSWordInfo& csWordInfo)
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from CSWords where PackageId=";
	sql.appendWithFormat("'%s'", csWordInfo.sPackageId.c_str());
	sql.appendWithFormat(" and WordId='%s'", csWordInfo.sCSWordId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	if (nRow > 0)
	{
		sql = "update Words Set";
		sql.appendWithFormat(" CollectedCount=%d", csWordInfo.iCollectedCount);
		sql.appendWithFormat(" where WordId='%s'", csWordInfo.sCSWordId.c_str());
		sql.appendWithFormat(" and PackageId='%s'", csWordInfo.sPackageId.c_str());
	}
	else
	{
		sql = "insert into Words values(";
		sql.appendWithFormat("'%s',", csWordInfo.sCSWordId.c_str());
		sql.appendWithFormat("'%s',", csWordInfo.sPackageId.c_str());
		sql.appendWithFormat("%d);", csWordInfo.iCollectedCount);
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	bool isFound = false;
	for(int iIndex=0; iIndex<m_PackageCSWords.size(); iIndex++)
	{
		if (m_PackageCSWords[iIndex].sCSWordId == csWordInfo.sCSWordId && m_PackageCSWords[iIndex].sPackageId == csWordInfo.sPackageId)
		{
			isFound = true;
			m_PackageCSWords[iIndex] = csWordInfo;
			break;
		}
	}

	if (isFound == false && csWordInfo.sPackageId == m_sCurrentPackageId)
		m_PackageCSWords.push_back(csWordInfo);

	return true;
}
