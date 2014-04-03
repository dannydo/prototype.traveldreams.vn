#ifndef __WORD_TABLE_H__
#define __WORD_TABLE_H__

#include "cocos2d.h"

struct WordInfo
{
	std::string sWordId;
	int iCountCollected;
	int iVersion;

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

	std::vector<WordInfo> getAllWordsForChapter(std::string& sChapterId);
	WordInfo getWordInfoOnChapter(const std::string& sChapterId, const std::string& sWordId);
	bool updateWors(WordInfo wordInfo);

private:
	void fetchWordsForChapter(const std::string& sChapterId);

	static WordTable* m_WordTable;
	std::vector<WordInfo> m_ChapterWords;
	std::string m_sCurrentChapterId;
};
#endif