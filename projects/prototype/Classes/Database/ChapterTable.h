#ifndef __CHAPTER_TABLE_H__
#define __CHAPTER_TABLE_H__

#include "cocos2d.h"

struct ChapterInfo
{
	std::string sChapterId;
	int iTotalLevelUnlock;
	int iTotalStar;
	bool bIsUnlock;
	int iVersion;
	int iTotalFlashCardUnlock;
	int iCountFlashCardNew;
	int iTotalFlash;

	ChapterInfo()
	{
		sChapterId = "";
		iTotalLevelUnlock = 0;
		iTotalStar = 0;
		bIsUnlock = false;
		iVersion = 0;
		iTotalFlashCardUnlock = 0;
		iCountFlashCardNew = 0;
		iTotalFlash = 0;
	}
};

class ChapterTable
{
public:
	static ChapterTable* getInstance();
	ChapterTable();
	static void releaseInstance();
	bool init();

	inline std::vector<ChapterInfo>& getChaptersInfo() { return m_Chapters; };
	ChapterInfo getChapterInfo(const std::string sChapterId);
	bool updateChapter(ChapterInfo chapterInfo);
	void refreshChapters();

private:
	void fetchAllChapter();

	static ChapterTable* m_ChapterTable;
	std::vector<ChapterInfo> m_Chapters;
};
#endif