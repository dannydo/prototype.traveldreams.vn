#ifndef _GAME_CONFIG_MANAGER_H_
#define _GAME_CONFIG_MANAGER_H_

#include "GameDataStructure.h"
#include <map>
#include "cocos2d.h"
#include "CustomModeDefines.h"

USING_NS_CC;

#define _MAX_OBSTACLE_TYPE_COUNT_	5

#define _MAXIMUM_BONUS_WORD_COUNT_	10
#define _MAXIMUM_LEVEL_ON_CHAPTER_ 30

struct Level_ObstacleConfig
{
public:
	int m_iObstacleID;
	int m_iObstacleLevel;

	// existing on board
	int m_iCount;	
	Cell m_ObstaclePositionList[_BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_];

	// drop
	bool m_bEnableGenerateByDrop;
	bool m_bCanDropOnAllColumn; //if true, use dropOnAllColumnRate, else then m_DropOnColumnCount and next 2 list
	int m_iDropOnAllColumnRate; //percent	
	int m_DropOnColumnsRateList[_BOARD_MAX_COLUMN_NUMBER_]; //percent

	Level_ObstacleConfig()
	{
		m_iObstacleID = -1;
		m_iObstacleLevel = 0;
		m_iCount = 0;

		m_bEnableGenerateByDrop = false;
		m_bCanDropOnAllColumn = true;
		m_iDropOnAllColumnRate = 0;		
		memset(m_DropOnColumnsRateList, 0, sizeof(m_DropOnColumnsRateList));
	}
};

struct LevelBossConfig
{
public:
	Cell m_Position;
	int m_iWidth, m_iHeight;
	int m_HitPointPerLetter;
};

enum BonusQuestType : int
{
	_BQT_COLLECT_GEM_ = 0,
	_BQT_COLLECT_COMBO_ = 1,
	_BQT_COLLECT_BONUS_WORD_ = 2,
	_BQT_TYPE_COUNT_ = 3
};

struct BonusQuestCollectGemDescription
{
public:
	int m_CountPerGemType[_MAX_GEM_ID_];
	//reward here?
};

struct BonusQuestCollectComboDescription
{
public:
	int m_CountPerComboGem[_GCT_SINGLE_COMBO_COUNT_];//only count combo4, combo5, combo6
};

struct BonusQuestCollectBonusWordDescription
{
public:
	//int m_BonusWordIDList[_MAXIMUM_BONUS_WORD_LENGTH_];
	std::string m_BonusWordIDList[_MAXIMUM_BONUS_WORD_COUNT_];
	int m_iBonusWordCount;
	int m_iRequiredLetterCount;
};

struct BonusQuestConfig
{
public:	
	int m_iBonusQuestCount;
	bool m_bIsBonusGemAppearOnStartGame;// true: appear on startGame, false: drop at later move
	Cell m_PositionOfBonusGemAtStartGame[3];
	int m_BonusGemAppearAtMoves[3];

	bool m_IsBonusEnabledQuestFlags[_BQT_TYPE_COUNT_];
	//bool m_bIsEnableCollectGemQuest;
	BonusQuestCollectGemDescription m_CollectGemQuest;
	//bool m_bIsEnableCollectComboQuest;
	BonusQuestCollectComboDescription m_CollectComboQuest;
	BonusQuestCollectBonusWordDescription m_CollectBonusWordQuest;

	BonusQuestConfig()
	{
		m_iBonusQuestCount = 0;
		m_bIsBonusGemAppearOnStartGame = true;
		memset( m_IsBonusEnabledQuestFlags, 0, sizeof( m_IsBonusEnabledQuestFlags));
	}
};

enum EndGameBonusType : int
{
	_EGBT_SPECIAL_GEMS_ = 1,
	_EGBT_CRAZY_PETS_ = 2
};

struct LevelConfig : public BaseLevelConfig
{
public:
	
	// number of move 
	int m_iNumberOfMove;

	// main word
	//int m_iMainWordID;
	//std::string m_sMainWordID;
	int m_iNumberLetterOfMainWord;
	//bool m_MainWordUnlockedFlagList[_GDS_WORD_MAX_LENGTH_]; //should remove this
	bool m_bIsMainWordExistedOnBoard;
	Cell m_MainWordLetterPosition[_GDS_WORD_MAX_LENGTH_];
	// display of main word
	//bool m_bBreakLineWhenDisplayMainWord;
	//int m_iLetterCountOfFirstLine; //note: include space
	// config of main word
	int m_iInitRateOfMainLetter; //letter of main word
	int m_iIncreasePercentAfterEachMoveOfMainLetter;
	int m_iDecreasePercentAfterLetterDestroyedOfMainLetter;
	int m_iDecreasePercentAfterLetterAppearedOfMainLetter;
	int m_iRatioBetweenLettersOfMainWord; //in the same generation
	bool m_bMainWordCanDropOnAllColumn;
	bool m_MainWordDropOnColumnsFlagList[_BOARD_MAX_COLUMN_NUMBER_]; //true: enable	

	// end-game bonus type
	EndGameBonusType m_eEndGameBonusType;
	// obstacles
	std::vector<Level_ObstacleConfig*> m_ObstacleConfigList;
	// boss config
	bool m_bEnableBoss;
	LevelBossConfig m_BossConfig;
	// bonus quest config
	BonusQuestConfig m_BonusQuestConfig;	

	LevelConfig()
	{
		m_eGameModeType = _GMT_NORMAL_;
	}

	~LevelConfig()
	{		
		Clear();
	}

	void Clear()
	{
		m_bEnableBoss = false;
		
		for(auto pObstacle : m_ObstacleConfigList)
			delete pObstacle;
		m_ObstacleConfigList.clear();
	}
};

struct ChapterConfig  {
public:
	int m_iTotalBackgroundImage;
	int m_iTotalevel;
	Point m_PositionLevel[_MAXIMUM_LEVEL_ON_CHAPTER_];
	std::vector<std::string> m_WordIDList;
};

struct WordlMapConfig {
public:
	struct WordMapChapterConfig  {
	public:
		std::string m_sChapterId;
		std::string m_sChapterName;
		std::string m_sPathData;
		Point m_position;
		int m_iTotalevel;
		bool m_hasUnlock;
		int m_iRequest;
	};

	std::string m_sFileNameBackgound;
	int m_iTotalChapter;
	std::map<std::string, int> m_WorlMapChapterConfigMap;
	std::vector<WordMapChapterConfig> m_WorlMapChapterConfigs;
};

struct ObstacleLevelDescription
{
public:
	int m_iLevelID;
	std::string m_sSpriteFileName;
};

struct ObstacleDescription
{
public:
	std::string m_sName;
	int m_iObstacleID;
	int m_iPriority;
	
	bool m_bIsWrapGem; //it is standalone (false) or wrap a gem (true)
	
	bool m_bIsInfectable;
	std::vector<int> m_InfectableObjects; //with priority
	
	bool m_bCanBeMoveThrough; //can be moved through
	
	bool m_bDropable;
	
	bool m_bLockRow;
	bool m_bLockColumn;
	
	std::vector<ObstacleLevelDescription> m_LevelList;

	Point m_TranslatePosition;
	
	bool m_bIsDrawLevelLabel;
	std::string m_sLevelLabelImageFolder;

	bool m_bDecreaseLevelAfterDestroyed;
	bool m_bDecreaseLevelAfterMoved;
	
	int m_TransformToObjectAtLevel0;
	int m_iLevelOfTransformedObject;
	
	bool m_bIsDestroyable;
	bool m_bMatchAroundToDestroyed;
		
	bool m_bAppearByDrop;
	//int m_iDropRate;
	
	bool m_bAppearByTransform;	
	int m_iTransformRate;
};

class GameConfigManager
{
private:
	static GameConfigManager* m_pInstance;
public:
	static GameConfigManager* getInstance()
	{
		if (m_pInstance == NULL)
		{
			m_pInstance = new GameConfigManager();
		}
		return m_pInstance;
	}

	static void releaseInstance()
	{
		if(m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}

private:
	GameConfigManager();
public:
	~GameConfigManager();
	
	void LoadGameConfig();
	void LoadObstacleConfig();
	void LoadWordMapConfig();

	void LoadCustomModeConfig();

	LevelConfig& GetLevelConfig(const std::string& sChapterID, const int& iLevelId);
	ChapterConfig& GetChapterConfig(const std::string& sChapterID);
	WordlMapConfig::WordMapChapterConfig& GetWordMapChapterConfig(const std::string& sChapterID);	

	inline const GameConfig& GetGameConfig() { return m_GameConfig;}	
	inline int GetObstacleTypeCount() { return m_ObstacleDescriptionArray.size();}

	inline const std::string& GetCurrentChapterID() { return m_sCurrentChapterID; };
	inline const int& GetCurrentLevelId() { return m_iCurrentLevelID; };
	inline const WordlMapConfig& GetWordlMapConfig() { return m_WordlMapConfig; };

	TimeModeLevelConfig& GetTimeModeDemoConfig() { if (!m_bIsTimeModeLevelConfigLoaded) LoadCustomModeConfig(); return m_TimeModeLevelConfig;}
	
	int CountLevelOfPreviousChapters(const std::string& sChapterID);

	inline bool GetNextChapterID(const std::string& sChapterID, std::string& sNextChapterID){
		int iChapterIndex = m_WordlMapConfig.m_WorlMapChapterConfigMap[sChapterID];		
		if (iChapterIndex + 1 < m_WordlMapConfig.m_iTotalChapter)
		{
			sNextChapterID = m_WordlMapConfig.m_WorlMapChapterConfigs[ iChapterIndex+1].m_sChapterId;
			return true;
		}
		else
			return false;
	}

	inline const ObstacleDescription* GetObstacleDescription(const int& iObstacleTypeID) {
		if (iObstacleTypeID < m_ObstacleDescriptionArray.size())
			return m_ObstacleDescriptionArray[iObstacleTypeID];
		else
			return NULL;
	}

	const ObstacleLevelDescription& GetObstacleLevelDescription(const int& iObstacleTypeID, const int& iLevel);


	void GenerateWordsForNewChapter(const std::string& sChapterID, std::vector<std::string>& outputWordList, std::vector<int>& outputLevelList);
	void UpdateNewWordForLevel(const std::string& sChapterID, const int& iLevel);
private:
	void LoadConfigOfLevel(const std::string& sChapterID, const int& iLevelId);
	void LoadConfigOfChapter(const std::string& sChapterID);
private:
	LevelConfig m_LevelConfig;
	int m_iCurrentLevelID;

	ChapterConfig m_ChapterConfig;
	std::string m_sCurrentChapterID;

	WordlMapConfig m_WordlMapConfig;

	GameConfig m_GameConfig;	

	std::vector<ObstacleDescription*> m_ObstacleDescriptionArray;
	std::map<std::string,int> m_ObstaleNameToIDMap;
	bool m_ObstacleCompatibleMatrix[_MAX_OBSTACLE_TYPE_COUNT_][_MAX_OBSTACLE_TYPE_COUNT_];


	// custom mode
	bool m_bIsTimeModeLevelConfigLoaded;
	TimeModeLevelConfig m_TimeModeLevelConfig;
};

#endif _GAME_CONFIG_MANAGER_H_