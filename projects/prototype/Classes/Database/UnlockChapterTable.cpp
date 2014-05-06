#include "UnlockChapterTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"
#include "FunctionCommon.h"

USING_NS_CC; 

UnlockChapterTable* UnlockChapterTable::m_UnlockChapterTable = NULL;

UnlockChapterTable::UnlockChapterTable()
{

}

void UnlockChapterTable::releaseInstance()
{
	if (m_UnlockChapterTable == NULL)
	{
		delete m_UnlockChapterTable;
	}
	m_UnlockChapterTable = NULL;	
}


UnlockChapterTable* UnlockChapterTable::getInstance()
{
	if (m_UnlockChapterTable == NULL) {
		m_UnlockChapterTable = new UnlockChapterTable();
		m_UnlockChapterTable->init();
	}

	return m_UnlockChapterTable;
}

bool UnlockChapterTable::init()
{
	return true;
}

UnlockChapterInfo UnlockChapterTable::fetchUnlockChapter(const std::string sChapterId, const std::string sType)
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from UnlockChapters where ChapterId=";
	sql.appendWithFormat("'%s'", sChapterId.c_str());
	sql.appendWithFormat(" and Type='%s'", sType.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	UnlockChapterInfo unlockChapterInfo;
	if (nRow > 0)
	{
		unlockChapterInfo.iUnlockChapterId = int(strtod(re[nColumn+0], 0));
		unlockChapterInfo.sChapterId = re[nColumn+1];
		unlockChapterInfo.iRequest = int(strtod(re[nColumn+2], 0));
		unlockChapterInfo.sType = re[nColumn+3];
		unlockChapterInfo.uBeginTime = long(strtod(re[nColumn+4], 0));
		unlockChapterInfo.iVersion = int(strtod(re[nColumn+5], 0));
	}

	sqlite3_free_table(re);

	return unlockChapterInfo;
}

bool UnlockChapterTable::updateUnlockChapter(const UnlockChapterInfo& unlockChapterInfo)
{
	String sql = "update UnlockChapters Set";
	sql.appendWithFormat(" Request=%d,", unlockChapterInfo.iRequest);
	sql.appendWithFormat(" BeginTime=%s,", getTimeLocalCurrent());
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);
	sql.appendWithFormat(" where UnlockChapterId=%d", unlockChapterInfo.iUnlockChapterId);

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}

bool UnlockChapterTable::insertUnlockChapter(const UnlockChapterInfo& unlockChapterInfo)
{
	String sql = "insert into UnlockChapters (ChapterId,Request,Type,BeginTime,Version) values(";
	sql.appendWithFormat(" '%s',", unlockChapterInfo.sChapterId.c_str());
	sql.appendWithFormat(" %d,", unlockChapterInfo.iRequest);
	sql.appendWithFormat(" '%s',", unlockChapterInfo.sType.c_str());
	sql.appendWithFormat(" %u,", getTimeLocalCurrent());
	sql.appendWithFormat(" %d)", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}

std::string	UnlockChapterTable::syncGetUnlockChapters()
{
	char **re;
	int nRow, nColumn;
		
	String sql = "select * from UnlockChapters where Version>";
	sql.appendWithFormat("%d ", VersionTable::getInstance()->getVersionInfo().iVersionSync);
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	String sJsonData = "\"UnlockChapters\":[";
	for (int iRow=1; iRow<=nRow; iRow++)
	{
		sJsonData.append("{");
		sJsonData.appendWithFormat("\"ChapterId\": \"%s\",", re[iRow*nColumn+1]);
		sJsonData.appendWithFormat("\"Request\": %s,", re[iRow*nColumn+2]);
		sJsonData.appendWithFormat("\"Type\": \"%s\",", re[iRow*nColumn+3]);
		sJsonData.appendWithFormat("\"BeginTime\": %s,", re[iRow*nColumn+4]);
		sJsonData.appendWithFormat("\"Version\": %s", re[iRow*nColumn+5]);

		if (iRow == nRow)
			sJsonData.append("}");
		else
			sJsonData.append("},");
	}
	sJsonData.append("]");
	sqlite3_free_table(re);

	return sJsonData.getCString();
}