#ifndef __LEVEL_TABLE_H__
#define __LEVEL_TABLE_H__

#include "cocos2d.h"

struct LevelInfo
{
	int iLevelId;
	std::string sChapterId;
	int iLevel;
	std::string sWordId;
	int iStar;
	int iScore;
	int iBonusQuest;
	int iTotalBonusQuest;
	bool bIsUnlock;
	int iVersion;
};

class LevelTable
{
public:
	static LevelTable* getInstance();
	bool init();
	LevelTable();
	static void releaseInstance();

	std::vector<LevelInfo> getAllLevelsForChapter(std::string& sChapterId);
	LevelInfo& getLevel(const std::string& sChapterId, const int& iLevel);
	bool updateLevel(const LevelInfo& levelInfo);

private:
	void fetchLevelsForChapter(const std::string& sChapterId);

	static LevelTable* m_LevelTable;
	std::vector<LevelInfo> m_ChapterLevels;
	std::string m_sCurrentChapterId;
};
#endif