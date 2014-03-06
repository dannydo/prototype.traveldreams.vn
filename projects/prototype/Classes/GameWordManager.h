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
	bool IsMainWordUnlocked() { return (m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount == 0);}
	inline int GetTotalCollectibleLettersOfMainWord() { return m_iTotalCollectibleLettersOfMainWord;}

	int GetLengthOfMainWord();
	int GetLengthOfSubWord();

	int GetWordIndexFromContent(const std::string& sWord);
	inline const Word& GetWord(const int& iWordIndex) { return m_WordList[iWordIndex];}

	void EnableVersusBossMode();
	bool GenerateLetterFromMainWord(unsigned char& sLetter);
	//unsigned char GetFirstInactivatedLetterOfMainWord();
	//void ActivateFirstInactivatedLetterOfMainWord();

	// generate new letters for new gems, also update rate
	void GenerateNewLetters(const std::vector<bool>& gemCanContainLetterFlagList, const int& iGemCount, std::vector<GemLetterData>& outputLettersForGems, bool bIsNewMove);	
	
	//bool GenerateNewLetter(unsigned char& sOuputLetter, const GemComboType_e& eComboType);	
	void UpdateParamForNewMove();

	bool UnlockLetter(const unsigned char& iLetter, int& iUnlockedLetterIndexOfMainWord, 
		bool& bIsMainWordJustUnlocked);
		//bool& bIsMainWordFullUnlocked);	
private:
	void ResetDataForNewPlay();
		
	bool GenerateLetterFromTrashCollection(unsigned char& sLetter);

	void AddLetterToTrashCollection(const unsigned char sLetter);
private:
	// word generate config
	WordGenerateConfig m_WordGenerateConfig;

	const LevelConfig* m_pLevelConfig;

	int m_iWordCount;	
	Word m_WordList[_GDS_WORD_MAX_COUNT_];

	int m_iMainWordIndex;
	int m_iTotalCollectibleLettersOfMainWord;
	//int m_iMainWordLength;	

	// current state
	int m_iCountOfLettersOnBoard;

	// generate word rate
	int m_iMainWordGenerateRate;	
	int m_iTrashWordGenerateRate;

	bool m_bAddDirectLetterOfMainWordToTrash;

	// 	
	std::vector<unsigned char> m_TrashLettersCollection;
};

#endif _GAME_WORD_MANAGER_H_