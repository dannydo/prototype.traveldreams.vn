#ifndef _GAME_CONFIG_MANAGER_H_
#define _GAME_CONFIG_MANAGER_H_

#include "GameDataStructure.h"
#include <map>

#define _MAX_OBSTACLE_TYPE_COUNT_	5


struct Level_ObstacleConfig
{
public:
	int m_iObstacleID;
	int m_iObstacleLevel;
	int m_iCount;	
	Cell m_ObstaclePositionList[_BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_];

	Level_ObstacleConfig()
	{
		m_iObstacleID = -1;
		m_iObstacleLevel = 0;
		m_iCount = 0;
	}
};

struct LevelBossConfig
{
public:
	Cell m_Position;
	int m_iWidth, m_iHeight;
	int m_HitPointPerLetter;
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
	// bonus words
	int m_iBonusWordsCount;
	int m_BonusWordIDList[_GDS_SUB_WORD_MAX_COUNT_];
	// obstacles
	std::vector<Level_ObstacleConfig*> m_ObstacleConfigList;
	// boss config
	bool m_bEnableBoss;
	LevelBossConfig m_BossConfig;

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
	
	bool m_bDecreaseLevelAfterDestroyed;
	bool m_bDecreaseLevelAfterMoved;
	
	int m_TransformToObjectAtLevel0;
	int m_iLevelOfTransformedObject;
	
	bool m_bIsDestroyable;
	bool m_bMatchAroundToDestroyed;
		
	bool m_bAppearByDrop;
	int m_iDropRate;
	
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
	LevelConfig m_LevelConfigList[_MAX_GAME_LEVEL_];
	GameConfig m_GameConfig;	

	std::vector<ObstacleDescription*> m_ObstacleDescriptionArray;
	std::map<std::string,int> m_ObstaleNameToIDMap;
	bool m_ObstacleCompatibleMatrix[_MAX_OBSTACLE_TYPE_COUNT_][_MAX_OBSTACLE_TYPE_COUNT_];
};

#endif _GAME_CONFIG_MANAGER_H_