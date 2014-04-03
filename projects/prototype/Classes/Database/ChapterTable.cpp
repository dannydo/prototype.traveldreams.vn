#include "ChapterTable.h" 
#include "InitDatabase.h"
#include "VersionTable.h"

USING_NS_CC; 

ChapterTable* ChapterTable::m_ChapterTable = NULL;

ChapterTable::ChapterTable()
{

}

void ChapterTable::releaseInstance()
{
	if (m_ChapterTable == NULL)
	{
		delete m_ChapterTable;
	}
	m_ChapterTable = NULL;	
}

ChapterTable* ChapterTable::getInstance()
{
	if (m_ChapterTable == NULL) {
		m_ChapterTable = new ChapterTable();
		m_ChapterTable->init();
	}

	return m_ChapterTable;
}

bool ChapterTable::init()
{
	fetchAllChapter();
	return true;
}

ChapterInfo ChapterTable::getChapterInfo(const std::string sChapterId)
{
	for(int iIndex=0; iIndex<m_Chapters.size(); iIndex++)
	{
		if(m_Chapters[iIndex].sChapterId == sChapterId)
		{
			return m_Chapters[iIndex];
		}
	}

	return ChapterInfo::ChapterInfo();
}

void ChapterTable::fetchAllChapter()
{
	char **re;
	int nRow, nColumn;
		
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), "select * from Chapters", &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		ChapterInfo chapterInfo;
		chapterInfo.sChapterId = re[iRow*nColumn+0];
		chapterInfo.iTotalLevelUnlock = int(strtod(re[iRow*nColumn+1], NULL));
		chapterInfo.iTotalStar = int(strtod(re[iRow*nColumn+2], NULL));
		chapterInfo.bIsUnlock = bool(strtod(re[iRow*nColumn+3], NULL));
		chapterInfo.iVersion = int(strtod(re[iRow*nColumn+4], NULL));

		m_Chapters.push_back(chapterInfo);
	}

	sqlite3_free_table(re);
}

bool ChapterTable::updateChapter(ChapterInfo chapterInfo)
{
	String sql = "update Chapters Set";
	sql.appendWithFormat(" TotalLevelUnlock=%d,", chapterInfo.iTotalLevelUnlock);
	sql.appendWithFormat(" TotalStar=%d,", chapterInfo.iTotalStar);
	sql.appendWithFormat(" IsUnlock=%d,", chapterInfo.bIsUnlock);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionId + 1);
	sql.appendWithFormat(" where ChapterId='%s'", chapterInfo.sChapterId.c_str());

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	for(int iIndex=0; iIndex<m_Chapters.size(); iIndex++)
	{
		if(m_Chapters[iIndex].sChapterId == chapterInfo.sChapterId)
		{
			m_Chapters[iIndex] = chapterInfo;
		}
	}

	return true;
}