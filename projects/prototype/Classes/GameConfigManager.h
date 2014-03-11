#ifndef _GAME_CONFIG_MANAGER_H_
#define _GAME_CONFIG_MANAGER_H_

#include "GameDataStructure.h"
#include <map>
#include "cocos2d.h"

USING_NS_CC;

#define _MAX_OBSTACLE_TYPE_COUNT_	5
#define _MAXIMUM_BONUS_WORD_LENGTH_	10

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
	int m_BonusWordIDList[_MAXIMUM_BONUS_WORD_LENGTH_];
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

struct LevelConfig
{
public:
	// map size
	int m_iRowNumber, m_iColumnNumber;
	// number of move and color
	int m_iNumberOfMove;
	int m_iNumberOfColor;
	// score of stars
	int m_ScoreOfStars[3];
	// map matrix
	int m_BoardMatrix[_BOARD_MAX_ROW_NUMBER_][_BOARD_MAX_COLUMN_NUMBER_];
	// main word
	int m_iMainWordID;
	bool m_MainWordUnlockedFlagList[_GDS_WORD_MAX_LENGTH_];
	bool m_bIsMainWordExistedOnBoard;
	Cell m_MainWordLetterPosition[_GDS_WORD_MAX_LENGTH_];
	// display of main word
	bool m_bBreakLineWhenDisplayMainWord;
	int m_iLetterCountOfFirstLine; //note: include space
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

	~LevelConfig()
	{		
		m_bEnableBoss = false;
	}
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
	
	void LoadLevelsConfig();
	void LoadGameConfig();
	void LoadObstacleConfig();

	LevelConfig& GetLevelConfig(int iLevel);
	inline const GameConfig& GetGameConfig() { return m_GameConfig;}	
	inline int GetObstacleTypeCount() { return m_ObstacleDescriptionArray.size();}

	inline const ObstacleDescription* GetObstacleDescription(const int& iObstacleTypeID) {
		if (iObstacleTypeID < m_ObstacleDescriptionArray.size())
			return m_ObstacleDescriptionArray[iObstacleTypeID];
		else
			return NULL;
	}

	const ObstacleLevelDescription& GetObstacleLevelDescription(const int& iObstacleTypeID, const int& iLevel);
private:
	void LoadConfigOfLevel(int iLevel);		
private:
	LevelConfig m_LevelConfigList[_MAX_GAME_LEVEL_+1];
	GameConfig m_GameConfig;	

	std::vector<ObstacleDescription*> m_ObstacleDescriptionArray;
	std::map<std::string,int> m_ObstaleNameToIDMap;
	bool m_ObstacleCompatibleMatrix[_MAX_OBSTACLE_TYPE_COUNT_][_MAX_OBSTACLE_TYPE_COUNT_];
};

#endif _GAME_CONFIG_MANAGER_H_