#include "CSPackageTable.h"
#include "InitDatabase.h"

USING_NS_CC; 
USING_NS_CC_EXT;

CSPackageTable* CSPackageTable::m_CSPackageTable = NULL;

CSPackageTable::CSPackageTable()
{

}

void CSPackageTable::releaseInstance()
{
	if (m_CSPackageTable == NULL)
	{
		delete m_CSPackageTable;
	}
	m_CSPackageTable = NULL;	
}

CSPackageTable* CSPackageTable::getInstance()
{
	if (m_CSPackageTable == NULL) {
		m_CSPackageTable = new CSPackageTable();
		m_CSPackageTable->init();
	}

	return m_CSPackageTable;
}

bool CSPackageTable::init()
{
	this->fetchCSPackages();
	return true;
}

void CSPackageTable::fetchCSPackages()
{
	while(!m_CSPackages.empty())
	{
		m_CSPackages.pop_back();
	}

	char **re;
	int nRow, nColumn;

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), "select * from CSPackage", &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		CSPackageInfo csPackageInfo;
		csPackageInfo.sPackageId = re[iRow*nColumn+0];
		csPackageInfo.sPackageName = re[iRow*nColumn+1];

		m_CSPackages.push_back(csPackageInfo);
	}

	sqlite3_free_table(re);
}

CSPackageInfo CSPackageTable::getCSPackageInfo(const std::string& sPackageId)
{	
	for(int iIndex=0; iIndex<m_CSPackages.size(); iIndex++)
	{
		if(m_CSPackages[iIndex].sPackageId == sPackageId)
		{
			return m_CSPackages[iIndex];
		}
	}

	return CSPackageInfo();
}

bool CSPackageTable::updateCSPackage(const CSPackageInfo& csPackageInfo)
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from CSPackage where PackageId=";
	sql.appendWithFormat("'%s'", csPackageInfo.sPackageId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	if (nRow > 0)
	{
		sql = "update CSPackage Set";
		sql.appendWithFormat(" PackageName=%s", csPackageInfo.sPackageName.c_str());
		sql.appendWithFormat(" where PackageId='%s'", csPackageInfo.sPackageId.c_str());
	}
	else
	{
		sql = "insert into CSPackage values(";
		sql.appendWithFormat("'%s',", csPackageInfo.sPackageId.c_str());
		sql.appendWithFormat("'%s');", csPackageInfo.sPackageName.c_str());
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	bool isFound = false;
	for(int iIndex=0; iIndex<m_CSPackages.size(); iIndex++)
	{
		if (m_CSPackages[iIndex].sPackageId == csPackageInfo.sPackageId)
		{
			isFound = true;
			m_CSPackages[iIndex] = csPackageInfo;
			break;
		}
	}

	if (isFound == false)
		m_CSPackages.push_back(csPackageInfo);

	return true;
}