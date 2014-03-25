#ifndef __LEVEL_TABLE_H__
#define __LEVEL_TABLE_H__

#include "cocos2d.h"

struct LevelInfo
{
	int iLevel;
	int iChapter;
	int iStar;
	int iScore;
	int iBonusQuest;
	int iTotalBonusQuest;
	bool bIsUnlock;
	bool bIsUpdate;
	std::string sWordKey;
};

class LevelTable
{
public:
	static LevelTable* getInstance();
	bool init();
	LevelTable();
	static void releaseInstance();

	std::vector<LevelInfo> fetchLevelsForChapter(const int& iChapter);
	LevelInfo fetchLevel(const int& iLevel);
	bool updateLevel(LevelInfo levelInfo);

private:
	static LevelTable* m_LevelTable;
};
#endif