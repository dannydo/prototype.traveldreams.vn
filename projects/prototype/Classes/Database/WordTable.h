#ifndef __WORD_TABLE_H__
#define __WORD_TABLE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#define _SECONDS_NEXT_PLAY_MINI_GAME_ 3600

struct WordInfo
{
	std::string sWordId;
	int iCountCollected;
	int iVersion;
	bool bIsCollected;
	unsigned long uTimeBeginPlayMiniGame;
	int iMapChapterWordId;
	std::string sChapterId;

	WordInfo()
	{
		sWordId = "";
		sChapterId = "";
		iMapChapterWordId = 0;
		iCountCollected = 0;
		iVersion = 0;
		bIsCollected = 0;
		uTimeBeginPlayMiniGame = 0;
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
	bool insertWord(const WordInfo& wordInfo);
	void refreshWordsForChapter(const std::string& sChapterId);

	std::string	syncGetWords();
	std::string	syncGetMapChapterWords();
	bool updateDataSyncWords(cs::JsonDictionary* pJsonSync, const int& iVersion);
	bool updateDataSyncMapChapterWords(cs::JsonDictionary* pJsonSync, const int& iVersion);

	int getNumberWordNew();
	int getNumberWordPlayMiniGame(const unsigned long& iTimeCurrent);
	std::vector<WordInfo> getAllWordNew(const unsigned long& iTimeCurrent);

	std::vector<WordInfo> getWordInfoCollectedForChapter(const std::string& sChapterId);
	int getTotalWordInfoCollectedForChapter(const std::string& sChapterId);

	unsigned long getMinTimeNextPlayMiniGame();

	int getTotalWordCollected() { return m_iTotalFlashCardCollected; };
	int increaseTotalWordCollected(const int& iIncrease);

private:
	void fetchWordsForChapter(const std::string& sChapterId);
	int getNumberWordCollected();

	static WordTable* m_WordTable;
	std::vector<WordInfo> m_ChapterWords;
	std::string m_sCurrentChapterId;

	int m_iTotalFlashCardCollected;
};
#endif