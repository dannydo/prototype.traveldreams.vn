#include "NewGameBoardManager.h"
#include "cocos2d.h"

NewGameBoardManager::NewGameBoardManager() : GameBoardManager()
{
	m_iCurrentScore = 0;		
	m_pObstacleProcessManager = new ObstacleProcessManager(this);
}

NewGameBoardManager::~NewGameBoardManager()
{
	delete m_pObstacleProcessManager;
}

void NewGameBoardManager::GenerateGameBoard(int iLevel)
{
	m_iCurrentLevel = iLevel;
	GameBoardManager::GenerateGameBoard(iLevel);
	
	m_pGameWordManager = GameWordManager::getInstance();	
	m_iCurrentMove = m_pLevelConfig->m_iNumberOfMove;

	m_pObstacleProcessManager->InitLevel();
	//m_pGameWordManager->GenerateWordForNewLevel(); //already called by main menu


	// process boss
	if (m_pLevelConfig->m_bEnableBoss)
	{
		if (m_pLevelConfig->m_bIsMainWordExistedOnBoard)
			m_pLevelConfig->m_bIsMainWordExistedOnBoard = false;
		
		m_pGameWordManager->EnableVersusBossMode();

		int iRow, iColumn, iRowInc, iColumnInc;
		for(iRowInc = 0; iRowInc < m_pLevelConfig->m_BossConfig.m_iHeight; iRowInc++)
			for(iColumnInc = 0; iColumnInc < m_pLevelConfig->m_BossConfig.m_iWidth; iColumnInc++)
			{
				iRow = m_pLevelConfig->m_BossConfig.m_Position.m_iRow + iRowInc;
				iColumn = m_pLevelConfig->m_BossConfig.m_Position.m_iColumn + iColumnInc;
				if (m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID >= 0)
				{
					m_pObstacleProcessManager->ForceDestroyObstacleBlock(m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID);
				}

				m_BoardValueMatrix[iRow][iColumn].Reset();
				m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell = true;
			}			

		m_LevelBossInfo.m_bIsEnable = true;
		m_LevelBossInfo.m_iCurrentHitPoint = m_pLevelConfig->m_BossConfig.m_HitPointPerLetter;
		m_LevelBossInfo.m_iRemainLettersCount = m_pGameWordManager->GetMainWord().m_iRemainInactivatedCharacterCount;
	}
	else
		m_LevelBossInfo.m_bIsEnable = false;


	// compute score/star for this level ==> score of stars now loaded from config file
	/*m_LevelConfig.m_ScoreOfStars[0] = GetBonusScoreForUnlockMainWord(true);
	int iTotalSubWordScore = 0;

	// NOTE: temporary hardcode
	if (m_pGameWordManager->GetSubWordCount() > 0)
	{
		for(int i=0; i< m_pGameWordManager->GetSubWordCount(); i++)
			iTotalSubWordScore += GetBonusScoreForUnlockSubWord(i, true);
	}
	else
		iTotalSubWordScore = m_LevelConfig.m_ScoreOfStars[0]/1.5f;

	m_LevelConfig.m_ScoreOfStars[1] = m_LevelConfig.m_ScoreOfStars[0] + iTotalSubWordScore /2;
	m_LevelConfig.m_ScoreOfStars[2] = m_LevelConfig.m_ScoreOfStars[0] + iTotalSubWordScore;*/
}

void NewGameBoardManager::ClearObstacleBlockID(const int& iObstacleBlockID)
{
	int iRow, iColumn;
	for(iRow=0; iRow < m_iRowNumber; iRow++)
		for(iColumn=0; iColumn < m_iColumnNumber; iColumn++)
			if (m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID == iObstacleBlockID)
			{
				m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID = -1;
				return;	
			}
}

bool NewGameBoardManager::IsRowLocked(const int& iRow)
{
	for(int iColumn=0; iColumn < m_iColumnNumber; iColumn++)
		if (m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID >= 0)
			if (m_pObstacleProcessManager->IsRowLocked( m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID))			
				return true;
	return false;
}

bool NewGameBoardManager::IsColumnLocked(const int& iColumn)
{
	for(int iRow=0; iRow < m_iRowNumber; iRow++)
		if (m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID >= 0)
			if (m_pObstacleProcessManager->IsColumnLocked( m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID))			
				return true;
	return false;
}

bool NewGameBoardManager::CanComboBeUpgraded(GemComboType_e eComboType)
{
	if (eComboType >= _GCT_COMBO4_ && eComboType <= _GCT_COMBO6_)
		return true;

	return false;
}

GemComboType_e NewGameBoardManager::UpgradeCombo(GemComboType_e eComboType)
{
	switch( eComboType)
	{
		default:
		case _GCT_COMBO4_:
			return _GCT_COMBO4_2_;
		case _GCT_COMBO5_:
			return _GCT_COMBO5_2_;
		case _GCT_COMBO6_:
			return _GCT_COMBO6_2_;
	}
}

bool NewGameBoardManager::RecheckAfterMoveV2(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboCreationInfo>& newDoubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList, std::vector<ComboEffectBundle*>& triggeredCombo6ChainList,
		std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<NewCellInfo>& unlockedLetterCells,
		std::vector<NewCellInfo>& newCells, bool bIsNewMove)
{
	// reset
	m_iLinkedBlockCount = 0;
	m_UnlockedGemLetterCellList.clear();
	m_bIsBossStateChanged = false;
	m_LevelBossInfo.m_bJustReleaseALetter = false;

	int iRow, iColumn, iGemID;

	// if this phase trigger only by waiting combo 6 then not need to create temporary list
	if (!(iSelectedRow < 0 && iSelectedColumn < 0 && m_WaitingTriggerCombo6List.size()>0))
	{
		// create temporary list to compute result for shifting move
		CopyDataToTempBoardMatrixAndResetFlags( iSelectedRow, iSelectedColumn, iDeltaRow, iDeltaColumn);
	}

	if (iSelectedRow >=0 || iSelectedColumn >= 0)
	{
		// ******** note: temporary hardcode and fix condition for combo 6 ************* 					
		if (iSelectedRow >= 0)
			for(iColumn =0; iColumn < m_iColumnNumber; iColumn++)
			{
				if (m_TemporaryValueMatrix[iSelectedRow][iColumn].m_eGemComboType == _GCT_COMBO6_
					&& ( (iColumn>0 &&  m_TemporaryValueMatrix[iSelectedRow][iColumn-1].m_bIsBlankCell == false) ||
					(iColumn< m_iColumnNumber-1 &&  m_TemporaryValueMatrix[iSelectedRow][iColumn+1].m_bIsBlankCell == false)))
				{										
					ComboEffectDescription comboEffectDescription;
					comboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_TemporaryValueMatrix[iSelectedRow][iColumn].m_eGemComboType,  iSelectedRow, iSelectedColumn); // _CET_EXPLOSION_;
					comboEffectDescription.m_Position = Cell(iSelectedRow, iColumn);
					comboEffectDescription.m_iGemID = m_TemporaryValueMatrix[iSelectedRow][iColumn].m_iGemID;					
					m_WaitingTriggerCombo6List.push_back(comboEffectDescription);

					m_TemporaryValueMatrix[iSelectedRow][iColumn].m_eGemComboType = _GCT_COMBO6_WAITING_TRIGGER_;
				}
			}
		else if (iSelectedColumn >= 0)
			for(iRow =0; iRow < m_iRowNumber; iRow++)
			{
				if (m_TemporaryValueMatrix[iRow][iSelectedColumn].m_eGemComboType == _GCT_COMBO6_
					&& ( (iRow>0 &&  m_TemporaryValueMatrix[iRow-1][iSelectedColumn].m_bIsBlankCell == false) ||
					(iRow< m_iRowNumber-1 &&  m_TemporaryValueMatrix[iRow+1][iSelectedColumn].m_bIsBlankCell == false)))
				{						
					ComboEffectDescription comboEffectDescription;
					comboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_TemporaryValueMatrix[iRow][iSelectedColumn].m_eGemComboType, iSelectedRow, iSelectedColumn); // _CET_EXPLOSION_;
					comboEffectDescription.m_Position = Cell(iRow, iSelectedColumn);
					comboEffectDescription.m_iGemID = m_TemporaryValueMatrix[iRow][iSelectedColumn].m_iGemID;					
					m_WaitingTriggerCombo6List.push_back(comboEffectDescription);

					m_TemporaryValueMatrix[iRow][iSelectedColumn].m_eGemComboType = _GCT_COMBO6_WAITING_TRIGGER_;
				}
			}

		if (m_WaitingTriggerCombo6List.size() > 0)
		{		
			memcpy( m_BoardValueMatrix, m_TemporaryValueMatrix, sizeof(m_TemporaryValueMatrix));		
		}

	}

	// activate marked combo 5 from last check
	//if (iSelectedRow < 0 && iSelectedColumn < 0 &&)
	if (m_WaitingTriggerCombo6List.size()>0)
	{
		TriggerWaitingCombo6List(comboChainList, triggeredCombo6ChainList);

		// create combo cells	
		CreateComboCells( iSelectedRow, iSelectedColumn, basicMatchingDestroyedCells, newComboCells);

		// create unlocked gems with letter
		CreateUnlockedGemLetterFromWaitingList(unlockedLetterCells);

		// calculate move cells and create new cells
		CalculateMoveCells( originalMovedCells, targetMovedCells);

		// generate new cells
		GenerateNewGems(newCells, bIsNewMove);

		return true;
	}


	//std::vector<Cell> basicMatchingDestroyedCells;
	
	int iSameValueCellCount;
	// bool bIsValidMove = false;

	// create temporary list to compute result for shifting move
	//CopyDataToTempBoardMatrixAndResetFlags( iSelectedRow, iSelectedColumn, iDeltaRow, iDeltaColumn);

	// ************* find and create double combo ****************************************************	
	//std::vector<DoubleComboCreationInfo> newDoubleComboList;
	/*
	for(iRow=0; iRow < m_iRowNumber; iRow++)
	{
		for(iColumn=0; iColumn < m_iColumnNumber; iColumn++)
		{
			if ( CanComboBeUpgraded(m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType))
			{
				bool bFoundDoubleCombo = false;
				Cell nextCell;			
				if (iRow < m_iRowNumber-1 && m_TemporaryValueMatrix[iRow+1][iColumn].m_eGemComboType == m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType)
				{
					bFoundDoubleCombo = true;
					nextCell.m_iRow = iRow + 1;
					nextCell.m_iColumn = iColumn;
				}
				else if (iColumn < m_iColumnNumber-1 && m_TemporaryValueMatrix[iRow][iColumn+1].m_eGemComboType == m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType)
				{
					bFoundDoubleCombo = true;
					nextCell.m_iRow = iRow ;
					nextCell.m_iColumn = iColumn + 1;
				}

				if (bFoundDoubleCombo)
				{
					DoubleComboCreationInfo doubleCombo;
					doubleCombo.m_Cell1 = Cell(iRow, iColumn);
					doubleCombo.m_Cell2 = nextCell;
					doubleCombo.m_Position = doubleCombo.m_Cell1;
					doubleCombo.m_eComboType = UpgradeCombo(m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType);

					newDoubleComboList.push_back(doubleCombo);

					// disable 2 combo cell
					m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;
					m_TemporaryValueMatrix[iRow][iColumn].m_iGemID = -1;
					m_TemporaryValueMatrix[nextCell.m_iRow][nextCell.m_iColumn].m_eGemComboType = _GCT_NONE_;
					m_TemporaryValueMatrix[nextCell.m_iRow][nextCell.m_iColumn].m_iGemID = -1;

					// recheck order of 2 cells
					if (nextCell.m_iRow == iSelectedRow || nextCell.m_iColumn == iSelectedColumn)
					{						
						doubleCombo.m_Position = doubleCombo.m_Cell2;
					}

					// create double combo cell
					//m_TemporaryValueMatrix[doubleCombo.m_Cell1.m_iRow][doubleCombo.m_Cell1.m_iColumn].m_eGemComboType = doubleCombo.m_eComboType;

				}
			}
		}
	}*/
	

	// ************* create block for basic matching ******************************************
	bool bIsNewBlockCreated;
	CreateBlockForBasicMatching( bIsNewBlockCreated, basicMatchingDestroyedCells);

	// **********************************
	if (m_iLinkedBlockCount > 0 || newDoubleComboList.size() > 0)
	{
		// for test
		//CountBasicCombo();

		// Update main board matrix from temporary matrix if it's valid move
		memcpy( m_BoardValueMatrix, m_TemporaryValueMatrix, sizeof(m_TemporaryValueMatrix));
		//if ( iSelectedRow >=0 || iSelectedColumn >= 0)		
		/*{
			int iRow, iColumn;

			for(iRow = 0; iRow < m_iRowNumber; iRow++)
				for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
					m_BoardValueMatrix[iRow][iColumn] = m_TemporaryValueMatrix[iRow][iColumn];
		}*/

		// remove cells by basic matching and create first combo chain		
		RemoveCellsByBasicMatching( basicMatchingDestroyedCells, comboChainList, iSelectedRow, iSelectedColumn);
	
		// add effect of double combo to chain
		//comboChainList.insert(comboChainList.end(), chainEffectFromDoubleCombo.begin(), chainEffectFromDoubleCombo.end());		

		// trigger combo chain from effect of triggered combos
		ExecuteComboChain(comboChainList);

		// create double combo cells
		for(auto doubleComboCreation : newDoubleComboList)
		{
			m_BoardValueMatrix[doubleComboCreation.m_Position.m_iRow][doubleComboCreation.m_Position.m_iColumn].m_eGemComboType = doubleComboCreation.m_eComboType;
			m_BoardValueMatrix[doubleComboCreation.m_Position.m_iRow][doubleComboCreation.m_Position.m_iColumn].m_iGemID = _SPECIAL_COMBO_GEM_ID;
		}

		// create combo cells	
		CreateComboCells( iSelectedRow, iSelectedColumn, basicMatchingDestroyedCells, newComboCells);

		// create unlocked gems with letter
		CreateUnlockedGemLetterFromWaitingList(unlockedLetterCells);

		// calculate move cells and create new cells
		CalculateMoveCells( originalMovedCells, targetMovedCells);

		// generate new cells
		GenerateNewGems(newCells, bIsNewMove);

		return true;
	}

	return false;
}


bool NewGameBoardManager::FastCheckBlocks( int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& basicMatchingDestroyedCells)
{
	// create temporary list to compute result for shifting move
	CopyDataToTempBoardMatrixAndResetFlags( iSelectedRow, iSelectedColumn, iDeltaRow, iDeltaColumn);

	// checking
	int iRow, iColumn, iGemID, iSameValueCellCount;
	Cell currentCheckCell, loopCell( iSelectedRow, iSelectedColumn);
	iRow = iSelectedRow;
	iColumn = iSelectedColumn;
	int iLimitRow = iSelectedRow, iLimitColumn = iSelectedColumn;

	if (iSelectedRow > 0)
	{		
		if (iDeltaColumn < 0)
		{
			iDeltaRow = 0;
			iDeltaColumn = -1;
			iColumn = m_iColumnNumber-1;
			iLimitColumn = -1;
		}
		else		
		{
			iDeltaRow = 0;
			iDeltaColumn = 1;
			iColumn = 0;
			iLimitColumn = m_iColumnNumber;
		}
	}
	else// if (iSelectedColumn > 0)
	{		
		if (iDeltaRow < 0)
		{
			iDeltaRow = -1;
			iDeltaColumn = 0;
			iRow = m_iRowNumber-1;
			iLimitRow= -1;
		}
		else		
		{
			iDeltaRow = 1;
			iDeltaColumn = 0;
			iRow = 0;
			iLimitRow = m_iRowNumber;
		}
	}

	bool bIsNewBlockCreated;

	while (iRow != iLimitRow || iColumn != iLimitColumn)
	{	
		if (!m_TemporaryValueMatrix[iRow][iColumn].m_bIsBlankCell && m_TemporaryValueMatrix[iRow][iColumn].m_iGemID != _SPECIAL_COMBO_GEM_ID) //not blank cell
		{
			currentCheckCell.m_iRow = iRow;
			currentCheckCell.m_iColumn = iColumn;				
			int iCurrentDestroyedCellIndex = basicMatchingDestroyedCells.size();
			
			bIsNewBlockCreated = true;

			while (1)
			{				
				//bStopBlockLoop = true;
				iGemID =  m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemID;			

				// horizontal check
				if( !m_CheckedHorizotalMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
				{				
					m_CheckedHorizotalMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;
					iSameValueCellCount = 1;						
			
					int iRowBelow = currentCheckCell.m_iRow - 1;
					while (iRowBelow >=0 && m_TemporaryValueMatrix[iRowBelow][currentCheckCell.m_iColumn].m_iGemID == iGemID)
					{
						m_CheckedHorizotalMatrix[iRowBelow][currentCheckCell.m_iColumn] = true;

						iSameValueCellCount++;
						iRowBelow--;				
					}

					int iRowAbove = currentCheckCell.m_iRow + 1;
					while (iRowAbove < m_iRowNumber && m_TemporaryValueMatrix[iRowAbove][currentCheckCell.m_iColumn].m_iGemID == iGemID)
					{
						m_CheckedHorizotalMatrix[iRowAbove][currentCheckCell.m_iColumn] = true;

						iSameValueCellCount++;
						iRowAbove++;				
					}

					if (iSameValueCellCount >= 3)
					{
						if (bIsNewBlockCreated)
						{							
							bIsNewBlockCreated = false;								
							
							// add current cell to list only when new block is created
							if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
							{
								m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;

								basicMatchingDestroyedCells.push_back(Cell(currentCheckCell.m_iRow, currentCheckCell.m_iColumn));												

								iCurrentDestroyedCellIndex++;
							}
						}

						for(int iRowIndex = currentCheckCell.m_iRow - 1; iRowIndex > iRowBelow; iRowIndex--)
						{
							if (!m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn])
							{
								m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn] = true;

								basicMatchingDestroyedCells.push_back(Cell(iRowIndex, currentCheckCell.m_iColumn));								
							}
						}				
																	
						for(int iRowIndex = currentCheckCell.m_iRow + 1; iRowIndex < iRowAbove; iRowIndex++)
						{
							if (!m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn])
							{
								m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn] = true;

								basicMatchingDestroyedCells.push_back(Cell(iRowIndex, currentCheckCell.m_iColumn));								
							}
						}				
					}
				} //end horizontal check

				// vertical check					
				if( !m_CheckedVerticalMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
				{
					m_CheckedVerticalMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;
					iSameValueCellCount = 1;

					int iColumnLeft = currentCheckCell.m_iColumn - 1;
					while (iColumnLeft >=0 && m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnLeft].m_iGemID == iGemID)
					{
						m_CheckedVerticalMatrix[currentCheckCell.m_iRow][iColumnLeft] = true;

						iSameValueCellCount++;
						iColumnLeft--;
					}

					int iColumnRight = currentCheckCell.m_iColumn + 1;
					while (iColumnRight < m_iColumnNumber && m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnRight].m_iGemID == iGemID)
					{
						m_CheckedVerticalMatrix[currentCheckCell.m_iRow][iColumnRight] = true;

						iSameValueCellCount++;
						iColumnRight++;
					}

					if (iSameValueCellCount >= 3)
					{
						if (bIsNewBlockCreated)
						{								
							bIsNewBlockCreated = false;								

							// add current cell to list only when new block is created
							if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
							{
								m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;

								basicMatchingDestroyedCells.push_back(Cell(currentCheckCell.m_iRow, currentCheckCell.m_iColumn));				

								iCurrentDestroyedCellIndex++;
							}
						}

						// calculate destroyed cells							
						for(int iColumnIndex= currentCheckCell.m_iColumn- 1; iColumnIndex > iColumnLeft; iColumnIndex--)
						{
							if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][ iColumnIndex])
							{
								basicMatchingDestroyedCells.push_back(Cell(currentCheckCell.m_iRow, iColumnIndex));									
							}
						}

						for(int iColumnIndex= currentCheckCell.m_iColumn+ 1; iColumnIndex <iColumnRight; iColumnIndex++)
						{
							if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][ iColumnIndex])
							{
								basicMatchingDestroyedCells.push_back(Cell(currentCheckCell.m_iRow, iColumnIndex));								
							}
						}
					}
				} //end vertical check			

				if (iCurrentDestroyedCellIndex == basicMatchingDestroyedCells.size()) //no new cells added to list ==> stop
				{
					break;
				}
				else{
					currentCheckCell = basicMatchingDestroyedCells[iCurrentDestroyedCellIndex];
					iCurrentDestroyedCellIndex++;
				}
			}
		}

		iRow += iDeltaRow;
		iColumn += iDeltaColumn;
	}

	if (basicMatchingDestroyedCells.size() > 0)
		return true;
	else
		return false;

}

bool NewGameBoardManager::DestroySingleCellUtil(const int& iRow, const int& iColumn, const int& iPhaseIndex)
{
	// combo 5 cell wil be activated/destroyed later
	if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_COMBO6_)
	{
		if (m_BoardValueMatrix[iRow][iColumn].m_iGemLetterBlockID >= 0)
		{			
			m_UnlockedGemLetterCellList.push_back(NewCellInfo(iRow, iColumn, m_BoardValueMatrix[iRow][iColumn].m_iGemLetterBlockID, true, m_BoardValueMatrix[iRow][iColumn].m_iGemID));
			m_BoardValueMatrix[iRow][iColumn].Reset();
			return false;
		}

		if (m_LevelBossInfo.m_bIsEnable && !m_bIsBossStateChanged)
		{
			if ( (iRow >= m_pLevelConfig->m_BossConfig.m_Position.m_iRow -1 && iRow <= m_pLevelConfig->m_BossConfig.m_Position.m_iRow + m_pLevelConfig->m_BossConfig.m_iHeight)
				&& (iColumn >= m_pLevelConfig->m_BossConfig.m_Position.m_iColumn -1 && iColumn <= m_pLevelConfig->m_BossConfig.m_Position.m_iColumn + m_pLevelConfig->m_BossConfig.m_iWidth))
			{
				CCLOG("Current HP:%d, current value of bJustReleaseALetter:%d", m_LevelBossInfo.m_iCurrentHitPoint, m_LevelBossInfo.m_bJustReleaseALetter?1:0);

				m_bIsBossStateChanged = true;
				m_LevelBossInfo.m_iCurrentHitPoint--;

				if (m_LevelBossInfo.m_iCurrentHitPoint == 0)
				{
					m_LevelBossInfo.m_bJustReleaseALetter = true;
					m_LevelBossInfo.m_iRemainLettersCount--;
					m_LevelBossInfo.m_iBossStateChangePhase = iPhaseIndex;

					if (m_LevelBossInfo.m_iRemainLettersCount > 0)
						m_LevelBossInfo.m_iCurrentHitPoint = m_pLevelConfig->m_BossConfig.m_HitPointPerLetter;										
				}				
			}
		}

		m_BoardValueMatrix[iRow][iColumn].Reset(); //m_iGemID = -1;	m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;
		return true;		
	}
	else
	{
		m_BoardValueMatrix[iRow][iColumn].m_eGemComboType =	_GCT_COMBO6_WAITING_TRIGGER_;						
		return false;
	}		
}

void NewGameBoardManager::CalculateMoveEffectOnBoss(bool& bIsBossStateChanged)
{

}

void NewGameBoardManager::ExecuteComboChain(std::vector<ComboEffectBundle*>& comboChainList)
{
	int iComboIndexInChain = 0;
	ComboEffectBundle* pComboInChain;
	while (iComboIndexInChain < comboChainList.size())
	{
		pComboInChain = comboChainList.at(iComboIndexInChain);
		Cell position = pComboInChain->m_ComboEffectDescription.m_Position;

		if ((pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_ROW_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLUMN_)
		|| (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_EXPLOSION_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DOUBLE_EXPLOSION_))
		{
			// demo explosion effects
			int iRowDeltaD, iRowDeltaA, iColumnDeltaD, iColumnDeltaA;
			
			if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_ROW_)
			{
				iRowDeltaD = iRowDeltaA = 0;
				iColumnDeltaD = -position.m_iColumn;
				iColumnDeltaA = iColumnDeltaD + m_iColumnNumber - 1;
			}
			else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLUMN_)
			{
				iColumnDeltaD = iColumnDeltaA = 0;
				iRowDeltaD = -position.m_iRow;
				iRowDeltaA = iRowDeltaD + m_iRowNumber -1;				
			}
			else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DOUBLE_EXPLOSION_)
			{
				iRowDeltaD = iColumnDeltaD = -2;
				iRowDeltaA = iColumnDeltaA = 2;
			}
			else // EXPLOSION
			{
				iRowDeltaD = iColumnDeltaD = -1;
				iRowDeltaA = iColumnDeltaA = 1;
			}

			// trigger explostion effect: temporary implementation
			for(int iRow = position.m_iRow+iRowDeltaD; iRow <= position.m_iRow+iRowDeltaA; iRow++)
				for(int iColumn = position.m_iColumn+iColumnDeltaD; iColumn <= position.m_iColumn+iColumnDeltaA; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && IsCellDestroyable(iRow, iColumn))				
				{
					if (m_pObstacleProcessManager->DestroyCellWithObstacle(m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID))
					{				
						// this may trigger another combo ?
						if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType >=0 && m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_NONE_)						
						{
							int iRandomDirection = rand() % 2;

							ComboEffectBundle* pNextTriggeredCombo = new ComboEffectBundle();
							pNextTriggeredCombo->m_ComboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_BoardValueMatrix[iRow][iColumn].m_eGemComboType, -1 * iRandomDirection, -1 * (1-iRandomDirection)); // _CET_EXPLOSION_;
							pNextTriggeredCombo->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
							pNextTriggeredCombo->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
							pNextTriggeredCombo->m_iNormalChainPhase =  pComboInChain->m_iNormalChainPhase + 1;

							//pComboInChain->m_TriggeredComboEfectBundleList.push_back(pNextTriggeredCombo);
								
							// add it to chain to trigger later
							comboChainList.push_back(pNextTriggeredCombo);
						}		

						// combo 5 cell wil be activated/destroyed later
						int iObstacleBlockID = m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID;
						if (DestroySingleCellUtil(iRow, iColumn, pComboInChain->m_iNormalChainPhase))
						{							
							pComboInChain->m_DestroyedCells.push_back(DestroyedByComboCell(iRow, iColumn));
						}						
					}
				}
			}	
		}
		else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_ROW_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_COLUMN_)
		{
			pComboInChain->m_iNormalChainPhase = -1; //trigger later
			m_WaitingTriggerCombo6List.push_back(pComboInChain->m_ComboEffectDescription);
			/*

			int iGemID = pComboInChain->m_ComboEffectDescription.m_iGemID;
			for(int iRow = 0; iRow <= m_iRowNumber; iRow++)
				for(int iColumn = 0; iColumn <= m_iColumnNumber; iColumn++)
			{			
				if ( !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && m_BoardValueMatrix[iRow][iColumn].m_iGemID == iGemID)
				{				
					// this may trigger another combo ?
					if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType >=0 && m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_NONE_)
					{
						ComboEffectBundle* pNextTriggeredCombo = new ComboEffectBundle();
						pNextTriggeredCombo->m_ComboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_BoardValueMatrix[iRow][iColumn].m_eGemComboType); // _CET_EXPLOSION_;
						pNextTriggeredCombo->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
						pNextTriggeredCombo->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
						pNextTriggeredCombo->m_iPhase =  pComboInChain->m_iPhase + 1;

						pComboInChain->m_TriggeredComboEfectBundleList.push_back(pNextTriggeredCombo);
								
						// add it to chain to trigger later
						comboChainList.push_back(pNextTriggeredCombo);
					}		

					m_BoardValueMatrix[iRow][iColumn].Reset(); //m_iGemID = -1;	m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;

					pComboInChain->m_DestroyedCells.push_back(Cell(iRow, iColumn));
				}
			}	*/
		}
		iComboIndexInChain++;
	}
}

void NewGameBoardManager::TriggerWaitingCombo6List(std::vector<ComboEffectBundle*>& comboChainList, std::vector<ComboEffectBundle*>& triggeredCombo6ChainList)
{	
	int iComboIndex = 0;
	ComboEffectDescription comboDescription;
	//std::vector<ComboEffectDescription> nextWaitingTriggerCombo5List;
	//std::vector<ComboEffectBundle*> nextComboChainList;

	while (iComboIndex < m_WaitingTriggerCombo6List.size())
	{
		//pComboInChain = comboChainList.at(iComboIndexInChain);
		comboDescription = m_WaitingTriggerCombo6List.at(iComboIndex);

		Cell position = comboDescription.m_Position; //pComboInChain->m_ComboEffectDescription.m_Position;

		//if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_)					
		ComboEffectBundle* pTriggeredCombo = new ComboEffectBundle();
		pTriggeredCombo->m_ComboEffectDescription = comboDescription;
		pTriggeredCombo->m_iActivatedByCombo6Phase = iComboIndex;	
		triggeredCombo6ChainList.push_back(pTriggeredCombo);

		int iGemID1=-1, iGemID2=-1;
		Cell cell1(-1, -1), cell2(-2, -2);
		if (comboDescription.m_eComboEffectType == _CET_DESTROY_COLOR_COLUMN_)
		{
			if (comboDescription.m_Position.m_iColumn > 0 && m_BoardValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn-1].m_bIsBlankCell == false)
			{
				iGemID1 = m_BoardValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn-1].m_iGemID;				
				cell1 = Cell(comboDescription.m_Position.m_iRow, comboDescription.m_Position.m_iColumn-1);
			}

			if (comboDescription.m_Position.m_iColumn < m_iColumnNumber-1 && m_BoardValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn+1].m_bIsBlankCell == false)
			{
				iGemID2 = m_BoardValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn+1].m_iGemID;
				cell2 = Cell(comboDescription.m_Position.m_iRow, comboDescription.m_Position.m_iColumn+1);
			}
		}
		else //row
		{
			if (comboDescription.m_Position.m_iRow > 0 && m_BoardValueMatrix[comboDescription.m_Position.m_iRow-1][comboDescription.m_Position.m_iColumn].m_bIsBlankCell == false)
			{
				iGemID1 = m_BoardValueMatrix[comboDescription.m_Position.m_iRow-1][comboDescription.m_Position.m_iColumn].m_iGemID;
				cell1 = Cell(comboDescription.m_Position.m_iRow-1, comboDescription.m_Position.m_iColumn);
			}

			if (comboDescription.m_Position.m_iRow < m_iRowNumber-1 && m_BoardValueMatrix[comboDescription.m_Position.m_iRow+1][comboDescription.m_Position.m_iColumn].m_bIsBlankCell == false)
			{
				iGemID2 = m_BoardValueMatrix[comboDescription.m_Position.m_iRow+1][comboDescription.m_Position.m_iColumn].m_iGemID;
				cell2 = Cell(comboDescription.m_Position.m_iRow+1, comboDescription.m_Position.m_iColumn);
			}
		}
		

		//int iGemID = comboDescription.m_iGemID;
		for(int iRow = 0; iRow <= m_iRowNumber; iRow++)
			for(int iColumn = 0; iColumn <= m_iColumnNumber; iColumn++)
		{			
			if ( !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && ( m_BoardValueMatrix[iRow][iColumn].m_iGemID == iGemID1 || m_BoardValueMatrix[iRow][iColumn].m_iGemID == iGemID2)
				&& IsCellDestroyable(iRow, iColumn))
				//&& m_BoardValueMatrix[iRow][iColumn].m_eGemComboType !=_GCT_COMBO5_WAITING_TRIGGER_)
				//if (m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID < 0 || m_pObstacleProcessManager->DestroyCell(m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID))
				if (m_pObstacleProcessManager->DestroyCellWithObstacle(m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID))
				{				
					// this may trigger another combo ?
					if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType >=0 && m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_NONE_)
					{
						int iRandomDirection = rand() % 2;

						ComboEffectBundle* pNextTriggeredCombo = new ComboEffectBundle();
						pNextTriggeredCombo->m_ComboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_BoardValueMatrix[iRow][iColumn].m_eGemComboType, -1 * iRandomDirection, -1 * (1-iRandomDirection)); // _CET_EXPLOSION_;						
						pNextTriggeredCombo->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
						pNextTriggeredCombo->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
						pNextTriggeredCombo->m_iActivatedByCombo6Phase =  pTriggeredCombo->m_iActivatedByCombo6Phase+1;

						//pTriggeredCombo->m_TriggeredComboEfectBundleList.push_back(pNextTriggeredCombo);
								
						// add it to chain to trigger later
						comboChainList.push_back(pNextTriggeredCombo);
					}		

					// combo 5 cell wil be activated/destroyed later
					int iObstacleBlockID = m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID;
					
					int iBackupGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
					bool bIsCompleteDestroyed = DestroySingleCellUtil(iRow, iColumn, pTriggeredCombo->m_iActivatedByCombo6Phase);
					DestroyedByComboCell destroyedCell(iRow, iColumn);
					destroyedCell.m_bIsCompleteDestroyed = bIsCompleteDestroyed;
					if (iBackupGemID == iGemID1)
					{
						destroyedCell.m_iGroupIndex = 0;
						if (cell1 == destroyedCell)
							destroyedCell.m_iDestroyPhaseIndex = 0;
						else
							destroyedCell.m_iDestroyPhaseIndex = 1;
					}
					else
					{
						destroyedCell.m_iGroupIndex = 1;
						if (cell2 == destroyedCell)
							destroyedCell.m_iDestroyPhaseIndex = 0;
						else
							destroyedCell.m_iDestroyPhaseIndex = 1;
					}
					pTriggeredCombo->m_DestroyedCells.push_back(destroyedCell);

					/*if (DestroySingleCellUtil(iRow, iColumn, pTriggeredCombo->m_iActivatedByCombo6Phase))
					{						
						pTriggeredCombo->m_DestroyedCells.push_back(DestroyedByComboCell(iRow, iColumn));
					}*/					
				}
		}

		// reset combo cell
		m_BoardValueMatrix[position.m_iRow][position.m_iColumn].Reset();
		DestroyedByComboCell destroyedCell(position.m_iRow, position.m_iColumn);
		destroyedCell.m_iDestroyPhaseIndex = -1;
		pTriggeredCombo->m_DestroyedCells.push_back(destroyedCell);

		// next
		iComboIndex++;
	}

	// active next chain
	m_WaitingTriggerCombo6List.clear();
	ExecuteComboChain( comboChainList);	
	//comboChainList.insert(comboChainList.end(), nextComboChainList.begin(), nextComboChainList.end());		
}


bool NewGameBoardManager::ExecuteEndGameBonus(
		std::vector<ComboEffectCell>& convertedToComboCells,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboCreationInfo>& doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList, std::vector<ComboEffectBundle*>& triggeredCombo5ChainList,
		std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<NewCellInfo>& unlockedLetterCells,
		std::vector<NewCellInfo>& newCells)
{
	// reset
	m_iLinkedBlockCount = 0;
	m_UnlockedGemLetterCellList.clear();

	// activate marked combo 5 from last check
	if (m_WaitingTriggerCombo6List.size()>0)
	{
		TriggerWaitingCombo6List(comboChainList, triggeredCombo5ChainList);

		// create combo cells	
		CreateComboCells( -1, -1, basicMatchingDestroyedCells, newComboCells);

		// create unlocked gems with letter
		CreateUnlockedGemLetterFromWaitingList(unlockedLetterCells);

		// calculate move cells and create new cells
		CalculateMoveCells( originalMovedCells, targetMovedCells);

		// generate new cells
		GenerateNewGems(newCells, false);

		return true;
	}



	int iRow, iColumn, iGemID;
	int iSameValueCellCount;
	// bool bIsValidMove = false;
	bool bHasExistingCombo = false;

	// create temporary list to compute result for shifting move
	CopyDataToTempBoardMatrixAndResetFlags( -1, -1, 0, 0);

	// convert some cells to combo4 based on remain move count
	std::vector<Cell> notComboCells;
	for(iRow = 0; iRow < m_iRowNumber; iRow++)
	{
		for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
		{
			if (!m_TemporaryValueMatrix[iRow][iColumn].m_bIsBlankCell && m_TemporaryValueMatrix[iRow][iColumn].m_iGemLetterBlockID < 0 && m_TemporaryValueMatrix[iRow][iColumn].m_iObstacleBlockID < 0
				&& m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType == _GCT_NONE_)
				notComboCells.push_back(Cell(iRow, iColumn));
		}
	}

	int iRandomIndex;
	int iMaxConvertedCell = MIN(m_iCurrentMove, 5);
	m_iCurrentMove -= iMaxConvertedCell;

	for(int i=0; i< iMaxConvertedCell; i++)
	{
		if (notComboCells.size() == 0)
			break;

		iRandomIndex = rand() % notComboCells.size();
		m_TemporaryValueMatrix[notComboCells[iRandomIndex].m_iRow][notComboCells[iRandomIndex].m_iColumn].m_eGemComboType = _GCT_COMBO4_;
		// if combo not has color, remove its color here
		
		ComboEffectCell comboCell(notComboCells[iRandomIndex], _GCT_COMBO4_, m_TemporaryValueMatrix[notComboCells[iRandomIndex].m_iRow][notComboCells[iRandomIndex].m_iColumn].m_iGemID);
		convertedToComboCells.push_back(comboCell);
		
		notComboCells[iRandomIndex] = notComboCells[notComboCells.size()-1];
		notComboCells.pop_back();
	}

	// ************* create double combo ****************************************************	
	for(iRow=0; iRow < m_iRowNumber; iRow++)
	{
		for(iColumn=0; iColumn < m_iColumnNumber; iColumn++)
		{
			if (m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_NONE_)
				bHasExistingCombo = true;

			/*if ( CanComboBeUpgraded(m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType))
			{
				bool bFoundDoubleCombo = false;
				Cell nextCell;			
				if (iRow < m_iRowNumber-1 && m_TemporaryValueMatrix[iRow+1][iColumn].m_eGemComboType == m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType)
				{
					bFoundDoubleCombo = true;
					nextCell.m_iRow = iRow + 1;
					nextCell.m_iColumn = iColumn;
				}
				else if (iColumn < m_iColumnNumber-1 && m_TemporaryValueMatrix[iRow][iColumn+1].m_eGemComboType == m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType)
				{
					bFoundDoubleCombo = true;
					nextCell.m_iRow = iRow ;
					nextCell.m_iColumn = iColumn + 1;
				}

				if (bFoundDoubleCombo)
				{
					DoubleComboCreationInfo doubleCombo;
					doubleCombo.m_Cell1 = Cell(iRow, iColumn);
					doubleCombo.m_Cell2 = nextCell;
					doubleCombo.m_Position = doubleCombo.m_Cell1;
					doubleCombo.m_eComboType = UpgradeCombo(m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType);

					doubleComboList.push_back(doubleCombo);

					// disable 2 combo cell
					m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;
					m_TemporaryValueMatrix[iRow][iColumn].m_iGemID = -1;
					m_TemporaryValueMatrix[nextCell.m_iRow][nextCell.m_iColumn].m_eGemComboType = _GCT_NONE_;
					m_TemporaryValueMatrix[nextCell.m_iRow][nextCell.m_iColumn].m_iGemID = -1;					

					// create double combo cell
					//m_TemporaryValueMatrix[doubleCombo.m_Cell1.m_iRow][doubleCombo.m_Cell1.m_iColumn].m_eGemComboType = doubleCombo.m_eComboType;

				}
			}*/
		}
	}
	
	// ************* create block for basic matching ******************************************
	bool bIsNewBlockCreated;
	CreateBlockForBasicMatching( bIsNewBlockCreated, basicMatchingDestroyedCells);

	// ********************* 
	if (m_iLinkedBlockCount > 0 || doubleComboList.size() > 0 || convertedToComboCells.size()>0 || bHasExistingCombo)
	{
		// for test
		//CountBasicCombo();

		// Update main board matrix from temporary matrix if it's valid move
		//if ( iSelectedRow >=0 || iSelectedColumn >= 0)		
		{
			int iRow, iColumn;

			for(iRow = 0; iRow < m_iRowNumber; iRow++)
				for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
					m_BoardValueMatrix[iRow][iColumn] = m_TemporaryValueMatrix[iRow][iColumn];
		}
		
		// add effect of double combo to first of chain
		//comboChainList.insert(comboChainList.begin(), chainEffectFromDoubleCombo.begin(), chainEffectFromDoubleCombo.end());

		// remove cells by basic matching and create first combo chain
		RemoveCellsByBasicMatching( basicMatchingDestroyedCells, comboChainList, 0, 0);
		

		// add converted combo from bonus and existing combo to list
		for(auto cell: convertedToComboCells)
		{
			if (m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType == _GCT_COMBO4_) //!= _GCT_NONE_)
			{
				ComboEffectBundle* pTriggeredComboEffectBundle = new ComboEffectBundle();
				pTriggeredComboEffectBundle->m_ComboEffectDescription.m_eComboEffectType = _CET_EXPLOSION_; //only implement explosion now
				pTriggeredComboEffectBundle->m_ComboEffectDescription.m_Position = cell;
				pTriggeredComboEffectBundle->m_ComboEffectDescription.m_iGemID = m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID;
				pTriggeredComboEffectBundle->m_iNormalChainPhase = comboChainList.size(); //0;														

				comboChainList.push_back(pTriggeredComboEffectBundle);
			}
		}

		if (convertedToComboCells.size() == 0  && doubleComboList.size()==0 && bHasExistingCombo)
		{
			for(iRow = 0; iRow < m_iRowNumber; iRow++)
				for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
					if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_NONE_)
					{
						int iRandomDirection = rand() % 2;

						ComboEffectBundle* pTriggeredComboEffectBundle = new ComboEffectBundle();
						pTriggeredComboEffectBundle->m_ComboEffectDescription.m_eComboEffectType = GetComboEffectTypeFromComboType(m_BoardValueMatrix[iRow][iColumn].m_eGemComboType, -1 * iRandomDirection, -1 * (1-iRandomDirection)); // _CET_EXPLOSION_;
							//_CET_EXPLOSION_; //only implement explosion now
						pTriggeredComboEffectBundle->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
						pTriggeredComboEffectBundle->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
						pTriggeredComboEffectBundle->m_iNormalChainPhase = comboChainList.size(); //0;														

						comboChainList.push_back(pTriggeredComboEffectBundle);
					}
		}

		// trigger combo chain from effect of triggered combos
		ExecuteComboChain(comboChainList);

		// create combo cells	
		CreateComboCells(-1, -1, basicMatchingDestroyedCells, newComboCells);

		// create unlocked gems with letter
		CreateUnlockedGemLetterFromWaitingList(unlockedLetterCells);

		// calculate move cells and create new cells
		CalculateMoveCells( originalMovedCells, targetMovedCells);

		// generate new cells   
		GenerateNewGems(newCells, false);

		return true;
	}

	return false;
}

void NewGameBoardManager::CopyDataToTempBoardMatrixAndResetFlags(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn)
{
	int iRow, iColumn;

	memset( m_CheckedHorizotalMatrix, 0, sizeof( m_CheckedHorizotalMatrix));
	memset( m_CheckedVerticalMatrix, 0, sizeof( m_CheckedVerticalMatrix));
	memset( m_FlagDestroyedMatrix, 0, sizeof( m_FlagDestroyedMatrix));

	if (iSelectedRow < 0 && iSelectedColumn < 0)
	{
		memcpy( m_TemporaryValueMatrix, m_BoardValueMatrix, sizeof(m_BoardValueMatrix));		

		/*for(iRow = 0; iRow < m_iRowNumber; iRow++)
		{
			for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
			{
				m_CheckedHorizotalMatrix[iRow][iColumn] = false;
				m_CheckedVerticalMatrix[iRow][iColumn] = false;
				m_FlagDestroyedMatrix[iRow][iColumn] = false;
				m_TemporaryValueMatrix[iRow][iColumn] = m_BoardValueMatrix[iRow][iColumn];					
			}
		}*/
	}
	else
	{
		for(iRow = 0; iRow < m_iRowNumber; iRow++)
		{
			for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
			{
				//m_CheckedHorizotalMatrix[iRow][iColumn] = false;
				//m_CheckedVerticalMatrix[iRow][iColumn] = false;
				//m_FlagDestroyedMatrix[iRow][iColumn] = false;

				if (!m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell)
				{
					if (iRow == iSelectedRow)
					{	
						//int iBlankCellCount = 0; //blank cell on the move
						int iTranslationCell = 0;
						int iSign = 1;

						if (iDeltaColumn != 0)
						{
							iSign = iDeltaColumn/abs(iDeltaColumn);
							iTranslationCell = 0;
							for(int iStep=1; iStep<= abs(iDeltaColumn); iStep++)
							{
								iTranslationCell += iSign;
								if (iTranslationCell < 0)
									iTranslationCell+= m_iColumnNumber;

								if (m_BoardValueMatrix[iRow][(iColumn + iTranslationCell)%m_iColumnNumber].m_bIsBlankCell)
									iStep--;
							}
					
						}

						m_TemporaryValueMatrix[iRow][(iColumn + iTranslationCell)%m_iColumnNumber] = m_BoardValueMatrix[iRow][iColumn];
					}
					else if (iColumn == iSelectedColumn)
					{
						//int iBlankCellCount = 0; //blank cell on the move
						int iTranslationCell = 0;
						int iSign = 1;
						if (iDeltaRow != 0)
						{
							iSign = iDeltaRow/abs(iDeltaRow);									
							iTranslationCell = 0;
							for(int iStep=1; iStep<= abs(iDeltaRow); iStep++)
							{
								iTranslationCell += iSign;
								if (iTranslationCell < 0)
									iTranslationCell+= m_iRowNumber;
								if (m_BoardValueMatrix[(iRow + iTranslationCell) % m_iRowNumber][iColumn].m_bIsBlankCell)
									iStep--;
							}						
						}

						m_TemporaryValueMatrix[(iRow + iTranslationCell) % m_iRowNumber][iColumn] = m_BoardValueMatrix[iRow][iColumn];
					}
					else
						m_TemporaryValueMatrix[iRow][iColumn] = m_BoardValueMatrix[iRow][iColumn];
				}
				else{ //blank cell
					m_TemporaryValueMatrix[iRow][iColumn].m_bIsBlankCell = true;
					m_TemporaryValueMatrix[iRow][iColumn].m_iGemID = -1;
				}
			}
		}	
	}
}

void NewGameBoardManager::CreateBlockForBasicMatching(bool& bIsNewBlockCreated, std::vector<Cell>& basicMatchingDestroyedCells)
{
	for(int i=0; i< _MAX_LINKED_BLOCK_COUNT_; i++)
	{
		m_LinkedBlockList[i].m_iCellCount = 0;
	}	

	int iRow, iColumn, iGemID, iSameValueCellCount;
	Cell currentCheckCell;
	m_iLinkedBlockCount = 0;

	for(iRow=0; iRow < m_iRowNumber; iRow++)
	{
		for(iColumn=0; iColumn < m_iColumnNumber; iColumn++)
		{
			if (!m_TemporaryValueMatrix[iRow][iColumn].m_bIsBlankCell && m_TemporaryValueMatrix[iRow][iColumn].m_iGemID != _SPECIAL_COMBO_GEM_ID) //not blank cell
			{
				currentCheckCell.m_iRow = iRow;
				currentCheckCell.m_iColumn = iColumn;
				bIsNewBlockCreated = true;
				int iCurrentDestroyedCellIndex = basicMatchingDestroyedCells.size();
				//bool bStopBlockLoop;

				while (1)
				{				
					//bStopBlockLoop = true;
					iGemID =  m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemID;			

					// horizontal check
					if( !m_CheckedHorizotalMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
					{				
						m_CheckedHorizotalMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;
						iSameValueCellCount = 1;						
			
						int iRowBelow = currentCheckCell.m_iRow - 1;
						while (iRowBelow >=0 && m_TemporaryValueMatrix[iRowBelow][currentCheckCell.m_iColumn].m_iGemID == iGemID)
						{
							m_CheckedHorizotalMatrix[iRowBelow][currentCheckCell.m_iColumn] = true;

							iSameValueCellCount++;
							iRowBelow--;				
						}

						int iRowAbove = currentCheckCell.m_iRow + 1;
						while (iRowAbove < m_iRowNumber && m_TemporaryValueMatrix[iRowAbove][currentCheckCell.m_iColumn].m_iGemID == iGemID)
						{
							m_CheckedHorizotalMatrix[iRowAbove][currentCheckCell.m_iColumn] = true;

							iSameValueCellCount++;
							iRowAbove++;				
						}

						if (iSameValueCellCount >= 3)
						{
							if (bIsNewBlockCreated)
							{
								m_LinkedBlockList[m_iLinkedBlockCount].m_iStartIndexInDestroyedList = basicMatchingDestroyedCells.size();
								m_LinkedBlockList[m_iLinkedBlockCount].m_iGemID = m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemID;
								m_iLinkedBlockCount++;
								
								bIsNewBlockCreated = false;								
							
								// add current cell to list only when new block is created
								if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
								{
									m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;

									basicMatchingDestroyedCells.push_back(Cell(currentCheckCell.m_iRow, currentCheckCell.m_iColumn));				
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;

									iCurrentDestroyedCellIndex++;
								}
							}

							for(int iRowIndex = currentCheckCell.m_iRow - 1; iRowIndex > iRowBelow; iRowIndex--)
							{
								if (!m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn])
								{
									m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn] = true;

									basicMatchingDestroyedCells.push_back(Cell(iRowIndex, currentCheckCell.m_iColumn));
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;
								}
							}				
																	
							for(int iRowIndex = currentCheckCell.m_iRow + 1; iRowIndex < iRowAbove; iRowIndex++)
							{
								if (!m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn])
								{
									m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn] = true;

									basicMatchingDestroyedCells.push_back(Cell(iRowIndex, currentCheckCell.m_iColumn));
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;
								}
							}				
						}
					} //end horizontal check

					// vertical check					
					if( !m_CheckedVerticalMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
					{
						m_CheckedVerticalMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;
						iSameValueCellCount = 1;

						int iColumnLeft = currentCheckCell.m_iColumn - 1;
						while (iColumnLeft >=0 && m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnLeft].m_iGemID == iGemID)
						{
							m_CheckedVerticalMatrix[currentCheckCell.m_iRow][iColumnLeft] = true;

							iSameValueCellCount++;
							iColumnLeft--;
						}

						int iColumnRight = currentCheckCell.m_iColumn + 1;
						while (iColumnRight < m_iColumnNumber && m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnRight].m_iGemID == iGemID)
						{
							m_CheckedVerticalMatrix[currentCheckCell.m_iRow][iColumnRight] = true;

							iSameValueCellCount++;
							iColumnRight++;
						}

						if (iSameValueCellCount >= 3)
						{
							if (bIsNewBlockCreated)
							{
								m_LinkedBlockList[m_iLinkedBlockCount].m_iStartIndexInDestroyedList = basicMatchingDestroyedCells.size();
								m_LinkedBlockList[m_iLinkedBlockCount].m_iGemID = m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemID;
								m_iLinkedBlockCount++;
								
								bIsNewBlockCreated = false;								

								// add current cell to list only when new block is created
								if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
								{
									m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;

									basicMatchingDestroyedCells.push_back(Cell(currentCheckCell.m_iRow, currentCheckCell.m_iColumn));				
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;

									iCurrentDestroyedCellIndex++;
								}
							}

							// calculate destroyed cells							
							for(int iColumnIndex= currentCheckCell.m_iColumn- 1; iColumnIndex > iColumnLeft; iColumnIndex--)
							{
								if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][ iColumnIndex])
								{
									basicMatchingDestroyedCells.push_back(Cell(currentCheckCell.m_iRow, iColumnIndex));	
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;
								}
							}

							for(int iColumnIndex= currentCheckCell.m_iColumn+ 1; iColumnIndex <iColumnRight; iColumnIndex++)
							{
								if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][ iColumnIndex])
								{
									basicMatchingDestroyedCells.push_back(Cell(currentCheckCell.m_iRow, iColumnIndex));	
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;
								}
							}
						}
					} //end vertical check			

					if (iCurrentDestroyedCellIndex == basicMatchingDestroyedCells.size()) //no new cells added to list ==> stop
					{
						break;
					}
					else{
						currentCheckCell = basicMatchingDestroyedCells[iCurrentDestroyedCellIndex];
						iCurrentDestroyedCellIndex++;
					}
				}
			}
		}
	}
}

void NewGameBoardManager::RemoveCellsByBasicMatching( std::vector<Cell>& basicMatchingDestroyedCells, std::vector<ComboEffectBundle*>& comboChainList, const int& iMoveRow, const int& iMoveColumn)
{
	for(auto& cell : basicMatchingDestroyedCells)
	{
		if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID >= 0)
			//if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iObstacleBlockID < 0 || m_pObstacleProcessManager->DestroyCell(m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iObstacleBlockID))
			if (m_pObstacleProcessManager->DestroyCellWithObstacle(m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iObstacleBlockID))
			{		
				// NOTE: destroyed cell include double combo cells!!! ????
				// 1 cell affected by 2 effects (basic matching and double combo) ==> how to display those effect???		
				if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType != _GCT_NONE_)
				{				
					ComboEffectBundle* pTriggeredComboEffectBundle = new ComboEffectBundle();
					pTriggeredComboEffectBundle->m_ComboEffectDescription.m_eComboEffectType = GetComboEffectTypeFromComboType(m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType, iMoveRow, iMoveColumn);
						//_CET_EXPLOSION_; //only implement explosion now
					pTriggeredComboEffectBundle->m_ComboEffectDescription.m_Position = cell;
					pTriggeredComboEffectBundle->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID;
					pTriggeredComboEffectBundle->m_iNormalChainPhase = comboChainList.size(); //0;														

					comboChainList.push_back(pTriggeredComboEffectBundle);
				}			

				// combo 5 will be activated/destroyed later				
				if (!DestroySingleCellUtil(cell.m_iRow, cell.m_iColumn, 0))
				{
					cell.m_iRow = cell.m_iColumn = -1;
				}
				
				/*if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType != _GCT_COMBO5_)
				{								
				}
				else{
					m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID = -1;
					cell.m_iRow = cell.m_iColumn = -1;
				}*/
			}
			else
				cell.m_iRow = cell.m_iColumn = -1;

	}	
}

void NewGameBoardManager::CreateComboCells(const int& iSelectedRow, const int& iSelectedColumn, const std::vector<Cell>& basicMatchingDestroyedCells, std::vector<ComboEffectCell>& newComboCells)
{
	if (m_iLinkedBlockCount > 0)
	{
		for(auto& linkedBlock : m_LinkedBlockList)
		{
			if (linkedBlock.m_iCellCount > 3)
			{
				// first detect combo type
				if (linkedBlock.m_iCellCount == 4)
				{
					m_ComboCountList[0]++;
					linkedBlock.m_eGemComboType = _GCT_COMBO4_;
				}
				else if (linkedBlock.m_iCellCount >= 6)
				{
					m_ComboCountList[2]++;
					linkedBlock.m_eGemComboType = _GCT_COMBO6_;
				}
				else //5 cell in block
				{
					if (
						(basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList].m_iRow == basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 1].m_iRow &&
						basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 1].m_iRow == basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 2].m_iRow &&
						basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 2].m_iRow == basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 3].m_iRow &&
						basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 3].m_iRow == basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 4].m_iRow)
						||
						(basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList].m_iColumn == basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 1].m_iColumn &&
						basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 1].m_iColumn == basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 2].m_iColumn &&
						basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 2].m_iColumn == basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 3].m_iColumn &&
						basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 3].m_iColumn == basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + 4].m_iColumn))
					{
						m_ComboCountList[1]++;
						linkedBlock.m_eGemComboType = _GCT_COMBO5_;
					}
					else
					{
						m_ComboCountList[2]++;
						linkedBlock.m_eGemComboType = _GCT_COMBO6_;
					}
				}

				//now, create new combo cells
				bool bFoundCellOnActiveDragLine = false;
				Cell comboCell;
				// find position to place combo cell
				if (iSelectedRow >=0 || iSelectedColumn >=0)
				{
					for(int j=0; j< linkedBlock.m_iCellCount; j++)
						if (( basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + j].m_iRow == iSelectedRow
							|| basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + j].m_iColumn == iSelectedColumn) && 
							basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + j].m_iRow >= 0)
							//&&  m_BoardValueMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_iGemID < 0)
						{
							bFoundCellOnActiveDragLine = true;
							comboCell = basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + j];							
							break;
						}
				}
				// NOTE: special case: all cells belong to combo are shielded ==>combo cell won't be created
				bool bAllCellAreShielded = true;

				for(int j=0; j< linkedBlock.m_iCellCount; j++)						
						if (basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + j].m_iRow >= 0
							&&  basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + j].m_iColumn >= 0)							
						{
							bAllCellAreShielded = false;
							break;
						}

				if (bAllCellAreShielded) 
					continue;

				if (!bFoundCellOnActiveDragLine)
				{
					int iRandomIndex = -1;
					while (iRandomIndex < 0 || basicMatchingDestroyedCells[iRandomIndex].m_iRow < 0)
					{
						iRandomIndex = (rand() % linkedBlock.m_iCellCount) + linkedBlock.m_iStartIndexInDestroyedList; 
						comboCell = basicMatchingDestroyedCells[iRandomIndex];
					}
				}
				
				if (linkedBlock.m_eGemComboType != _GCT_COMBO6_)
					m_BoardValueMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_iGemID = linkedBlock.m_iGemID;//_SPECIAL_COMBO_GEM_ID; //
				else
					m_BoardValueMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_iGemID = _SPECIAL_COMBO_GEM_ID; //

				m_BoardValueMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_eGemComboType = linkedBlock.m_eGemComboType;

				newComboCells.push_back(ComboEffectCell(comboCell, linkedBlock.m_eGemComboType, linkedBlock.m_iGemID) );					
			}
		}
	}
}

void NewGameBoardManager::CreateUnlockedGemLetterFromWaitingList(std::vector<NewCellInfo>& newCells)
{
	// create unlocked letter cells from waiting list, this must before move cells
	for(auto& cell : m_UnlockedGemLetterCellList)
	{
		m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID = cell.m_iGemID;
		
		newCells.push_back(cell);
	}

}

void NewGameBoardManager::CalculateMoveCells(std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells) //, std::vector<Cell>& newCells)
{
	int iRow, iColumn;	

	// calculate move cells
	for(iRow = 1; iRow < m_iRowNumber; iRow++)
		for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
		{								
			if (m_BoardValueMatrix[iRow][iColumn].m_iGemID >= 0)
			{
				// find destroyed cell below
				bool bFoundDestroyedCell = false;
				int iFindRowIndex = iRow -1;
				int iCheckRowIndex = -1;
				while (iFindRowIndex >= 0 && (( m_BoardValueMatrix[iFindRowIndex][iColumn].m_iGemID < 0 && !m_BoardValueMatrix[iFindRowIndex][iColumn].m_bIsBlankCell)
					|| m_BoardValueMatrix[iFindRowIndex][iColumn].m_bIsBlankCell))
				{
					if ( m_BoardValueMatrix[iFindRowIndex][iColumn].m_iGemID < 0 && !m_BoardValueMatrix[iFindRowIndex][iColumn].m_bIsBlankCell)
					{
						bFoundDestroyedCell = true;
						iCheckRowIndex = iFindRowIndex;
					}
						
					iFindRowIndex--;
				}

				if (bFoundDestroyedCell)
				{
					originalMovedCells.push_back(Cell(iRow, iColumn));						

					m_BoardValueMatrix[iCheckRowIndex][iColumn] = m_BoardValueMatrix[iRow][iColumn];
					m_BoardValueMatrix[iRow][iColumn].Reset(); //m_iGemID = -1;	m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;
					targetMovedCells.push_back(Cell(iCheckRowIndex, iColumn));

					//m_pObstacleProcessManager->MoveObstacles( iRow, iColumn, iCheckRowIndex, iColumn);
				}
			}
		}	

	// refine position of waiting trigger combo 5
	for(auto& waitingTriggerCombo : m_WaitingTriggerCombo6List)
		for(int iIndex=0; iIndex < originalMovedCells.size(); iIndex++)
			if (waitingTriggerCombo.m_Position == originalMovedCells[iIndex])
				waitingTriggerCombo.m_Position = targetMovedCells[iIndex];		
}

void NewGameBoardManager::GenerateNewGems(std::vector<NewCellInfo>& newCells, bool bIsNewMove)
{
	int iRow, iColumn;	
	
	// generate new gems
	for(iRow = 0; iRow < m_iRowNumber; iRow++)
		for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
			if (!m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && m_BoardValueMatrix[iRow][iColumn].m_iGemID < 0)
			{
				m_BoardValueMatrix[iRow][iColumn].m_iGemID = rand() % m_pLevelConfig->m_iNumberOfColor;
				newCells.push_back(NewCellInfo(iRow, iColumn, m_BoardValueMatrix[iRow][iColumn].m_iGemID));
			}

	std::vector<GemLetterData> outputLettersForGems;
	m_pGameWordManager->GenerateNewLetters( newCells.size(), outputLettersForGems, bIsNewMove);	

	unsigned char iLetter = 255;	
	for(int iIndex = 0; iIndex < newCells.size(); iIndex++)	
	{
		iLetter = outputLettersForGems[iIndex].m_iLetter;		
		NewCellInfo& cell = newCells[iIndex];
		if (iLetter < 255)
		{			
			cell.m_iGemLetterBlockID = m_GemLetterManager.AllocFreeBlock(iLetter, outputLettersForGems[iIndex].m_bIsInMainWord);
			m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemLetterBlockID = cell.m_iGemLetterBlockID;			
		}
		else
		{
			m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemLetterBlockID = -1;

			// generate obstacle!!!
			for(auto pObstacleLevelConfig : m_pLevelConfig->m_ObstacleConfigList)
			{
				if (pObstacleLevelConfig->m_bEnableGenerateByDrop)
				{
					const ObstacleDescription* pObstacleDescription = GameConfigManager::getInstance()->GetObstacleDescription(pObstacleLevelConfig->m_iObstacleID);
					if (!pObstacleDescription->m_bAppearByDrop)
						continue;

					if (pObstacleLevelConfig->m_bCanDropOnAllColumn)
					{
						if (SuccessWithPercentRatio(pObstacleLevelConfig->m_iDropOnAllColumnRate))
							m_pObstacleProcessManager->GenerateObstacle( pObstacleLevelConfig, m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iObstacleBlockID);
					}
					else if (pObstacleLevelConfig->m_DropOnColumnsRateList[cell.m_iColumn] >0)
						if (SuccessWithPercentRatio(pObstacleLevelConfig->m_DropOnColumnsRateList[cell.m_iColumn]))
							m_pObstacleProcessManager->GenerateObstacle( pObstacleLevelConfig, m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iObstacleBlockID);
				}
			
			}
		}
	}
}

// score and stars
int NewGameBoardManager::GetScoreForUnlockLetterInMainWord()
{
	return m_GameConfig.m_iScoreOfMainWord;
}

int NewGameBoardManager::GetScoreForUnlockLetterInSubWord()
{
	return m_GameConfig.m_iScoreOfSubWord;
}

int NewGameBoardManager::IncreaseScoreForLetterInMainWord()
{
	m_iCurrentScore += m_GameConfig.m_iScoreOfMainWord;
	return m_GameConfig.m_iScoreOfMainWord;
}

int NewGameBoardManager::IncreaseScoreForLetterInSubWords(const int& iLetterCount)
{
	int iTotalScoreIncrement = m_GameConfig.m_iScoreOfSubWord * iLetterCount;
	m_iCurrentScore += iTotalScoreIncrement;
	return iTotalScoreIncrement;
}

int NewGameBoardManager::IncreaseScoreForUnlockMainWord()
{
	int iBonusScore = GetBonusScoreForUnlockMainWord();
	m_iCurrentScore += iBonusScore;
	
	return iBonusScore;
}

int NewGameBoardManager::GetBonusScoreForUnlockMainWord(bool bIncludeIndividualLetters)
{
	int iBonusScore = m_pGameWordManager->GetMainWord().m_iWordLength * m_GameConfig.m_iScoreOfMainWord * ( m_GameConfig.m_iMainWordScoreRatio + (int)bIncludeIndividualLetters);	

	return iBonusScore;
}

int NewGameBoardManager::IncreaseScoreForUnlockSubWord(const int& iSubWordID)
{
	int iBonusScore = GetBonusScoreForUnlockSubWord(iSubWordID);
	m_iCurrentScore += iBonusScore;

	return iBonusScore;
}

int NewGameBoardManager::GetBonusScoreForUnlockSubWord(const int& iSubWordID, bool bIncludeIndividualLetters)
{
	int iBonusScore = m_pGameWordManager->GetSubWord(iSubWordID).m_iWordLength * m_GameConfig.m_iScoreOfSubWord * (m_GameConfig.m_iSubWordScoreRatio + (int)bIncludeIndividualLetters);
	return iBonusScore;
}

int NewGameBoardManager::IncreaseScoreForDestroyCells(const int& iGemCount, const ComboEffectType& eComboEffectType)
{
	int iRatio = m_GameConfig.m_iScoreOfGem;
	switch (eComboEffectType)
	{
		case ComboEffectType::_CET_EXPLOSION_:
			iRatio *= m_GameConfig.m_iCombEffectRatio4;
			break;
		case ComboEffectType::_CET_DOUBLE_EXPLOSION_:
			iRatio *= m_GameConfig.m_iCombEffectRatio4_4;
			break;
	}
	int iIncrementScore = iRatio * iGemCount;
	m_iCurrentScore += iIncrementScore;

	return iIncrementScore;
}

int NewGameBoardManager::GetEarnedStars()
{
	if (m_iCurrentScore >= m_pLevelConfig->m_ScoreOfStars[2])
		return 3;
	else if (m_iCurrentScore >= m_pLevelConfig->m_ScoreOfStars[1])
		return 2;
	else  if (m_iCurrentScore >= m_pLevelConfig->m_ScoreOfStars[0])
		return 1;
	else
		return 0;
}

ComboEffectType NewGameBoardManager::GetComboEffectTypeFromComboType(GemComboType_e eGemComboType, const int& iMoveRow, const int& iMoveColumn)
{
	switch(eGemComboType)
	{
		default:
		case _GCT_COMBO4_:
		{
			if (iMoveRow >= 0)
				return _CET_DESTROY_ROW_;
			else
				return _CET_DESTROY_COLUMN_;
		}
		case _GCT_COMBO5_:
			return _CET_EXPLOSION_;			
		case _GCT_COMBO6_:
			{
				if (iMoveRow >= 0)
					return _CET_DESTROY_COLOR_ROW_;
				else
					return _CET_DESTROY_COLOR_COLUMN_;
			}
			//return _CET_DOUBLE_EXPLOSION_;
	}
}

void NewGameBoardManager::PreCheckComboEffect(int iSelectedRow, int iSelectedColumn, ComboActivateDirection_e eDirection, std::vector<Cell>& basicMatchingDestroyedCells)
{
	if (m_BoardValueMatrix[iSelectedRow][iSelectedColumn].m_eGemComboType == _GCT_COMBO4_)
	{
		basicMatchingDestroyedCells.push_back(Cell( iSelectedRow, iSelectedColumn));

		switch (eDirection)
		{
			case _CAD_LEFT_:
			{
				for(int iColumn = iSelectedColumn-1; iColumn >= 0; iColumn--)
					if (!m_BoardValueMatrix[iSelectedRow][iColumn].m_bIsBlankCell)
					{
						basicMatchingDestroyedCells.push_back(Cell( iSelectedRow, iColumn));
					}
				break;
			}
			case _CAD_UP_:
			{
				for(int iRow = iSelectedRow+1; iRow < m_iRowNumber; iRow++)
					if (!m_BoardValueMatrix[iRow][iSelectedColumn].m_bIsBlankCell)
					{
						basicMatchingDestroyedCells.push_back(Cell( iRow, iSelectedColumn));
					}
				break;
			}
			case _CAD_RIGHT_:
			{
				for(int iColumn = iSelectedColumn+1; iColumn < m_iColumnNumber; iColumn++)
					if (!m_BoardValueMatrix[iSelectedRow][iColumn].m_bIsBlankCell)
					{
						basicMatchingDestroyedCells.push_back(Cell( iSelectedRow, iColumn));
					}
				break;
			}
			default:
			case _CAD_DOWN_:
			{
				for(int iRow = iSelectedRow-1; iRow >= 0; iRow--)
					if (!m_BoardValueMatrix[iRow][iSelectedColumn].m_bIsBlankCell)
					{
						basicMatchingDestroyedCells.push_back(Cell( iRow, iSelectedColumn));
					}
				break;
			}
		}		
	}
	else //if (m_BoardValueMatrix[iSelectedRow][iSelectedColumn].m_eGemComboType == _GCT_COMBO4_2_)
	{
		basicMatchingDestroyedCells.push_back(Cell( iSelectedRow, iSelectedColumn));

		switch (eDirection)
		{
			case _CAD_LEFT_:
			{
				for(int iColumn = iSelectedColumn-1; iColumn >= 0; iColumn--)
				{
					if (!m_BoardValueMatrix[iSelectedRow][iColumn].m_bIsBlankCell)
					{
						basicMatchingDestroyedCells.push_back(Cell( iSelectedRow, iColumn));
					}

					if (  iSelectedRow - (iSelectedColumn-iColumn) >= 0 && !m_BoardValueMatrix[iSelectedRow - (iSelectedColumn-iColumn) ][iColumn].m_bIsBlankCell)
						basicMatchingDestroyedCells.push_back(Cell( iSelectedRow - (iSelectedColumn-iColumn), iColumn));

					if (  iSelectedRow + (iSelectedColumn-iColumn) < m_iRowNumber && !m_BoardValueMatrix[iSelectedRow + (iSelectedColumn-iColumn) ][iColumn].m_bIsBlankCell)
						basicMatchingDestroyedCells.push_back(Cell( iSelectedRow + (iSelectedColumn-iColumn), iColumn));
				}
				break;
			}
			case _CAD_UP_:
			{
				for(int iRow = iSelectedRow+1; iRow < m_iRowNumber; iRow++)
				{
					if (!m_BoardValueMatrix[iRow][iSelectedColumn].m_bIsBlankCell)
					{
						basicMatchingDestroyedCells.push_back(Cell( iRow, iSelectedColumn));
					}

					if (  iSelectedColumn - (iRow - iSelectedRow) >= 0 && !m_BoardValueMatrix[iRow][iSelectedColumn - (iRow - iSelectedRow)].m_bIsBlankCell)
						basicMatchingDestroyedCells.push_back(Cell( iRow, iSelectedColumn - (iRow - iSelectedRow)));

					if (  iSelectedColumn + (iRow - iSelectedRow) < m_iColumnNumber && !m_BoardValueMatrix[iRow][iSelectedColumn + (iRow - iSelectedRow)].m_bIsBlankCell)
						basicMatchingDestroyedCells.push_back(Cell( iRow, iSelectedColumn + (iRow - iSelectedRow)));
				}
				break;
			}
			case _CAD_RIGHT_:
			{
				for(int iColumn = iSelectedColumn+1; iColumn < m_iColumnNumber; iColumn++)
				{
					if (!m_BoardValueMatrix[iSelectedRow][iColumn].m_bIsBlankCell)
					{
						basicMatchingDestroyedCells.push_back(Cell( iSelectedRow, iColumn));
					}

					if (  iSelectedRow - (iColumn - iSelectedColumn) >= 0 && !m_BoardValueMatrix[iSelectedRow - (iColumn - iSelectedColumn) ][iColumn].m_bIsBlankCell)
						basicMatchingDestroyedCells.push_back(Cell( iSelectedRow - (iColumn - iSelectedColumn), iColumn));

					if (  iSelectedRow + (iColumn - iSelectedColumn) < m_iRowNumber && !m_BoardValueMatrix[iSelectedRow + (iColumn - iSelectedColumn) ][iColumn].m_bIsBlankCell)
						basicMatchingDestroyedCells.push_back(Cell( iSelectedRow + (iColumn - iSelectedColumn), iColumn));
				}
				break;
			}
			default:
			case _CAD_DOWN_:
			{
				for(int iRow = iSelectedRow-1; iRow >= 0; iRow--)
				{
					if (!m_BoardValueMatrix[iRow][iSelectedColumn].m_bIsBlankCell)
					{
						basicMatchingDestroyedCells.push_back(Cell( iRow, iSelectedColumn));
					}

					if (  iSelectedColumn - (iSelectedRow-iRow) >= 0 && !m_BoardValueMatrix[iRow][iSelectedColumn - (iSelectedRow-iRow)].m_bIsBlankCell)
						basicMatchingDestroyedCells.push_back(Cell( iRow, iSelectedColumn - (iSelectedRow-iRow)));

					if (  iSelectedColumn + (iSelectedRow-iRow) < m_iColumnNumber && !m_BoardValueMatrix[iRow][iSelectedColumn + (iSelectedRow-iRow)].m_bIsBlankCell)
						basicMatchingDestroyedCells.push_back(Cell( iRow, iSelectedColumn + (iSelectedRow-iRow)));
				}
				break;
			}
		}		
	}
}


void NewGameBoardManager::ExecuteComboEffect(int iSelectedRow, int iSelectedColumn, ComboActivateDirection_e eDirection,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboCreationInfo>& newDoubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList, std::vector<ComboEffectBundle*>& triggeredCombo5ChainList,
		std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<NewCellInfo>& unlockedLetterCells,
		std::vector<NewCellInfo>& newCells)
{
	m_UnlockedGemLetterCellList.clear();
	m_bIsBossStateChanged = false;
	m_LevelBossInfo.m_bJustReleaseALetter = false;

	// get effective cells
	PreCheckComboEffect(iSelectedRow, iSelectedColumn, eDirection, basicMatchingDestroyedCells);

	m_BoardValueMatrix[iSelectedRow][iSelectedColumn].m_eGemComboType = _GCT_NONE_;

	if (basicMatchingDestroyedCells.size() > 0)
	{
		// remove cells by basic matching and create first combo chain		
		RemoveCellsByBasicMatching( basicMatchingDestroyedCells, comboChainList, 0, 0);

		// trigger combo chain from effect of triggered combos
		ExecuteComboChain(comboChainList);

		// create combo cells	
		CreateComboCells( iSelectedRow, iSelectedColumn, basicMatchingDestroyedCells, newComboCells);

		// create unlocked gems with letter
		CreateUnlockedGemLetterFromWaitingList(unlockedLetterCells);

		// calculate move cells and create new cells
		CalculateMoveCells( originalMovedCells, targetMovedCells);

		// generate new cells
		GenerateNewGems(newCells, false);
	}
}



bool NewGameBoardManager::haveCellMatch3(const Cell& cell)
{
	if (m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn].m_bIsBlankCell == false)
	{	
		int iGemID = m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID;

		//Check horizontal
		if ((cell.m_iRow - 1 >= 0 && iGemID == m_TemporaryValueMatrix[cell.m_iRow - 1][cell.m_iColumn].m_iGemID)
			&& (cell.m_iRow - 2 >= 0 && iGemID == m_TemporaryValueMatrix[cell.m_iRow - 2][cell.m_iColumn].m_iGemID))
		{
			return true;
		}
		
		if ((cell.m_iRow - 1 >= 0 && iGemID == m_TemporaryValueMatrix[cell.m_iRow - 1][cell.m_iColumn].m_iGemID)
			&& (cell.m_iRow + 1 < m_iRowNumber && iGemID == m_TemporaryValueMatrix[cell.m_iRow + 1][cell.m_iColumn].m_iGemID))
		{
			return true;
		}
		
		if ((cell.m_iRow + 1 < m_iRowNumber && iGemID == m_TemporaryValueMatrix[cell.m_iRow + 1][cell.m_iColumn].m_iGemID)
			&& (cell.m_iRow + 2 < m_iRowNumber && iGemID == m_TemporaryValueMatrix[cell.m_iRow + 2][cell.m_iColumn].m_iGemID))
		{
			return true;
		}

		//Check vertical
		if ((cell.m_iColumn - 1 >= 0 && iGemID == m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn - 1].m_iGemID)
			&& (cell.m_iColumn - 2 >= 0 && iGemID == m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn - 2].m_iGemID))
		{
			return true;
		}
		
		if ((cell.m_iColumn - 1 >= 0 && iGemID == m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn - 1].m_iGemID)
			&& (cell.m_iColumn + 1 < m_iColumnNumber && iGemID == m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn + 1].m_iGemID))
		{
			return true;
		}
		
		if ((cell.m_iColumn + 1 < m_iColumnNumber && iGemID == m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn + 1].m_iGemID)
			&& (cell.m_iColumn + 2 < m_iColumnNumber && iGemID == m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn + 2].m_iGemID))
		{
			return true;
		}
	}

	return false;
}

int NewGameBoardManager::haveMatch3WHenMoveRow(const int& iRow)
{
	if (IsRowLocked(iRow) == true)
	{
		return 0;
	}

	Cell currentCheckCell;
	currentCheckCell.m_iRow = iRow;
	int iDeltaMoveRow = 0;
	int iIndexColumn, iTranslationCell;

	while (iDeltaMoveRow < m_iColumnNumber - 1)
	{
		iDeltaMoveRow++;

		// create temporary list to compute result for shifting move
		for (iIndexColumn=0; iIndexColumn<m_iColumnNumber; iIndexColumn++)
		{
			if (!m_BoardValueMatrix[iRow][iIndexColumn].m_bIsBlankCell)
			{	
				//int iBlankCellCount = 0; //blank cell on the move
				iTranslationCell = 0;

				for(int iStep=1; iStep<= iDeltaMoveRow; iStep++)
				{
					iTranslationCell += 1;
					if (m_BoardValueMatrix[iRow][(iIndexColumn + iTranslationCell)%m_iColumnNumber].m_bIsBlankCell)
						iStep--;
				}

				m_TemporaryValueMatrix[iRow][(iIndexColumn + m_iColumnNumber + iTranslationCell)%m_iColumnNumber] = m_BoardValueMatrix[iRow][iIndexColumn];
				
			}
			else
			{
				//blank cell
				m_TemporaryValueMatrix[iRow][iIndexColumn].m_bIsBlankCell = true;
				m_TemporaryValueMatrix[iRow][iIndexColumn].m_iGemID = -1;
			}
		}

		for (iIndexColumn=0; iIndexColumn<m_iColumnNumber; iIndexColumn++)
		{	
			currentCheckCell.m_iColumn = iIndexColumn;
			if(haveCellMatch3(currentCheckCell) == true)
			{
				return iDeltaMoveRow;
			}
		}
	}

	return 0;
}

int NewGameBoardManager::haveMatch3WHenMoveColumn(const int& iColumn)
{
	if (IsColumnLocked(iColumn) == true)
	{
		return 0;
	}

	Cell currentCheckCell;
	currentCheckCell.m_iColumn = iColumn;
	int iDeltaMoveColumn = 0;
	int iIndexRow, iTranslationCell;

	while (iDeltaMoveColumn < m_iRowNumber - 1)
	{
		iDeltaMoveColumn++;

		// create temporary list to compute result for shifting move
		for (iIndexRow=0; iIndexRow<m_iRowNumber; iIndexRow++)
		{
			if (!m_BoardValueMatrix[iIndexRow][iColumn].m_bIsBlankCell)
			{	
				//int iBlankCellCount = 0; //blank cell on the move
				iTranslationCell = 0;

				for(int iStep=1; iStep<= abs(iDeltaMoveColumn); iStep++)
				{
					iTranslationCell += 1;
					if (m_BoardValueMatrix[(iIndexRow + iTranslationCell) % m_iRowNumber][iColumn].m_bIsBlankCell)
						iStep--;
				}
					
				m_TemporaryValueMatrix[(iIndexRow + m_iRowNumber + iTranslationCell) % m_iRowNumber][iColumn] = m_BoardValueMatrix[iIndexRow][iColumn];
			}
			else
			{ 
				//blank cell
				m_TemporaryValueMatrix[iIndexRow][iColumn].m_bIsBlankCell = true;
				m_TemporaryValueMatrix[iIndexRow][iColumn].m_iGemID = -1;
			}
		}

		for (iIndexRow=0; iIndexRow<m_iRowNumber; iIndexRow++)
		{	
			currentCheckCell.m_iRow = iIndexRow;
			if(haveCellMatch3(currentCheckCell) == true)
			{
				return iDeltaMoveColumn;
			}
		}
	}

	return 0;
}

Hint NewGameBoardManager::findHintForGame()
{
	Hint hint;
	hint.m_deltaMove = 0;
	hint.m_iRow = -1;
	hint.m_iColumn = -1;

	int iIndexRow, iIndexColumn;
	
	// create temporary list to compute result for shifting move
	memcpy( m_TemporaryValueMatrix, m_BoardValueMatrix, sizeof(m_BoardValueMatrix));

	for(iIndexRow=0; iIndexRow < m_iRowNumber; iIndexRow++)
	{
		int deltaMove = haveMatch3WHenMoveRow(iIndexRow);

		//Reset row moved
		for (iIndexColumn = 0; iIndexColumn < m_iColumnNumber; iIndexColumn++)
		{
			m_TemporaryValueMatrix[iIndexRow][iIndexColumn] = m_BoardValueMatrix[iIndexRow][iIndexColumn];
		}

		if(deltaMove != 0)
		{
			hint.m_deltaMove = deltaMove;
			hint.m_iRow = iIndexRow;

			return hint;
		}
	}

	for(iIndexColumn=0; iIndexColumn < m_iColumnNumber; iIndexColumn++)
	{
		int deltaMove = haveMatch3WHenMoveColumn(iIndexColumn);

		//Reset column moved
		for (iIndexRow = 0; iIndexRow < m_iRowNumber; iIndexRow++)
		{
			m_TemporaryValueMatrix[iIndexRow][iIndexColumn] = m_BoardValueMatrix[iIndexRow][iIndexColumn];
		}

		if(deltaMove != 0)
		{
			hint.m_deltaMove = deltaMove;
			hint.m_iColumn = iIndexColumn;

			return hint;
		}
	}

	return hint;
}