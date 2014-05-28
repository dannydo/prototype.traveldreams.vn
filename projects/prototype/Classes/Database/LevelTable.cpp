#include "LevelTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"
#include "GameConfigManager.h"
#include "WordTable.h"

USING_NS_CC; 
USING_NS_CC_EXT;

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
	m_sCurrentChapterId = "";
	return true;
}

void LevelTable::fetchLevelsForChapter(const std::string& sChapterId)
{
	std::vector<LevelInfo> tempChapterLevels;

	char **re;
	int nRow, nColumn;

	String sql = "select * from Levels where ChapterId=";
	sql.appendWithFormat("'%s' order by Level asc ", sChapterId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		LevelInfo levelInfo;
		levelInfo.iLevelId = int(strtod(re[iRow*nColumn+0], 0));
		levelInfo.sChapterId = re[iRow*nColumn+1];
		levelInfo.iLevel = int(strtod(re[iRow*nColumn+2], 0));
		levelInfo.sWordId = re[iRow*nColumn+3];
		levelInfo.iStar = int(strtod(re[iRow*nColumn+4], 0));
		levelInfo.iScore = int(strtod(re[iRow*nColumn+5], 0));
		levelInfo.iBonusQuest = int(strtod(re[iRow*nColumn+6], 0));
		levelInfo.iTotalBonusQuest = int(strtod(re[iRow*nColumn+7], 0));
		levelInfo.bIsUnlock = bool(strtod(re[iRow*nColumn+8], 0));
		levelInfo.iVersion = int(strtod(re[iRow*nColumn+9], 0));

		tempChapterLevels.push_back(levelInfo);
	}

    m_ChapterLevels = tempChapterLevels;

	sqlite3_free_table(re);
}

std::vector<LevelInfo>& LevelTable::getAllLevelsForChapter(std::string& sChapterId)
{
	if(m_sCurrentChapterId != sChapterId)
	{
		this->fetchLevelsForChapter(sChapterId);
		m_sCurrentChapterId = sChapterId;
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
	String sql = "";
	sql = "update Levels Set";
	sql.appendWithFormat(" ChapterId='%s',", levelInfo.sChapterId.c_str());
	sql.appendWithFormat(" Level=%d,", levelInfo.iLevel);
	sql.appendWithFormat(" WordId='%s',", levelInfo.sWordId.c_str());
	sql.appendWithFormat(" Star=%d,", levelInfo.iStar);
	sql.appendWithFormat(" Score=%d,", levelInfo.iScore);
	sql.appendWithFormat(" BonusQuest=%d,", levelInfo.iBonusQuest);
	sql.appendWithFormat(" TotalBonusQuest=%d,", levelInfo.iTotalBonusQuest);
	sql.appendWithFormat(" IsUnlock=%d,", levelInfo.bIsUnlock);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);
	sql.appendWithFormat(" where LevelId=%d", levelInfo.iLevelId);

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	int iIndex;
	for(iIndex=0; iIndex<m_ChapterLevels.size(); iIndex++)
	{
		if (m_ChapterLevels[iIndex].iLevel == levelInfo.iLevel && m_ChapterLevels[iIndex].sChapterId == levelInfo.sChapterId)
		{
			m_ChapterLevels[iIndex] = levelInfo;
			break;
		}
	}

	WordInfo wordInfo;
	wordInfo.sWordId = levelInfo.sWordId;
	wordInfo.sChapterId = levelInfo.sChapterId;
	wordInfo.iCountCollected = 1;
	wordInfo.iMapChapterWordId = 0;
	wordInfo.bIsCollected = 0;
	wordInfo.uTimeBeginPlayMiniGame = 0;
	WordTable::getInstance()->insertWord(wordInfo);

	return true;
}

std::string	LevelTable::syncGetLevels()
{
	char **re;
	int nRow, nColumn;
		
	String sql = "select * from Levels where Version>";
	sql.appendWithFormat("%d ", VersionTable::getInstance()->getVersionInfo().iVersionSync);
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	String sJsonData = "\"Levels\":[";
	for (int iRow=1; iRow<=nRow; iRow++)
	{
		sJsonData.append("{");
		sJsonData.appendWithFormat("\"ChapterId\": \"%s\",", re[iRow*nColumn+1]);
		sJsonData.appendWithFormat("\"Level\": %s,", re[iRow*nColumn+2]);
		sJsonData.appendWithFormat("\"WordId\": \"%s\",", re[iRow*nColumn+3]);
		sJsonData.appendWithFormat("\"Star\": %s,", re[iRow*nColumn+4]);
		sJsonData.appendWithFormat("\"Score\": %s,", re[iRow*nColumn+5]);
		sJsonData.appendWithFormat("\"BonusQuest\": %s,", re[iRow*nColumn+6]);
		sJsonData.appendWithFormat("\"TotalBonusQuest\": %s,", re[iRow*nColumn+7]);
		sJsonData.appendWithFormat("\"IsUnlock\": %s,", re[iRow*nColumn+8]);
		sJsonData.appendWithFormat("\"Version\": %s", re[iRow*nColumn+9]);

		if (iRow == nRow)
			sJsonData.append("}");
		else
			sJsonData.append("},");
	}
	sJsonData.append("]");
	sqlite3_free_table(re);

	return sJsonData.getCString();
}

bool LevelTable::updateDataSyncLevels(cs::JsonDictionary* pJsonSync, const int& iVersion)
{
	int iIndexCurrentChapter = -1;
	int iCurrentLevel = 1;
	cs::JsonDictionary *pJsonUser = pJsonSync->getSubDictionary("User");
	WordlMapConfig worldMapConfig = GameConfigManager::getInstance()->GetWordlMapConfig();
	if (pJsonUser != NULL)
	{
		WordlMapConfig::WordMapChapterConfig* wordMapChapterConfig = GameConfigManager::getInstance()->GetWordMapChapterConfig(pJsonUser->getItemStringValue("CurrentChapter"));
		if (wordMapChapterConfig != NULL)
		{
			iIndexCurrentChapter = worldMapConfig.m_WorlMapChapterConfigMap[pJsonUser->getItemStringValue("CurrentChapter")];
			iCurrentLevel = pJsonUser->getItemIntValue("CurrentLevel", 1);
		}
	}

	String sqlRun = "";
	std::vector<LevelInfo> levels;

	if (pJsonSync->getArrayItemCount("Levels") > 0)
	{
		char **re;
		int nRow, nColumn;
		String sql = "select * from Levels";
		sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

		for(int iRow=1; iRow<=nRow; iRow++)
		{
			LevelInfo levelInfo;
			levelInfo.sChapterId = re[iRow*nColumn+1];
			levelInfo.iLevel = int(strtod(re[iRow*nColumn+2], 0)); 

			levels.push_back(levelInfo);
		}

		sqlite3_free_table(re);
	}

	for(int iIndex=0; iIndex<pJsonSync->getArrayItemCount("Levels"); iIndex++)
	{
		cs::JsonDictionary* pJsonLevel = pJsonSync->getSubItemFromArray("Levels", iIndex);
		bool isInsert = true;

		std::string sChapterId = pJsonLevel->getItemStringValue("ChapterId");
		int iLevel = pJsonLevel->getItemIntValue("Level", 1);
		for(int iIndexLevel=0; iIndexLevel<levels.size(); iIndexLevel++)
		{
			if (sChapterId == levels[iIndexLevel].sChapterId && iLevel == levels[iIndexLevel].iLevel)
			{
				isInsert = false;
				break;
			}
		}

		int iIndexChapter = worldMapConfig.m_WorlMapChapterConfigMap[sChapterId];
		int isUnlock = 1;
		if (iIndexCurrentChapter != -1)
		{
			if(iIndexChapter > iIndexCurrentChapter)
				isUnlock = 0;
			else if (iIndexChapter == iIndexCurrentChapter && iCurrentLevel <= iLevel)
				isUnlock = 0;
		}

		if (isInsert)
		{
			// Insert Level
			sqlRun.append("insert into Levels (ChapterId,Level,WordId,Star,Score,BonusQuest,TotalBonusQuest,IsUnlock,Version) values(");
			sqlRun.appendWithFormat("'%s',", pJsonLevel->getItemStringValue("ChapterId"));
			sqlRun.appendWithFormat("%d,", pJsonLevel->getItemIntValue("Level", 1));
			sqlRun.appendWithFormat("'%s',", pJsonLevel->getItemStringValue("WordId"));
			sqlRun.appendWithFormat("%d,", pJsonLevel->getItemIntValue("Stars", 0));
			sqlRun.appendWithFormat("%d,", pJsonLevel->getItemIntValue("Score", 0));
			sqlRun.appendWithFormat("%d,", pJsonLevel->getItemIntValue("BonusQuest", 0));
			sqlRun.appendWithFormat("%d,", pJsonLevel->getItemIntValue("TotalBonusQuest", 0));
			sqlRun.appendWithFormat("%d,", isUnlock);
			sqlRun.appendWithFormat("%d);", iVersion);
		}
		else
		{
			// Update Level
			sqlRun.append("update Levels Set");
			sqlRun.appendWithFormat(" WordId='%s',", pJsonLevel->getItemStringValue("WordId"));
			sqlRun.appendWithFormat(" Star=%d,", pJsonLevel->getItemIntValue("Stars", 0));
			sqlRun.appendWithFormat(" Score=%d,", pJsonLevel->getItemIntValue("Score", 0));
			sqlRun.appendWithFormat(" BonusQuest=%d,", pJsonLevel->getItemIntValue("BonusQuest", 0));
			sqlRun.appendWithFormat(" TotalBonusQuest=%d,", pJsonLevel->getItemIntValue("TotalBonusQuest", 0));
			sqlRun.appendWithFormat(" IsUnlock=%d,", isUnlock);
			sqlRun.appendWithFormat(" Version=%d", iVersion);
			sqlRun.appendWithFormat(" where ChapterId='%s'", pJsonLevel->getItemStringValue("ChapterId"));
			sqlRun.appendWithFormat(" and Level=%d;", pJsonLevel->getItemIntValue("Level", 0));
		}
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sqlRun.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	CCLOG("sync Level true");
    if (pJsonSync->getArrayItemCount("Levels") > 0)
    {
	    if (m_sCurrentChapterId != "")
		    this->fetchLevelsForChapter(m_sCurrentChapterId);
    }

	return true;
}
