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
	bool bIsUnlock;
	bool bIsUpdate;
};

class LevelTable
{
public:
	static LevelTable* getInstance();
	bool init();
	std::vector<LevelInfo> fetchLevelsForChapter(const int& iChapter);
	bool updateLevel(LevelInfo levelInfo);

private:
	LevelTable();
	virtual ~LevelTable();

	static LevelTable* m_LevelTable;
};
#endif