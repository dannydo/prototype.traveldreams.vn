#ifndef _GAME_CONFIG_MANAGER_H_
#define _GAME_CONFIG_MANAGER_H_

#include "GameDataStructure.h"


struct Level_ObstacleConfig
{
public:
	int m_iObstacleID;
	int m_iCount;
	Cell m_ObstaclePositionList[_BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_];
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
	bool m_bIsMainWordExistedOnBoard;
	Cell m_MainWordLetterPosition[_GDS_WORD_MAX_LENGTH_];
	// bonus words
	int m_iBonusWordsCount;
	int m_BonusWordIDList[_GDS_SUB_WORD_MAX_COUNT_];
	// obstacles
	std::vector<Level_ObstacleConfig*> m_ObstacleConfigList;
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

	const LevelConfig& GetLevelConfig(int iLevel);
	inline const GameConfig& GetGameConfig() { return m_GameConfig;}	

private:
	void LoadConfigOfLevel(int iLevel);
private:
	LevelConfig m_LevelConfigList[_MAX_GAME_LEVEL_];
	GameConfig m_GameConfig;	
};

#endif _GAME_CONFIG_MANAGER_H_