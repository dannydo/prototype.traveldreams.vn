#ifndef __CHAPTER_TABLE_H__
#define __CHAPTER_TABLE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

struct ChapterInfo
{
	std::string sChapterId;
	int iTotalLevelUnlock;
	int iTotalStar;
	bool bIsUnlock;
	int iVersion;
	int iTotalFlashCardUnlock;
	int iTotalFlashCard;
	int iStageRevise;

	ChapterInfo()
	{
		sChapterId = "";
		iTotalLevelUnlock = 0;
		iTotalStar = 0;
		bIsUnlock = false;
		iVersion = 0;
		iTotalFlashCardUnlock = 0;
		iTotalFlashCard = 0;
		iStageRevise = 0;
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

	std::string	syncGetChapters();
	bool updateDataSyncChapters(cs::JsonDictionary* pJsonSync, const int& iVersion);

private:
	void fetchAllChapter();

	static ChapterTable* m_ChapterTable;
	std::vector<ChapterInfo> m_Chapters;
};
#endif
