#ifndef _CUSTOM_MODE_DEFINES_H_	
#define _CUSTOM_MODE_DEFINES_H_	

#include "GameDataStructure.h"


struct BaseLevelConfig
{
public:
	GameModeType_e m_eGameModeType;

	// map size
	int m_iRowNumber, m_iColumnNumber;
	// number of color	
	int m_iNumberOfColor;
	// score of stars
	int m_ScoreOfStars[3];
	// map matrix
	int m_BoardMatrix[_BOARD_MAX_ROW_NUMBER_][_BOARD_MAX_COLUMN_NUMBER_];
};


struct TimeModeLevelConfig : public BaseLevelConfig
{
public:
	struct ObstacleConfig
	{
	public:
		int m_iObstacleID;
		int m_iObstacleLevel;
	};

	struct ObstacleDropConfig : public ObstacleConfig
	{
	public:		
		int m_DropOnColumnsRateList[_BOARD_MAX_COLUMN_NUMBER_];		
	};

	struct ManualStageConfig
	{
	public:
		int m_iMaximumEnergy;
		int m_iEnergyLostRatePersecond;
		
		// obstacle generate config
		std::vector<ObstacleDropConfig> m_ObstacleDropConfigList;

		// obstacle appear on letter config		
		std::vector<ObstacleConfig> m_ObstacleOnLetterConfigList;
	};

public:		
	std::string m_sCustomPackageID;

	// word list
	std::vector<int> m_WordIndexList;
	std::vector<int> m_WordCollectedCountList;

	// stage config
	//int m_iNumberOfManualStageConfig;
	// manual config list
	std::vector<ManualStageConfig*> m_ManualStageConfigList;

	// auto config with increasing difficulty for greater levels
	int m_iStageConfig_MaximumValueIncreasePercent;
	int m_iStageConfig_LostRateIncreasePercent;

	// special score config only used in this level
	GameConfig m_ScoreConfig;
	
	TimeModeLevelConfig()
	{
		m_eGameModeType = _GMT_TIME_MODE_;
	}

	~TimeModeLevelConfig()
	{				
	}	
};

#endif