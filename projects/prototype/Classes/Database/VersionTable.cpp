#include "VersionTable.h"
#include "InitDatabase.h"

USING_NS_CC; 

VersionTable* VersionTable::m_VersionTable = NULL;

VersionTable::VersionTable()
{

}

void VersionTable::releaseInstance()
{
	if (m_VersionTable == NULL)
	{
		delete m_VersionTable;
	}
	m_VersionTable = NULL;	
}


VersionTable* VersionTable::getInstance()
{
	if (m_VersionTable == NULL) {
		m_VersionTable = new VersionTable();
		m_VersionTable->init();
	}

	return m_VersionTable;
}

bool VersionTable::init()
{
	fetchVersion();
	return true;
}

void VersionTable::fetchVersion()
{
	char **re;
	int nRow, nColumn;
		
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), "select * from Versions  limit 1", &re, &nRow, &nColumn,NULL);

	if (nRow > 0)
	{
		m_VersionInfo.iVersionId = int(strtod(re[nColumn+0], 0));
		m_VersionInfo.iVersionSync = int(strtod(re[nColumn+1], 0));
		m_VersionInfo.iVersionDatabase = int(strtod(re[nColumn+2], 0));
	}

	sqlite3_free_table(re);
}

bool VersionTable::updateVersion(const VersionInfo& versionInfo)
{
	String sql = "update Versions Set";
	sql.appendWithFormat(" VersionSync=%d,", versionInfo.iVersionSync);
	sql.appendWithFormat(" VersionDatabase=%d", versionInfo.iVersionDatabase);
	sql.appendWithFormat(" where VersionId=%d", versionInfo.iVersionId);

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	m_VersionInfo = versionInfo;

	this->fetchVersion();

	return true;
}

void VersionTable::refreshVersionInfo()
{
	this->fetchVersion();
}