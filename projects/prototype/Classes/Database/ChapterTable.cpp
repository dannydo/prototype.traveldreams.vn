#include "ChapterTable.h" 
#include "InitDatabase.h"

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

void ChapterTable::fetchAllChapter()
{
	char **re;
	int nRow, nColumn;
		
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), "select * from Chapters", &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		ChapterInfo chapterInfo;
		chapterInfo.iChapter = int(strtod(re[iRow*nColumn+0], NULL));
		chapterInfo.sName = re[iRow*nColumn+1];
		chapterInfo.iMinLevel = int(strtod(re[iRow*nColumn+2], NULL));
		chapterInfo.iMaxLevel = int(strtod(re[iRow*nColumn+3], NULL));
		chapterInfo.iTotalLevelUnlock = int(strtod(re[iRow*nColumn+4], NULL));
		chapterInfo.iTotalStar = int(strtod(re[iRow*nColumn+5], NULL));
		chapterInfo.iTotalScore = int(strtod(re[iRow*nColumn+6], NULL));
		chapterInfo.bIsUnlock = bool(strtod(re[iRow*nColumn+7], NULL));
		chapterInfo.bIsUpdate = bool(strtod(re[iRow*nColumn+8], NULL));

		m_Chapters.push_back(chapterInfo);
	}

	sqlite3_free_table(re);
}

bool ChapterTable::updateChapter(ChapterInfo chapterInfo)
{
	String sql = "update Chapters Set";
	sql.appendWithFormat(" Name='%s',", chapterInfo.sName.c_str());
	sql.appendWithFormat(" MinLevel=%d,", chapterInfo.iMinLevel);
	sql.appendWithFormat(" MaxLevel=%d,", chapterInfo.iMaxLevel);
	sql.appendWithFormat(" TotalLevelUnlock=%d,", chapterInfo.iTotalLevelUnlock);
	sql.appendWithFormat(" TotalStar=%d,", chapterInfo.iTotalStar);
	sql.appendWithFormat(" TotalScore=%d,", chapterInfo.iTotalScore);
	sql.appendWithFormat(" IsUnlock=%d,", chapterInfo.bIsUnlock);
	sql.appendWithFormat(" IsUpdate=%d", chapterInfo.bIsUpdate);
	sql.appendWithFormat(" where Chapter=%d", chapterInfo.iChapter);

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	for(int iIndex=0; iIndex<m_Chapters.size(); iIndex++)
	{
		if(m_Chapters[iIndex].iChapter == chapterInfo.iChapter)
		{
			m_Chapters[iIndex] = chapterInfo;
		}
	}

	return true;
}