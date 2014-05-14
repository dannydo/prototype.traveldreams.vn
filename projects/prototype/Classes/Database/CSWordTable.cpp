#include "CSWordTable.h"
#include "InitDatabase.h"

USING_NS_CC; 
USING_NS_CC_EXT;

void CSWordTable::getAllCSWordsForPackage(std::vector<CSWordInfo>& packageCSWords, const std::string& sPackageId)
{
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

		packageCSWords.push_back(csWordInfo);
	}

	sqlite3_free_table(re);
}

CSWordInfo CSWordTable::getCSWordInfoOnPackage(const std::string& sPackageId, const std::string& sCSWordId)
{	
	char **re;
	int nRow, nColumn;

	String sql = "select * from CSWords where PackageId=";
	sql.appendWithFormat("'%s'", sPackageId.c_str());
	sql.appendWithFormat(" and WordId='%s'", sCSWordId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	if (nRow > 0)
	{
		CSWordInfo csWordInfo;
		csWordInfo.sCSWordId = re[nColumn+0];
		csWordInfo.sPackageId = re[nColumn+1];
		csWordInfo.iCollectedCount = int(strtod(re[nColumn+2], 0));

		sqlite3_free_table(re);
		return csWordInfo;
	}

	sqlite3_free_table(re);
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
		sql = "update CSWords Set";
		sql.appendWithFormat(" CollectedCount=%d", csWordInfo.iCollectedCount);
		sql.appendWithFormat(" where WordId='%s'", csWordInfo.sCSWordId.c_str());
		sql.appendWithFormat(" and PackageId='%s'", csWordInfo.sPackageId.c_str());
	}
	else
	{
		sql = "insert into CSWords values(";
		sql.appendWithFormat("'%s',", csWordInfo.sCSWordId.c_str());
		sql.appendWithFormat("'%s',", csWordInfo.sPackageId.c_str());
		sql.appendWithFormat("%d);", csWordInfo.iCollectedCount);
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}
