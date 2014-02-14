#ifndef _GAME_WORD_MANAGER_H_
#define _GAME_WORD_MANAGER_H_

#include "GameDataStructure.h"
#include "GameConfigManager.h"
#include "GemLetterManager.h"

bool SuccessWithPercentRatio(const int& iPercentRatio);

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
	void GenerateWordForNewLevel(int iLevel);
	void RetryCurrentLevel();

	inline const Word& GetMainWord() { return m_WordList[m_iMainWordIndex];}
	int GetSubWordCount() { return m_iSubWordCount;}
	inline const Word& GetSubWord(const int& iSubWordIndex) {  return m_WordList[m_SubWordList[iSubWordIndex]];}
	bool IsMainWordUnlocked() { return (m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount == 0);}

	int GetLengthOfMainWord();
	int GetLengthOfSubWord();

	int GetWordIndexFromContent(const std::string& sWord);
	inline const Word& GetWord(const int& iWordIndex) { return m_WordList[iWordIndex];}

	void EnableVersusBossMode();
	bool GenerateLetterFromMainWord(unsigned char& sLetter);
	//unsigned char GetFirstInactivatedLetterOfMainWord();
	//void ActivateFirstInactivatedLetterOfMainWord();

	// generate new letters for new gems, also update rate
	void GenerateNewLetters(const int& iGemCount, std::vector<GemLetterData>& outputLettersForGems, bool bIsNewMove);	
	
	//bool GenerateNewLetter(unsigned char& sOuputLetter, const GemComboType_e& eComboType);	
	void UpdateParamForNewMove();

	bool UnlockLetter(const unsigned char& iLetter, const bool& bIsInMainWord, int& iUnlockedLetterIndexOfMainWord, std::vector<int> (&unlockedLettersIndexOfSubWords)[_GDS_SUB_WORD_MAX_COUNT_],
		bool& bIsMainWordJustUnlocked, bool (&justUnlockedSubWords)[_GDS_SUB_WORD_MAX_COUNT_]);
		//bool& bIsMainWordFullUnlocked);	
private:
	void ResetDataForNewPlay();
	
	bool GenerateLetterFromSubWords(unsigned char& sLetter);
	bool GenerateLetterFromTrashCollection(unsigned char& sLetter);

	void AddLetterToTrashCollection(const unsigned char sLetter);
private:
	// word generate config
	WordGenerateConfig m_WordGenerateConfig;

	const LevelConfig* m_pLevelConfig;

	int m_iWordCount;	
	Word m_WordList[_GDS_WORD_MAX_COUNT_];

	int m_iMainWordIndex;
	//int m_iMainWordLength;

	int m_SubWordList[_GDS_SUB_WORD_MAX_COUNT_];
	int m_iSubWordCount;
	//int m_SubWordLength[_GDS_SUB_WORD_MAX_COUNT_];

	// current state
	int m_iCountOfLettersOnBoard;

	// generate word rate
	int m_iMainWordGenerateRate;
	int m_iSubWordGenerateRate;
	int m_iTrashWordGenerateRate;

	bool m_bAddDirectLetterOfMainWordToTrash;

	// 
	std::vector<unsigned char> m_SubWordLettersCollection;
	std::vector<unsigned char> m_TrashLettersCollection;
};

#endif _GAME_WORD_MANAGER_H_