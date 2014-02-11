#ifndef _OBSTACLE_PROCESS_MANAGER_H_
#define _OBSTACLE_PROCESS_MANAGER_H_

#include "GameDataStructure.h"
#include "GameConfigManager.h"
//#include "NewGameBoardManager.h"

struct ObstacleData
{
public:
	//int m_iObstacleTypeID;
	bool m_bIsActive;
	int m_iObstacleLevel;
	bool m_bIsDirty;

	ObstacleData()
	{
		m_bIsActive = false;
		m_iObstacleLevel = 0;
		m_bIsDirty = false;
	}
};

struct ObstacleDataBlock
{
public:
	bool m_bIsFreeBlock;
	ObstacleData m_ObstacleList[_MAX_OBSTACLE_TYPE_COUNT_];

	ObstacleDataBlock()
	{
		m_bIsFreeBlock = true;
	}
};

class NewGameBoardManager;

class ObstacleProcessManager
{
public:
	ObstacleProcessManager(NewGameBoardManager* pGameBoardManager);
	void InitLevel();

	bool DestroyCellWithObstacle(const int& iBlockID);
	void ForceDestroyObstacleBlock(const int& iBlockID); //clear block of obstacles, regard of its types (detach obstacle with cell is the job of GameBoardManager)
	bool IsRowLocked(const int& iBlockID);
	bool IsColumnLocked(const int& iBlockID);

	//inline ObstacleData& GetObstacleDataOfCell(const int& iRow, const int& iColumn, const int& iObstacleTypeID) { return m_ObstacleBoardMatrix[iRow][iColumn][iObstacleTypeID];}
	inline ObstacleData& GetObstacleData(const int& iBlockID, const int& iObstacleTypeID) { return m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID];}
	//void MoveObstacles(const int& iRow1, const int& iColumn1, const int& iRow2, const int& iColumn2);

	void CleanWaitingClearList();
	void UpdateAfterMove();
private:
	int AllocFreeBlock();
private:
	NewGameBoardManager* m_pGameBoardManager;	

	int m_SeedID;	
	//ObstacleData m_ObstacleBoardMatrix[_BOARD_MAX_ROW_NUMBER_][_BOARD_MAX_COLUMN_NUMBER_][_MAX_OBSTACLE_TYPE_COUNT_];
	ObstacleDataBlock m_ObstacleBoardList[_BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_];
	std::vector<int> m_BlockWaitingClearList;
};

#endif _OBSTACLE_PROCESS_MANAGER_H_