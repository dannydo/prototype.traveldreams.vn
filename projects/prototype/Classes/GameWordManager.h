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
	void LoadWordGenerateConfig();
	void LoadWords();
	void GenerateWordForNewLevel();
	void RetryCurrentLevel();

	inline const Word& GetMainWord() { return m_WordList[m_iMainWordIndex];}
	int GetSubWordCount() { return _GDS_SUB_WORD_MAX_COUNT_;}
	inline const Word& GetSubWord(const int& iSubWordIndex) {  return m_WordList[m_SubWordList[iSubWordIndex]];}
	bool IsMainWordUnlocked() { return (m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount == 0);}

	// generate new letters for new gems, also update rate
	void GenerateNewLetters(const int& iGemCount, std::vector<unsigned char>& outputLettersForGems, bool bIsNewMove);	
	
	bool GenerateNewLetter(unsigned char& sOuputLetter, const GemComboType_e& eComboType);	
	void UpdateParamForNewMove();

	bool UnlockLetter(const unsigned char& iLetter, int& iUnlockedLetterIndexOfMainWord, std::vector<int> (&unlockedLettersIndexOfSubWords)[_GDS_SUB_WORD_MAX_COUNT_],
		bool& bIsMainWordJustUnlocked, bool (&justUnlockedSubWords)[_GDS_SUB_WORD_MAX_COUNT_]);
		//bool& bIsMainWordFullUnlocked);

	int GetLengthOfMainWord();
	int GetLengthOfSubWord();
private:
	void ResetDataForNewPlay();

	bool GenerateLetterFromMainWord(unsigned char& sLetter);
	bool GenerateLetterFromSubWords(unsigned char& sLetter);
	bool GenerateLetterFromTrashCollection(unsigned char& sLetter);

	void AddLetterToTrashCollection(const unsigned char sLetter);
private:
	// word generate config
	WordGenerateConfig m_WordGenerateConfig;

	int m_iWordCount;	
	Word m_WordList[_GDS_WORD_MAX_COUNT_];

	int m_iMainWordIndex;
	//int m_iMainWordLength;

	int m_SubWordList[_GDS_SUB_WORD_MAX_COUNT_];
	//int m_SubWordLength[_GDS_SUB_WORD_MAX_COUNT_];

	// current state
	int m_iCountOfLettersOnBoard;

	// generate word rate
	int m_iMainWordGenerateRate;
	int m_iSubWordGenerateRate;
	int m_iTrashWordGenerateRate;

	// 
	std::vector<unsigned char> m_SubWordLettersCollection;
	std::vector<unsigned char> m_TrashLettersCollection;
};

#endif _GAME_WORD_MANAGER_H_