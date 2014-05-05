#include "ChapterTable.h" 
#include "InitDatabase.h"
#include "VersionTable.h"

USING_NS_CC; 
USING_NS_CC_EXT;

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

void ChapterTable::refreshChapters()
{
	this->fetchAllChapter();
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

	return ChapterInfo();
}

void ChapterTable::fetchAllChapter()
{
	while(!m_Chapters.empty())
	{
		m_Chapters.pop_back();
	}

	char **re;
	int nRow, nColumn;
		
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), "select * from Chapters", &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		ChapterInfo chapterInfo;
		chapterInfo.sChapterId = re[iRow*nColumn+0];
		chapterInfo.iTotalLevelUnlock = int(strtod(re[iRow*nColumn+1], 0));
		chapterInfo.iTotalStar = int(strtod(re[iRow*nColumn+2], 0));
		chapterInfo.bIsUnlock = bool(strtod(re[iRow*nColumn+3], 0));
		chapterInfo.iVersion = int(strtod(re[iRow*nColumn+4], 0));
		chapterInfo.iTotalFlashCardUnlock = int(strtod(re[iRow*nColumn+5], 0));
		chapterInfo.iTotalFlashCard = int(strtod(re[iRow*nColumn+6], 0));

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
	sql.appendWithFormat(" Version=%d,", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);
	sql.appendWithFormat(" TotalFlashCardUnlock=%d,", chapterInfo.iTotalFlashCardUnlock);
	sql.appendWithFormat(" TotalFlashCard=%d", chapterInfo.iTotalFlashCard);
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

std::string	ChapterTable::syncGetChapters()
{
	char **re;
	int nRow, nColumn;
		
	String sql = "select * from Chapters where Version>";
	sql.appendWithFormat("%d ", VersionTable::getInstance()->getVersionInfo().iVersionSync);
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	String sJsonData = "\"Chapters\":[";
	for (int iRow=1; iRow<=nRow; iRow++)
	{
		sJsonData.append("{");
		sJsonData.appendWithFormat("\"ChapterId\": \"%s\",", re[iRow*nColumn+0]);
		sJsonData.appendWithFormat("\"TotalLevelUnlock\": %s,", re[iRow*nColumn+1]);
		sJsonData.appendWithFormat("\"TotalStar\": %s,", re[iRow*nColumn+2]);
		sJsonData.appendWithFormat("\"IsUnlock\": %s,", re[iRow*nColumn+3]);
		sJsonData.appendWithFormat("\"Version\": %s,", re[iRow*nColumn+4]);
		sJsonData.appendWithFormat("\"TotalFlashCardUnlock\": %s,", re[iRow*nColumn+5]);
		sJsonData.appendWithFormat("\"TotalFlashCard\": %s", re[iRow*nColumn+6]);

		if (iRow == nRow)
			sJsonData.append("}");
		else
			sJsonData.append("},");
	}
	sJsonData.append("]");
	sqlite3_free_table(re);

	return sJsonData.getCString();
}

bool ChapterTable::updateDataSyncChapters(cs::JsonDictionary* pJsonSync, const int& iVersion)
{
	String sqlRun = "";
	std::vector<ChapterInfo> chapters;

	if (pJsonSync->getArrayItemCount("Chapters") > 0)
	{
		char **re;
		int nRow, nColumn;
		String sql = "select * from Chapters";
		sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

		for(int iRow=1; iRow<=nRow; iRow++)
		{
			ChapterInfo chapterInfo;
			chapterInfo.sChapterId = re[iRow*nColumn+0];

			chapters.push_back(chapterInfo);
		}

		sqlite3_free_table(re);
	}

	for(int iIndex=0; iIndex<pJsonSync->getArrayItemCount("Chapters"); iIndex++)
	{
		cs::JsonDictionary* pJsonChapter = pJsonSync->getSubItemFromArray("Chapters", iIndex);
		bool isInsert = true;

		std::string sChapterId = pJsonChapter->getItemStringValue("ChapterId");
		for(int iIndexChapter=0; iIndexChapter<chapters.size(); iIndexChapter++)
		{
			if (sChapterId == chapters[iIndexChapter].sChapterId)
			{
				isInsert = false;
				break;
			}
		}

		if (isInsert)
		{
			// Insert Chapter
			sqlRun.append("insert into Chapters values(");
			sqlRun.appendWithFormat("'%s',", pJsonChapter->getItemStringValue("ChapterId"));
			sqlRun.appendWithFormat("%s,", pJsonChapter->getItemStringValue("TotalLevelUnlock"));
			sqlRun.appendWithFormat("%s,", pJsonChapter->getItemStringValue("TotalStar"));
			sqlRun.appendWithFormat("%d,", 1);//pJsonChapter->getItemIntValue("IsUnlock", 0));
			sqlRun.appendWithFormat("%d,", iVersion);
			sqlRun.appendWithFormat("%s,", pJsonChapter->getItemStringValue("TotalFlashCardUnlock"));
			sqlRun.appendWithFormat("%s);", pJsonChapter->getItemStringValue("TotalFlashCard"));
		}
		else
		{
			// Update Chapter
			sqlRun.append("update Chapters Set");
			sqlRun.appendWithFormat(" TotalLevelUnlock=%s,", pJsonChapter->getItemStringValue("TotalLevelUnlock"));
			sqlRun.appendWithFormat(" TotalStar=%s,", pJsonChapter->getItemStringValue("TotalStar"));
			sqlRun.appendWithFormat(" IsUnlock=%d,", 1);//pJsonChapter->getItemIntValue("IsUnlock", 0));
			sqlRun.appendWithFormat(" Version=%d,", iVersion);
			sqlRun.appendWithFormat(" TotalFlashCardUnlock=%s,", pJsonChapter->getItemStringValue("TotalFlashCardUnlock"));
			sqlRun.appendWithFormat(" TotalFlashCard=%s", pJsonChapter->getItemStringValue("TotalFlashCard"));
			sqlRun.appendWithFormat(" where ChapterId='%s';", pJsonChapter->getItemStringValue("ChapterId"));
		}
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sqlRun.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	this->fetchAllChapter();

	return true;
}