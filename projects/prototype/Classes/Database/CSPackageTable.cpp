#include "CSPackageTable.h"
#include "InitDatabase.h"

USING_NS_CC; 
USING_NS_CC_EXT;

void CSPackageTable::getAllCSPackages(std::vector<CSPackageInfo>& csPackages)
{
	char **re;
	int nRow, nColumn;

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), "select * from CSPackage", &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		CSPackageInfo csPackageInfo;
		csPackageInfo.sPackageId = re[iRow*nColumn+0];
		csPackageInfo.sPackageName = re[iRow*nColumn+1];
		csPackageInfo.iStage = int(strtod(re[iRow*nColumn+2], 0));
		csPackageInfo.sPackageCode = re[iRow*nColumn+3];
		csPackageInfo.sCreatedBy = re[iRow*nColumn+4];
		csPackageInfo.iTotalWordUnlock = int(strtod(re[iRow*nColumn+5], 0));
		csPackageInfo.iTotalWord = int(strtod(re[iRow*nColumn+6], 0));

		csPackages.push_back(csPackageInfo);
	}

	sqlite3_free_table(re);
}

CSPackageInfo CSPackageTable::getCSPackageInfo(const std::string& sPackageId)
{	
	std::vector<CSPackageInfo> csPackages;
	char **re;
	int nRow, nColumn;

	String sql = "select * from CSPackage where PackageId=";
	sql.appendWithFormat("'%s'",sPackageId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	if (nRow > 0)
	{
		CSPackageInfo csPackageInfo;
		csPackageInfo.sPackageId = re[nColumn+0];
		csPackageInfo.sPackageName = re[nColumn+1];
		csPackageInfo.iStage = int(strtod(re[nColumn+2], 0));
		csPackageInfo.sPackageCode = re[nColumn+3];
		csPackageInfo.sCreatedBy = re[nColumn+4];
		csPackageInfo.iTotalWordUnlock = int(strtod(re[nColumn+5], 0));
		csPackageInfo.iTotalWord = int(strtod(re[nColumn+6], 0));

		sqlite3_free_table(re);
		return csPackageInfo;
	}

	sqlite3_free_table(re);
	return CSPackageInfo();
}

bool CSPackageTable::updateCSPackage(const CSPackageInfo& csPackageInfo)
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from CSPackage where PackageId=";
	sql.appendWithFormat("'%s'", csPackageInfo.sPackageId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	int iStage = csPackageInfo.iStage;
	if (nRow > 0)
	{
		CSPackageInfo csPackageInfoTemp;
		csPackageInfoTemp.sPackageId = re[nColumn+0];
		csPackageInfoTemp.sPackageName = re[nColumn+1];
		csPackageInfoTemp.iStage = int(strtod(re[nColumn+2], 0));

		if (csPackageInfoTemp.iStage > iStage)
			iStage = csPackageInfoTemp.iStage;

		sql = "update CSPackage Set";
		sql.appendWithFormat(" PackageName='%s',", csPackageInfo.sPackageName.c_str());
		sql.appendWithFormat(" Stage=%d,", iStage);
		sql.appendWithFormat(" PackageCode='%s',", csPackageInfo.sPackageCode.c_str());
		sql.appendWithFormat(" CreatedBy='%s',", csPackageInfo.sCreatedBy.c_str());
		sql.appendWithFormat(" TotalWordUnlock=%d,", csPackageInfo.iTotalWordUnlock);
		sql.appendWithFormat(" TotalWord=%d", csPackageInfo.iTotalWord);
		sql.appendWithFormat(" where PackageId='%s'", csPackageInfo.sPackageId.c_str());
	}
	else
	{
		sql = "insert into CSPackage values(";
		sql.appendWithFormat("'%s',", csPackageInfo.sPackageId.c_str());
		sql.appendWithFormat("'%s',", csPackageInfo.sPackageName.c_str());
		sql.appendWithFormat("%d,", csPackageInfo.iStage);
		sql.appendWithFormat("'%s',", csPackageInfo.sPackageCode.c_str());
		sql.appendWithFormat("'%s',", csPackageInfo.sCreatedBy.c_str());
		sql.appendWithFormat("%d,", csPackageInfo.iTotalWordUnlock);
		sql.appendWithFormat("%d);", csPackageInfo.iTotalWord);
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}