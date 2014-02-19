#include "ObstacleProcessManager.h"
#include "NewGameBoardManager.h"

ObstacleProcessManager::ObstacleProcessManager(NewGameBoardManager* pGameBoardManager)
{
	m_pGameBoardManager = pGameBoardManager;
}

void ObstacleProcessManager::InitLevel()
{
	const LevelConfig& levelConfig = m_pGameBoardManager->GetLevelConfig();
	int iBlockID;
	for(auto pObstacleLevelConfig : levelConfig.m_ObstacleConfigList)
	{
		for(int i=0; i< pObstacleLevelConfig->m_iCount; i++)
		{
			iBlockID = m_pGameBoardManager->GetObstacleBlockID( pObstacleLevelConfig->m_ObstaclePositionList[i].m_iRow, pObstacleLevelConfig->m_ObstaclePositionList[i].m_iColumn);
			if (iBlockID < 0)
			{
				iBlockID = AllocFreeBlock();				
				m_pGameBoardManager->SetObstacleBlockID( pObstacleLevelConfig->m_ObstaclePositionList[i].m_iRow, pObstacleLevelConfig->m_ObstaclePositionList[i].m_iColumn, iBlockID);
			}
			
			m_ObstacleBoardList[iBlockID].m_ObstacleList[pObstacleLevelConfig->m_iObstacleID].m_bIsActive = true;
			m_ObstacleBoardList[iBlockID].m_ObstacleList[pObstacleLevelConfig->m_iObstacleID].m_iObstacleLevel = 
				pObstacleLevelConfig->m_iObstacleLevel;
		}
	}

	/*
	for(int i=0; i< pLevelConfig->m_ObstacleConfigList.size)

	int iRow, iColumn, iObstacleTypeID;
	for(iRow=0; iRow < _BOARD_MAX_ROW_NUMBER_; iRow++)
		for(iColumn=0; iColumn < _BOARD_MAX_COLUMN_NUMBER_; iColumn++)
			for(iObstacleTypeID=0; iObstacleTypeID < _MAX_OBSTACLE_TYPE_COUNT_; iObstacleTypeID++)
			{

			}*/
}


void ObstacleProcessManager::GenerateObstacle(Level_ObstacleConfig* pLevelObstacleConfig, int& iBlockID)
{
	if (iBlockID < 0)
		iBlockID =  AllocFreeBlock();

	m_ObstacleBoardList[iBlockID].m_ObstacleList[pLevelObstacleConfig->m_iObstacleID].m_bIsActive = true;
	m_ObstacleBoardList[iBlockID].m_ObstacleList[pLevelObstacleConfig->m_iObstacleID].m_iObstacleLevel = pLevelObstacleConfig->m_iObstacleLevel;
	m_ObstacleBoardList[iBlockID].m_ObstacleList[pLevelObstacleConfig->m_iObstacleID].m_bIsDirty = true;
}

int ObstacleProcessManager::AllocFreeBlock()
{
	for(int i=0; i< _BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_; i++)
		if (m_ObstacleBoardList[i].m_bIsFreeBlock)
		{
			m_ObstacleBoardList[i].m_bIsFreeBlock = false;
			return i;
		}
	return -1;
}

bool ObstacleProcessManager::DestroyCellWithObstacle(const int& iBlockID)
{
	if (iBlockID < 0)
		return true;

	int iObstacleTypeCount = GameConfigManager::getInstance()->GetObstacleTypeCount();
	

	for(int iObstacleTypeID = 0; iObstacleTypeID < iObstacleTypeCount; iObstacleTypeID++)
	{
		const ObstacleDescription* pObstacleDescription = GameConfigManager::getInstance()->GetObstacleDescription(iObstacleTypeID);
		if (pObstacleDescription ==  NULL)
			continue;
		if (!pObstacleDescription->m_bIsDestroyable)
			return false;

		if (m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsActive)
		{
			if (m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsDirty)
				return false;

			// some obstacle should be removed instanly 1 time (with its gem) regard its level if m_bDecreaseLevelAfterDestroyed = false
			// otherwise, only obstacle at level 0 has this effect
			if (pObstacleDescription->m_bDecreaseLevelAfterDestroyed==false || m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_iObstacleLevel == 0)
			{
				m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsActive = false;												
			}
			else
			{
				m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_iObstacleLevel--;
				m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsDirty = true;				

				if (m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_iObstacleLevel == 0 && pObstacleDescription->m_LevelList[0].m_iLevelID != 0) // level 1 is lowest 
				{
					m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsActive = false;				
				}

				return false;
			}
		}
	}
	
	m_BlockWaitingClearList.push_back(iBlockID);

	return true;		
}

void ObstacleProcessManager::ForceDestroyObstacleBlock(const int& iBlockID)
{
	if (iBlockID < 0)
		return;
	m_ObstacleBoardList[iBlockID].m_bIsFreeBlock = true;	
}

void ObstacleProcessManager::CleanWaitingClearList()
{
	// update block waiting clear list
	for(auto iBlockID :  m_BlockWaitingClearList)
	{
		m_ObstacleBoardList[iBlockID].m_bIsFreeBlock = true;
		m_pGameBoardManager->ClearObstacleBlockID(iBlockID);
	}

	m_BlockWaitingClearList.clear();
}

void ObstacleProcessManager::UpdateAfterMove()
{	
	// update obstacle list that decrease level after moved
	int iObstacleTypeCount = GameConfigManager::getInstance()->GetObstacleTypeCount();
	int iBlockID;
	for(int iObstacleTypeID = 0; iObstacleTypeID < iObstacleTypeCount; iObstacleTypeID++)
	{
		const ObstacleDescription* pObstacleDescription = GameConfigManager::getInstance()->GetObstacleDescription(iObstacleTypeID);
		if (pObstacleDescription ==  NULL || !pObstacleDescription->m_bDecreaseLevelAfterMoved)
			continue;
		for(iBlockID=0; iBlockID< _BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_; iBlockID++)
			if (!m_ObstacleBoardList[iBlockID].m_bIsFreeBlock)
				if (m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsActive && !m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsDirty)
				{
					m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_iObstacleLevel--;
					m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsDirty = true;					

					if (m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_iObstacleLevel == 0)
					{
						m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsActive = false;

						if (pObstacleDescription->m_TransformToObjectAtLevel0 >= 0)
						{
							m_ObstacleBoardList[iBlockID].m_ObstacleList[pObstacleDescription->m_TransformToObjectAtLevel0].m_bIsActive = true;
							m_ObstacleBoardList[iBlockID].m_ObstacleList[pObstacleDescription->m_TransformToObjectAtLevel0].m_bIsDirty = true;
							m_ObstacleBoardList[iBlockID].m_ObstacleList[pObstacleDescription->m_TransformToObjectAtLevel0].m_iObstacleLevel = pObstacleDescription->m_iLevelOfTransformedObject;
						}													
					}
				}
	}
}

bool ObstacleProcessManager::IsRowLocked(const int& iBlockID)
{
	int iColumn=0;
	int iNumberOfColumn = m_pGameBoardManager->GetColumnNumber();
	int iObstacleTypeCount = GameConfigManager::getInstance()->GetObstacleTypeCount();

	for(int iObstacleTypeID = 0; iObstacleTypeID < iObstacleTypeCount; iObstacleTypeID++)
	{
		const ObstacleDescription* pObstacleDescription = GameConfigManager::getInstance()->GetObstacleDescription(iObstacleTypeID);
		if (pObstacleDescription ==  NULL || !pObstacleDescription->m_bLockRow)
			continue;
		if (m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsActive)
			return true;		
	}
	return false;
}
	
bool ObstacleProcessManager::IsColumnLocked(const int& iBlockID)
{
	int iRow=0;
	int iNumberOfRow = m_pGameBoardManager->GetRowNumber();
	int iObstacleTypeCount = GameConfigManager::getInstance()->GetObstacleTypeCount();

	for(int iObstacleTypeID = 0; iObstacleTypeID < iObstacleTypeCount; iObstacleTypeID++)
	{
		const ObstacleDescription* pObstacleDescription = GameConfigManager::getInstance()->GetObstacleDescription(iObstacleTypeID);
		if (pObstacleDescription ==  NULL || !pObstacleDescription->m_bLockColumn)
			continue;

		if (m_ObstacleBoardList[iBlockID].m_ObstacleList[iObstacleTypeID].m_bIsActive)
			return true;				
	}
	return false;
}
/*
void ObstacleProcessManager::MoveObstacles(const int& iRow1, const int& iColumn1, const int& iRow2, const int& iColumn2)
{
	int iObstacleTypeCount = GameConfigManager::getInstance()->GetObstacleTypeCount();
	for(int iObstacleTypeID = 0; iObstacleTypeID < iObstacleTypeCount; iObstacleTypeID++)
	{
		m_ObstacleBoardMatrix[iRow2][iColumn2][iObstacleTypeID] = m_ObstacleBoardMatrix[iRow1][iColumn1][iObstacleTypeID];

		m_ObstacleBoardMatrix[iRow1][iColumn1][iObstacleTypeID].m_bIsActive = false;
		m_ObstacleBoardMatrix[iRow1][iColumn1][iObstacleTypeID].m_bIsDirty = false;
		m_ObstacleBoardMatrix[iRow1][iColumn1][iObstacleTypeID].m_iObstacleLevel = 0;
	}
}*/