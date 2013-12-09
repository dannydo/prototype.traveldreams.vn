#ifndef _GAME_WORD_MANAGER_H_
#define _GAME_WORD_MANAGER_H_

#include "GameDataStructure.h"

class GameWordManager
{
public:	
	static GameWordManager* getInstance()
	{ 
		if (m_pInstance == NULL)
		{
			m_pInstance = new GameWordManager();
			m_pInstance->LoadWords();
		}
		return m_pInstance;
	}

	static void releaseInstance()
	{
		if (m_pInstance == NULL)
		{
			delete m_pInstance;
		}
		m_pInstance = NULL;
	}


private:
	GameWordManager();

	static GameWordManager* m_pInstance;
public:	
	void LoadWords();
	void GenerateWordForNewLevel();

	inline const Word& GetMainWord() { return m_WordList[m_iMainWordIndex];}
	int GetSubWordCount() { return _GDS_SUB_WORD_MAX_COUNT_;}
	inline const Word& GetSubWord(const int& iSubWordIndex) {  return m_WordList[m_SubWordList[iSubWordIndex]];}

	bool UnlockWord(const char& iLetter, int& iUnlockedLetterIndexOfMainWord, std::vector<int> (&unlockedLettersIndexOfSubWords)[_GDS_SUB_WORD_MAX_COUNT_]);

private:
	int m_iWordCount;	
	Word m_WordList[_GDS_WORD_MAX_COUNT_];

	int m_iMainWordIndex;
	int m_SubWordList[_GDS_SUB_WORD_MAX_COUNT_];
};

#endif _GAME_WORD_MANAGER_H_