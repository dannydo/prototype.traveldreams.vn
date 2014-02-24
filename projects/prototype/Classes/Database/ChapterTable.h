#ifndef __CHAPTER_TABLE_H__
#define __CHAPTER_TABLE_H__

#include "cocos2d.h"

struct ChapterInfo
{
	int iChapter;
	std::string sName;
	int iMinLevel;
	int iMaxLevel;
	int iTotalStar;
	int iTotalScore;
	bool bIsUnlock;
	bool bIsUpdate;
};

class ChapterTable
{
public:
	static ChapterTable* getInstance();
	bool init();

	inline std::vector<ChapterInfo> getUserInfo() { return m_Chapters; };
	void fetchAllChapter();
	bool updateChapter(ChapterInfo chapterInfo);

private:
	ChapterTable();
	virtual ~ChapterTable();

	static ChapterTable* m_ChapterTable;
	std::vector<ChapterInfo> m_Chapters;
};
#endif