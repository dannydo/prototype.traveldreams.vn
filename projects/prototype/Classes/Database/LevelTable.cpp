#include "LevelTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"

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

void LevelTable::fetchLevelsForChapter(const std::string& sChapterId)
{
	while(!m_ChapterLevels.empty())
	{
		m_ChapterLevels.pop_back();
	}

	char **re;
	int nRow, nColumn;

	String sql = "select * from Levels where ChapterId=";
	sql.appendWithFormat("'%s'", sChapterId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		LevelInfo levelInfo;
		levelInfo.iLevelId = int(strtod(re[iRow*nColumn+0], NULL));
		levelInfo.sChapterId = re[iRow*nColumn+1];
		levelInfo.iLevel = int(strtod(re[iRow*nColumn+2], NULL));
		levelInfo.sWordId = re[iRow*nColumn+3];
		levelInfo.iStar = int(strtod(re[iRow*nColumn+4], NULL));
		levelInfo.iScore = int(strtod(re[iRow*nColumn+5], NULL));
		levelInfo.iBonusQuest = int(strtod(re[iRow*nColumn+6], NULL));
		levelInfo.iTotalBonusQuest = int(strtod(re[iRow*nColumn+7], NULL));
		levelInfo.bIsUnlock = bool(strtod(re[iRow*nColumn+8], NULL));
		levelInfo.iVersion = int(strtod(re[iRow*nColumn+9], NULL));

		m_ChapterLevels.push_back(levelInfo);
	}

	sqlite3_free_table(re);
}

std::vector<LevelInfo> LevelTable::getAllLevelsForChapter(std::string& sChapterId)
{
	if(m_sCurrentChapterId != sChapterId)
	{
		this->fetchLevelsForChapter(sChapterId);
	}

	return m_ChapterLevels;
}

LevelInfo& LevelTable::getLevel(const std::string& sChapterId, const int& iLevel)
{	
	if(m_sCurrentChapterId != sChapterId)
	{
		this->fetchLevelsForChapter(sChapterId);
		m_sCurrentChapterId = sChapterId;
	}

	return m_ChapterLevels[iLevel-1];//-1 cause levelID start from 1
}

bool LevelTable::updateLevel(const LevelInfo& levelInfo)
{
	String sql = "update Levels Set";
	sql.appendWithFormat(" ChapterId='%s',", levelInfo.sChapterId.c_str());
	sql.appendWithFormat(" Level=%d,", levelInfo.iLevel);
	sql.appendWithFormat(" WordId='%s',", levelInfo.sWordId.c_str());
	sql.appendWithFormat(" Star=%d,", levelInfo.iStar);
	sql.appendWithFormat(" Score=%d,", levelInfo.iScore);
	sql.appendWithFormat(" BonusQuest=%d,", levelInfo.iBonusQuest);
	sql.appendWithFormat(" TotalBonusQuest=%d,", levelInfo.iTotalBonusQuest);
	sql.appendWithFormat(" IsUnlock=%d,", levelInfo.bIsUnlock);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionId + 1);
	sql.appendWithFormat(" where LevelId=%d", levelInfo.iLevelId);

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}