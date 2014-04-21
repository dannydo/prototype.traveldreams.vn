#include "LevelTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"
#include "GameConfigManager.h"

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
	while(!m_ChapterLevels.empty())
	{
		m_ChapterLevels.pop_back();
	}

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

		m_ChapterLevels.push_back(levelInfo);
	}

	sqlite3_free_table(re);
}

std::vector<LevelInfo>& LevelTable::getAllLevelsForChapter(std::string& sChapterId)
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
	int iIndex;
	for(iIndex=0; iIndex<m_ChapterLevels.size(); iIndex++)
	{
		if (m_ChapterLevels[iIndex].iLevel == levelInfo.iLevel && m_ChapterLevels[iIndex].sChapterId == levelInfo.sChapterId)
		{
			break;
		}
	}

	String sql = "";
	if (iIndex < m_ChapterLevels.size())
	{
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

		m_ChapterLevels[iIndex] = levelInfo;
	}
	else 
	{
		// Insert Level
		sql.append("insert into Levels (ChapterId,Level,WordId,Star,Score,BonusQuest,TotalBonusQuest,IsUnlock,Version) values(");
		sql.appendWithFormat("'%s',", levelInfo.sChapterId.c_str());
		sql.appendWithFormat("%d,", levelInfo.iLevel);
		sql.appendWithFormat("'%s',", levelInfo.sWordId.c_str());
		sql.appendWithFormat("%d,", levelInfo.iStar);
		sql.appendWithFormat("%d,", levelInfo.iScore);
		sql.appendWithFormat("%d,",levelInfo.iBonusQuest);
		sql.appendWithFormat("%d,", levelInfo.iTotalBonusQuest);
		sql.appendWithFormat("%d,", levelInfo.bIsUnlock);
		sql.appendWithFormat("%d);", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);

		if (m_sCurrentChapterId != "")
			this->fetchLevelsForChapter(m_sCurrentChapterId);
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

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
	int iIndexCurrentChapter = 0;
	int iCurrentLevel = 1;
	cs::JsonDictionary *pJsonUser = pJsonSync->getSubDictionary("User");
	WordlMapConfig worldMapConfig = GameConfigManager::getInstance()->GetWordlMapConfig();
	if (pJsonUser != NULL)
	{
		iIndexCurrentChapter = worldMapConfig.m_WorlMapChapterConfigMap[pJsonUser->getItemStringValue("CurrentChapter")];
		iCurrentLevel = int(strtod(pJsonUser->getItemStringValue("CurrentLevel"), 0));
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
		int iLevel = int(strtod(pJsonLevel->getItemStringValue("Level"), 0));
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
		if(iIndexChapter > iIndexCurrentChapter)
			isUnlock = 0;
		else if (iIndexChapter == iIndexCurrentChapter && iCurrentLevel <= iLevel)
			isUnlock = 0;

		if (isInsert)
		{
			// Insert Level
			sqlRun.append("insert into Levels (ChapterId,Level,WordId,Star,Score,BonusQuest,TotalBonusQuest,IsUnlock,Version) values(");
			sqlRun.appendWithFormat("'%s',", pJsonLevel->getItemStringValue("ChapterId"));
			sqlRun.appendWithFormat("%s,", pJsonLevel->getItemStringValue("Level"));
			sqlRun.appendWithFormat("'%s',", pJsonLevel->getItemStringValue("WordId"));
			sqlRun.appendWithFormat("%s,", pJsonLevel->getItemStringValue("Stars"));
			sqlRun.appendWithFormat("%s,", pJsonLevel->getItemStringValue("Score"));
			sqlRun.appendWithFormat("%s,", pJsonLevel->getItemStringValue("BonusQuest"));
			sqlRun.appendWithFormat("%s,", pJsonLevel->getItemStringValue("TotalBonusQuest"));
			sqlRun.appendWithFormat("%d,", isUnlock);
			sqlRun.appendWithFormat("%d);", iVersion);
		}
		else
		{
			// Update Level
			sqlRun.append("update Levels Set");
			sqlRun.appendWithFormat(" WordId='%s',", pJsonLevel->getItemStringValue("WordId"));
			sqlRun.appendWithFormat(" Star=%s,", pJsonLevel->getItemStringValue("Stars"));
			sqlRun.appendWithFormat(" Score=%s,", pJsonLevel->getItemStringValue("Score"));
			sqlRun.appendWithFormat(" BonusQuest=%s,", pJsonLevel->getItemStringValue("BonusQuest"));
			sqlRun.appendWithFormat(" TotalBonusQuest=%s,", pJsonLevel->getItemStringValue("TotalBonusQuest"));
			sqlRun.appendWithFormat(" IsUnlock=%d,", isUnlock);
			sqlRun.appendWithFormat(" Version=%d", iVersion);
			sqlRun.appendWithFormat(" where ChapterId='%s'", pJsonLevel->getItemStringValue("ChapterId"));
			sqlRun.appendWithFormat(" and Level=%s;", pJsonLevel->getItemStringValue("Level"));
		}
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sqlRun.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	if (m_sCurrentChapterId != "")
		this->fetchLevelsForChapter(m_sCurrentChapterId);
	return true;
}