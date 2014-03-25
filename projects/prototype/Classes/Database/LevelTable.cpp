#include "LevelTable.h"
#include "InitDatabase.h"

USING_NS_CC; 

LevelTable* LevelTable::m_LevelTable = NULL;

LevelTable::LevelTable()
{

}

void LevelTable::releaseInstance()
{
	if (m_LevelTable == NULL)
	{
		delete m_LevelTable;
	}
	m_LevelTable = NULL;	
}

LevelTable* LevelTable::getInstance()
{
	if (m_LevelTable == NULL) {
		m_LevelTable = new LevelTable();
		m_LevelTable->init();
	}

	return m_LevelTable;
}

bool LevelTable::init()
{
	return true;
}

std::vector<LevelInfo> LevelTable::fetchLevelsForChapter(const int& iChapter)
{
	std::vector<LevelInfo> levels;
	char **re;
	int nRow, nColumn;

	String sql = "select * from Levels where Chapter=";
	sql.appendWithFormat("%d", iChapter);

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		LevelInfo levelInfo;
		levelInfo.iLevel = int(strtod(re[iRow*nColumn+0], NULL));
		levelInfo.iChapter = int(strtod(re[iRow*nColumn+1], NULL));
		levelInfo.iStar = int(strtod(re[iRow*nColumn+2], NULL));
		levelInfo.iScore = int(strtod(re[iRow*nColumn+3], NULL));
		levelInfo.iBonusQuest = int(strtod(re[iRow*nColumn+4], NULL));
		levelInfo.bIsUnlock = bool(strtod(re[iRow*nColumn+5], NULL));
		levelInfo.bIsUpdate = bool(strtod(re[iRow*nColumn+6], NULL));
		levelInfo.sWordKey = re[iRow*nColumn+7];
		levelInfo.iTotalBonusQuest = int(strtod(re[iRow*nColumn+8], NULL));

		levels.push_back(levelInfo);
	}

	sqlite3_free_table(re);

	return levels;
}

LevelInfo LevelTable::fetchLevel(const int& iLevel)
{	
	char **re;
	int nRow, nColumn;

	String sql = "select * from Levels where Level=";
	sql.appendWithFormat("%d", iLevel);

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	LevelInfo levelInfo;
	if (nRow > 0)
	{
		levelInfo.iLevel = int(strtod(re[nColumn+0], NULL));
		levelInfo.iChapter = int(strtod(re[nColumn+1], NULL));
		levelInfo.iStar = int(strtod(re[nColumn+2], NULL));
		levelInfo.iScore = int(strtod(re[nColumn+3], NULL));
		levelInfo.iBonusQuest = int(strtod(re[nColumn+4], NULL));
		levelInfo.bIsUnlock = bool(strtod(re[nColumn+5], NULL));
		levelInfo.bIsUpdate = bool(strtod(re[nColumn+6], NULL));
		levelInfo.sWordKey = re[nColumn+7];
		levelInfo.iTotalBonusQuest = int(strtod(re[nColumn+8], NULL));
	}

	sqlite3_free_table(re);

	return levelInfo;
}

bool LevelTable::updateLevel(LevelInfo levelInfo)
{
	String sql = "update Levels Set";
	sql.appendWithFormat(" Chapter=%d,", levelInfo.iChapter);
	sql.appendWithFormat(" Star=%d,", levelInfo.iStar);
	sql.appendWithFormat(" Score=%d,", levelInfo.iScore);
	sql.appendWithFormat(" BonusQuest=%d,", levelInfo.iBonusQuest);
	sql.appendWithFormat(" TotalBonusQuest=%d,", levelInfo.iTotalBonusQuest);
	sql.appendWithFormat(" IsUnlock=%d,", levelInfo.bIsUnlock);
	sql.appendWithFormat(" IsUpdate=%d,", levelInfo.bIsUpdate);
	sql.appendWithFormat(" WordKey='%s'", levelInfo.sWordKey.c_str());
	sql.appendWithFormat(" where Level=%d", levelInfo.iLevel);

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}