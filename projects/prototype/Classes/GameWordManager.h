#ifndef _GAME_WORD_MANAGER_H_
#define _GAME_WORD_MANAGER_H_

#include "GameDataStructure.h"
#include "GameConfigManager.h"
#include "GemLetterManager.h"

bool SuccessWithPercentRatio(const int& iPercentRatio);

struct WorldListPackgeDescription
{
public:
	std::string m_sPackgageID;
	std::string m_sPackagePath;
};

class GameWordManager
{
public:	
	static GameWordManager* getInstance()
	{ 
		if (m_pInstance == NULL)
		{
			m_pInstance = new GameWordManager();
			//m_pInstance->LoadWords();
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
	void LoadWorldListConfig();
	void LoadWords(const int& iPackageIndex);

	void ClearCache();

	int AddAndLoadCustomPackageToList(const std::string& sPackageID, const std::string& sPackagePath );
	inline const std::vector<Word>& GetWordList() { return m_WordList;}

	void GenerateWordForNewLevel(std::string sChapterID, int iLevel);
	void RetryCurrentLevel();

	// temporary only random words of word list, should implement correctly later
	void GenerateWordForNewLevelOfTimeMode(TimeModeLevelConfig* pTimeModeConfig, bool bStartNewTimeModeSession = false);
	long GetTotalPlayTimeOfTimeModeSession();

	inline const Word& GetMainWord() { return m_WordList[m_iMainWordIndex];}	
	inline const int GetLoadedIndexOfMainWord() { return m_iMainWordIndex;}
	bool IsMainWordUnlocked() { return (m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount == 0);}
	inline int GetTotalCollectibleLettersOfMainWord() { return m_iTotalCollectibleLettersOfMainWord;}
	inline const LevelConfig* GetLevelConfig() { return m_pLevelConfig;}

	int GetLengthOfMainWord();
	//int GetLengthOfSubWord();

	int GetLoadedWordIndexFromID(const std::string& sWordID);
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

	const std::string& GetPackagePathFromWordID(const std::string& sWordID);
	const std::string& GetPackagePathFromWord(const Word& word);
	const std::string GetWordIdFromWord(const Word& word);

private:
	void PreLoadPackageForWord(std::string sWordID);

	void ResetDataForNewPlay();
		
	bool GenerateLetterFromTrashCollection(unsigned char& sLetter);

	void AddLetterToTrashCollection(const unsigned char sLetter);
private:
	// word generate config
	WordGenerateConfig m_WordGenerateConfig;

	// cache current level config
	const LevelConfig* m_pLevelConfig;

	// new feature: support world package
	std::map<std::string,int> m_MapPackgeNameToIndex;
	std::vector<WorldListPackgeDescription> m_WorldPackageList;

	std::map<std::string,int> m_MapWordIDToLoadedIndex;


	//int m_iWordCount;	
	//Word m_WordList[_GDS_WORD_MAX_COUNT_];
	std::vector<Word> m_WordList;	

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


	// time mode only
	unsigned long m_iStartTimeOfTimeModeGameSession;
	/*struct TimeModeAnalysedWordInfo
	{
		int m_iLoadedWordIndex;
		int m_iCollectedCount;

		TimeModeAnalysedWordInfo(const int& iLoadedWordIndex, const int& iCollectedCount)
		{
			m_iLoadedWordIndex = iLoadedWordIndex;
			m_iCollectedCount = iCollectedCount;
		}
	};
	std::vector<TimeModeAnalysedWordInfo> m_TimeModeAnalysedWordList;*/
};

#endif _GAME_WORD_MANAGER_H_