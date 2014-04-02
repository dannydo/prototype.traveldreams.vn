#ifndef __UNLOCK_CHAPTER_TABLE_H__
#define __UNLOCK_CHAPTER_TABLE_H__

#include "cocos2d.h"

struct UnlockChapterInfo
{
	int iUnlockChapterId;
	std::string sChapterId;
	int iRequest;
	std::string sType;
	unsigned long uBeginTime;
	int iVersion;
};

class UnlockChapterTable
{
public:
	static UnlockChapterTable* getInstance();
	UnlockChapterTable();
	bool init();
	static void releaseInstance();

	UnlockChapterInfo fetchUnlockChapter(const std::string sChapterId, const std::string sType);
	bool insertUnlockChapter(const UnlockChapterInfo& unlockChapterInfo);
	bool updateUnlockChapter(const UnlockChapterInfo& unlockChapterInfo);
	
private:
	unsigned long getTimeLocalCurrent();

	static UnlockChapterTable* m_UnlockChapterTable;
};
#endif