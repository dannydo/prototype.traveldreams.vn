#ifndef __WORD_TABLE_H__
#define __WORD_TABLE_H__

#include "cocos2d.h"

struct WordInfo
{
	std::string sWordId;
	int iCountCollected;
	int iVersion;
	int iOrderUnlock;
	bool bIsNew;
	int iMapChapterWordId;
	std::string sChapterId;

	WordInfo()
	{
		sWordId = "";
		iCountCollected = -1;
		iVersion = -1;
	}
};

class WordTable
{
public:
	static WordTable* getInstance();
	bool init();
	WordTable();
	static void releaseInstance();

	std::vector<WordInfo>& getAllWordsForChapter(const std::string& sChapterId);
	WordInfo getWordInfoOnChapter(const std::string& sChapterId, const std::string& sWordId);
	bool updateWord(const WordInfo& wordInfo);
	void refreshWordsForChapter(const std::string& sChapterId);

private:
	void fetchWordsForChapter(const std::string& sChapterId);

	static WordTable* m_WordTable;
	std::vector<WordInfo> m_ChapterWords;
	std::string m_sCurrentChapterId;
};
#endif