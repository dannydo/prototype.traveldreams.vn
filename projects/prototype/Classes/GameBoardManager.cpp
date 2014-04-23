#include "GameBoardManager.h"
#include "cocos2d.h"

GameBoardManager::GameBoardManager()
{
	//m_ComboCountList[0] = m_ComboCountList[1] = m_ComboCountList[2] = 0;
}

void GameBoardManager::GenerateGameBoard(GameModeType_e eGameModeType, int iTimeModeStage)
{	
	if (eGameModeType == _GMT_NORMAL_)
	{
		m_GameConfig = GameConfigManager::getInstance()->GetGameConfig();

		std::string sCurrentChapterID = GameConfigManager::getInstance()->GetCurrentChapterID();
		int iCurrentLevel = GameConfigManager::getInstance()->GetCurrentLevelId();
		m_pLevelConfig = &GameConfigManager::getInstance()->GetLevelConfig( sCurrentChapterID, iCurrentLevel);
	}
	else
	{		
		m_pLevelConfig = &GameConfigManager::getInstance()->GetTimeModeDemoConfig();

		m_GameConfig = ((TimeModeLevelConfig*)m_pLevelConfig)->m_ScoreConfig;
		auto& baseGameConfig = GameConfigManager::getInstance()->GetGameConfig();
		
		m_GameConfig.m_iScoreLetterOfBonusWord = baseGameConfig.m_iScoreLetterOfBonusWord;
		m_GameConfig.m_iBonusScoreCompleteBonusQuest = baseGameConfig.m_iBonusScoreCompleteBonusQuest;
		m_GameConfig.m_iBonusScoreActivateBonusEndGameCombo = baseGameConfig.m_iBonusScoreActivateBonusEndGameCombo;
		m_GameConfig.m_iEndGameComboEffectDestroyCellRatio = baseGameConfig.m_iEndGameComboEffectDestroyCellRatio;	
	}

	// Load game config
	//LoadGameConfig();

	// Load basic combo list
	//LoadBasicComboPatternList();

	//

	m_iRowNumber = m_pLevelConfig->m_iRowNumber;
	m_iColumnNumber = m_pLevelConfig->m_iColumnNumber;

	srand(time(NULL));

	// board config now is loaded from file
	int iRow, iColumn;
	for(iRow = 0; iRow < m_iRowNumber; iRow++)
		for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
		{
			if (m_pLevelConfig->m_BoardMatrix[iRow][iColumn] <= 0)
			{
				m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell = true;
				m_BoardValueMatrix[iRow][iColumn].m_iGemID = -1;

				if (m_pLevelConfig->m_BoardMatrix[iRow][iColumn] == _GDS_LOCAL_DRAG_WALL_CELL_CONFIG_ID_)
					m_BoardValueMatrix[iRow][iColumn].m_bIsDragLocalWall = true;

				continue;
			}
			
			m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell = false;

			switch (m_pLevelConfig->m_BoardMatrix[iRow][iColumn])
			{
				case 4:
					m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_COMBO4_;
					m_BoardValueMatrix[iRow][iColumn].m_iGemID = rand() % m_pLevelConfig->m_iNumberOfColor;			
					break;
				case 5:
					m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_COMBO5_;
					m_BoardValueMatrix[iRow][iColumn].m_iGemID = rand() % m_pLevelConfig->m_iNumberOfColor;			
					break;
				default:
					m_BoardValueMatrix[iRow][iColumn].m_iGemID = rand() % m_pLevelConfig->m_iNumberOfColor;			
					break;
				case 6:
					m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_COMBO6_;
					m_BoardValueMatrix[iRow][iColumn].m_iGemID = _SPECIAL_COMBO_GEM_ID;
			}			
			
			while ( (iRow>1 && m_BoardValueMatrix[iRow][iColumn].m_iGemID == m_BoardValueMatrix[iRow-1][iColumn].m_iGemID && m_BoardValueMatrix[iRow][iColumn].m_iGemID == m_BoardValueMatrix[iRow-2][iColumn].m_iGemID) ||
					(iColumn >1 && m_BoardValueMatrix[iRow][iColumn].m_iGemID == m_BoardValueMatrix[iRow][iColumn-1].m_iGemID && m_BoardValueMatrix[iRow][iColumn].m_iGemID == m_BoardValueMatrix[iRow][iColumn-2].m_iGemID))
			{
				m_BoardValueMatrix[iRow][iColumn].m_iGemID = (m_BoardValueMatrix[iRow][iColumn].m_iGemID + 1) % m_pLevelConfig->m_iNumberOfColor;
			}
		}

	// for test
	//m_BoardValueMatrix[1][0].m_iGemID = m_BoardValueMatrix[1][1].m_iGemID=m_BoardValueMatrix[0][2].m_iGemID=m_BoardValueMatrix[1][3].m_iGemID=	m_BoardValueMatrix[1][4].m_iGemID = 0;
	//	m_BoardValueMatrix[4][2].m_iGemID=m_BoardValueMatrix[5][2].m_iGemID=0;
	//m_BoardValueMatrix[1][2].m_iGemID=m_BoardValueMatrix[2][2].m_iGemID=1;

	// generate obstacles
	/*for(auto pObstacle : m_pLevelConfig->m_ObstacleConfigList)
	{
		for(int i=0; i< pObstacle->m_iCount; i++)			
			m_BoardValueMatrix[pObstacle->m_ObstaclePositionList[i].m_iRow][pObstacle->m_ObstaclePositionList[i].m_iColumn].m_eObstacleType = (ObstacleType_e)pObstacle->m_iObstacleID;
	}*/
	/*
	// test obstacle
	if (iLevel == 2)
	{
		 //m_BoardValueMatrix[2][2].m_eObstacleType = m_BoardValueMatrix[3][3].m_eObstacleType = m_BoardValueMatrix[4][4].m_eObstacleType = m_BoardValueMatrix[5][5].m_eObstacleType  = m_BoardValueMatrix[6][6].m_eObstacleType = _OT_LOCK_ROW_AND_COLUMN_;
	}

	int iRow, iColumn;
	for(iRow = 0; iRow < m_iRowNumber; iRow++)
		for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
		{
			if (iLevel == 2)
			{
			}
			else if (iLevel == 3)
			{
				if ( iColumn >=3 && iColumn<=4 && (iRow <=1 || iRow>=m_iRowNumber-2) )
				{
					m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell = true;
					m_BoardValueMatrix[iRow][iColumn].m_iGemID = -1;
					continue;
				}
			}
			else if (iLevel == 4)
			{
				if (( iRow<=1 &&  (iColumn<=1 || iColumn >= m_iColumnNumber-2)) || (iRow >= m_iRowNumber-2 &&  (iColumn <=1 || iColumn>= m_iColumnNumber-2)))
				{
					m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell = true;
					m_BoardValueMatrix[iRow][iColumn].m_iGemID = -1;
					continue;
				}
			}
			else if (iLevel == 5)
			{
				if ((iColumn >= 3 && iColumn <=4) && (iRow >= 3 && iRow <=4))
				{
					m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell = true;
					m_BoardValueMatrix[iRow][iColumn].m_iGemID = -1;
					continue;
				}
			}		
			
			m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell = false;

			m_BoardValueMatrix[iRow][iColumn].m_iGemID = rand() % _MAX_GEM_ID_;

			//m_BoardValueMatrix[1][0].m_iGemID = m_BoardValueMatrix[1][1].m_iGemID=m_BoardValueMatrix[0][2].m_iGemID=m_BoardValueMatrix[1][3].m_iGemID=m_BoardValueMatrix[4][2].m_iGemID=m_BoardValueMatrix[5][2].m_iGemID=0;
			//m_BoardValueMatrix[1][2].m_iGemID=m_BoardValueMatrix[2][2].m_iGemID=1;

			//m_BoardValueMatrix[2][3].m_iGemID = m_BoardValueMatrix[1][2].m_iGemID=m_BoardValueMatrix[0][2].m_iGemID= m_BoardValueMatrix[3][2].m_iGemID=m_BoardValueMatrix[4][2].m_iGemID=1;			
			
			while ( (iRow>1 && m_BoardValueMatrix[iRow][iColumn].m_iGemID == m_BoardValueMatrix[iRow-1][iColumn].m_iGemID && m_BoardValueMatrix[iRow][iColumn].m_iGemID == m_BoardValueMatrix[iRow-2][iColumn].m_iGemID) ||
					(iColumn >1 && m_BoardValueMatrix[iRow][iColumn].m_iGemID == m_BoardValueMatrix[iRow][iColumn-1].m_iGemID && m_BoardValueMatrix[iRow][iColumn].m_iGemID == m_BoardValueMatrix[iRow][iColumn-2].m_iGemID))
			{
				m_BoardValueMatrix[iRow][iColumn].m_iGemID = (m_BoardValueMatrix[iRow][iColumn].m_iGemID + 1) % _MAX_GEM_ID_;
			}
		}*/
}
/*
bool GameBoardManager::DoAMove(int iSelectedRow, int iSelectedColumn,int iDeltaRow, int iDeltaColumn, std::vector<Cell>& destroyCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells, std::vector<Cell>& newCells)
{
	int iSameValueCellCount;
	bool bIsValidMove = false;
	if (iSelectedRow >= 0) // move row
	{
		int iRow, iColumn, iGemID, iNewColumn;
		for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
		{
			iSameValueCellCount = 1;
			iGemID =  m_BoardValueMatrix[iSelectedRow][iColumn];
			iNewColumn = (iColumn + iDeltaColumn + m_iColumnNumber) % m_iColumnNumber;
			
			int iRowBelow = iSelectedRow - 1;
			while (iRowBelow >=0 && m_BoardValueMatrix[iRowBelow][iNewColumn] == iGemID)
			{
				iSameValueCellCount++;
				iRowBelow--;
			}

			int iRowAbove = iSelectedRow + 1;
			while (iRowAbove < m_iRowNumber && m_BoardValueMatrix[iRowAbove][iNewColumn] == iGemID)
			{
				iSameValueCellCount++;
				iRowAbove++;
			}

			if (iSameValueCellCount >= 3)
			{
				bIsValidMove = true;
				// calculate destroyed cells
				destroyCells.push_back(Cell(iSelectedRow, iNewColumn));				

				for(iRow = iSelectedRow - 1; iRow >iRowBelow; iRow--)
				{
					destroyCells.push_back(Cell(iRow, iNewColumn));					
				}

				for(iRow = iSelectedRow + 1; iRow < iRowAbove; iRow++)
				{
					destroyCells.push_back(Cell(iRow, iNewColumn));					
				}				
			}
		}
	}
	else if (iSelectedColumn >= 0) // move column
	{
		int iRow, iColumn, iGemID, iNewRow, iSameValueCellCount;
		for(iRow = 0; iRow < m_iRowNumber; iRow++)
		{
			iSameValueCellCount = 1;
			iGemID =  m_BoardValueMatrix[iRow][iSelectedColumn];
			iNewRow = (iRow + iDeltaRow + m_iRowNumber) % m_iRowNumber;			
			int iColumnLeft = iSelectedColumn - 1;
			while (iColumnLeft >=0 && m_BoardValueMatrix[iNewRow][iColumnLeft] == iGemID)
			{
				iSameValueCellCount++;
				iColumnLeft--;
			}

			int iColumnRight = iSelectedColumn + 1;
			while (iColumnRight < m_iColumnNumber && m_BoardValueMatrix[iNewRow][iColumnRight] == iGemID)
			{
				iSameValueCellCount++;
				iColumnRight++;
			}

			if (iSameValueCellCount >= 3)
			{
				bIsValidMove = true;

				// calculate destroyed cells
				destroyCells.push_back(Cell(iNewRow, iSelectedColumn));

				for(iColumn= iSelectedColumn- 1; iColumn >iColumnLeft; iColumn--)
				{
					destroyCells.push_back(Cell(iNewRow, iColumn));					
				}

				for(iColumn= iSelectedColumn+ 1; iColumn <iColumnRight; iColumn++)
				{
					destroyCells.push_back(Cell(iNewRow, iColumn));					
				}
			}
		}
	}

	if (bIsValidMove)
	{
		int iRow, iColumn;
		
		// translate row or column
		if (iSelectedRow >= 0)
		{
			int tempList[_BOARD_MAX_COLUMN_NUMBER_];
			for(iColumn =0; iColumn < m_iColumnNumber; iColumn++)
				tempList[iColumn] = m_BoardValueMatrix[iSelectedRow][iColumn];
			for(iColumn =0; iColumn < m_iColumnNumber; iColumn++)
				m_BoardValueMatrix[iSelectedRow][(iColumn+iDeltaColumn+m_iColumnNumber) % m_iColumnNumber] = tempList[iColumn];
		}
		else
		{
			int tempList[_BOARD_MAX_ROW_NUMBER_];
			for(iRow =0; iRow < m_iRowNumber; iRow++)
				tempList[iRow] = m_BoardValueMatrix[iRow][iSelectedColumn];
			for(iRow =0; iRow < m_iRowNumber; iRow++)
				m_BoardValueMatrix[ (iRow + iDeltaRow+m_iRowNumber) % m_iRowNumber][iSelectedColumn] = tempList[iRow];				
		}

		// remove cells
		for(auto cell : destroyCells)
			m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn] = -1;

		// calculate move cells
		for(iRow = 1; iRow < m_iRowNumber; iRow++)
			for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
			{
				if (m_BoardValueMatrix[iRow][iColumn] >= 0 &&  m_BoardValueMatrix[iRow-1][iColumn] < 0)
				{
					originalMovedCells.push_back(Cell(iRow, iColumn));
					int iNewRow = iRow-1;
					while (iNewRow >=0 && m_BoardValueMatrix[iNewRow][iColumn] < 0)
						iNewRow--;
					iNewRow++;

					m_BoardValueMatrix[iNewRow][iColumn] = m_BoardValueMatrix[iRow][iColumn];
					m_BoardValueMatrix[iRow][iColumn] = -1;
					targetMovedCells.push_back(Cell(iNewRow, iColumn));
				}				
			}

		// generate new gems
		for(iRow = 0; iRow < m_iRowNumber; iRow++)
			for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
				if (m_BoardValueMatrix[iRow][iColumn] < 0)
				{
					m_BoardValueMatrix[iRow][iColumn] = rand() % _MAX_GEM_ID_;
					newCells.push_back(Cell(iRow, iColumn));
				}
	}	

	return bIsValidMove;
}
*/

/*
bool GameBoardManager::RecheckAfterMoveV2(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& destroyCells, std::vector<ComboEffectCell>& activatedComboEffectCells, std::vector<Cell>& destroyedByEffectCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells, std::vector<Cell>& newCells,
		std::vector<ComboEffectCell>& newComboCells)
{	
	
	int iRow, iColumn, iGemID;
	int iSameValueCellCount;
	// bool bIsValidMove = false;

	// temporary list		
	for(iRow = 0; iRow < m_iRowNumber; iRow++)
	{
		for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
		{
			m_CheckedHorizotalMatrix[iRow][iColumn] = false;
			m_CheckedVerticalMatrix[iRow][iColumn] = false;
			m_FlagDestroyedMatrix[iRow][iColumn] = false;

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
							if (m_BoardValueMatrix[iRow][(iColumn + iTranslationCell + m_iColumnNumber)%m_iColumnNumber].m_bIsBlankCell)
								iStep--;
						}
					
					}

					m_TemporaryValueMatrix[iRow][(iColumn + m_iColumnNumber + iTranslationCell)%m_iColumnNumber] = m_BoardValueMatrix[iRow][iColumn];
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
							if (m_BoardValueMatrix[(iRow + iTranslationCell + m_iRowNumber) % m_iRowNumber][iColumn].m_bIsBlankCell)
								iStep--;
						}						
					}

					m_TemporaryValueMatrix[(iRow + m_iRowNumber + iTranslationCell) % m_iRowNumber][iColumn] = m_BoardValueMatrix[iRow][iColumn];
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

	for(int i=0; i< _MAX_LINKED_BLOCK_COUNT_; i++)
	{
		m_LinkedBlockList[i].m_iCellCount = 0;
	}	

	bool bIsNewBlockCreated;
	Cell currentCheckCell;
	m_iLinkedBlockCount = 0;

	for(iRow=0; iRow < m_iRowNumber; iRow++)
	{
		for(iColumn=0; iColumn < m_iColumnNumber; iColumn++)
		{
			if (!m_TemporaryValueMatrix[iRow][iColumn].m_bIsBlankCell) //not blank cell
			{
				currentCheckCell.m_iRow = iRow;
				currentCheckCell.m_iColumn = iColumn;
				bIsNewBlockCreated = true;
				int iCurrentDestroyedCellIndex = destroyCells.size();
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
								m_LinkedBlockList[m_iLinkedBlockCount].m_iStartIndexInDestroyedList = destroyCells.size();
								m_LinkedBlockList[m_iLinkedBlockCount].m_iGemID = m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemID;
								m_iLinkedBlockCount++;
								
								bIsNewBlockCreated = false;								
							
								// add current cell to list only when new block is created
								if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
								{
									m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;

									destroyCells.push_back(Cell(currentCheckCell.m_iRow, currentCheckCell.m_iColumn));				
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;

									iCurrentDestroyedCellIndex++;
								}
							}

							for(int iRowIndex = currentCheckCell.m_iRow - 1; iRowIndex > iRowBelow; iRowIndex--)
							{
								if (!m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn])
								{
									m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn] = true;

									destroyCells.push_back(Cell(iRowIndex, currentCheckCell.m_iColumn));
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;
								}
							}				
																	
							for(int iRowIndex = currentCheckCell.m_iRow + 1; iRowIndex < iRowAbove; iRowIndex++)
							{
								if (!m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn])
								{
									m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn] = true;

									destroyCells.push_back(Cell(iRowIndex, currentCheckCell.m_iColumn));
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
								m_LinkedBlockList[m_iLinkedBlockCount].m_iStartIndexInDestroyedList = destroyCells.size();
								m_LinkedBlockList[m_iLinkedBlockCount].m_iGemID = m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemID;
								m_iLinkedBlockCount++;
								
								bIsNewBlockCreated = false;								

								// add current cell to list only when new block is created
								if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn])
								{
									m_FlagDestroyedMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn] = true;

									destroyCells.push_back(Cell(currentCheckCell.m_iRow, currentCheckCell.m_iColumn));				
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;

									iCurrentDestroyedCellIndex++;
								}
							}

							// calculate destroyed cells							
							for(int iColumnIndex= currentCheckCell.m_iColumn- 1; iColumnIndex > iColumnLeft; iColumnIndex--)
							{
								if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][ iColumnIndex])
								{
									destroyCells.push_back(Cell(currentCheckCell.m_iRow, iColumnIndex));	
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;
								}
							}

							for(int iColumnIndex= currentCheckCell.m_iColumn+ 1; iColumnIndex <iColumnRight; iColumnIndex++)
							{
								if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][ iColumnIndex])
								{
									destroyCells.push_back(Cell(currentCheckCell.m_iRow, iColumnIndex));	
									m_LinkedBlockList[m_iLinkedBlockCount-1].m_iCellCount++;
								}
							}
						}
					} //end vertical check			

					if (iCurrentDestroyedCellIndex == destroyCells.size()) //no new cells added to list ==> stop
					{
						break;
					}
					else{
						currentCheckCell = destroyCells[iCurrentDestroyedCellIndex];
						iCurrentDestroyedCellIndex++;
					}
				}
			}
		}
	}


	if (m_iLinkedBlockCount > 0)
	{			
		for(int i=0; i<m_iLinkedBlockCount; i++)
		{
			if (m_LinkedBlockList[i].m_iCellCount >3)
			{
				if (m_LinkedBlockList[i].m_iCellCount == 4)
				{
					m_ComboCountList[0]++;
					m_LinkedBlockList[i].m_eGemComboType = _GCT_COMBO4_;
				}
				else if (m_LinkedBlockList[i].m_iCellCount >= 6)
				{
					m_ComboCountList[2]++;
					m_LinkedBlockList[i].m_eGemComboType = _GCT_COMBO6_;					
				}
				else //5 cell in block
				{
					if (
						(destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList].m_iRow == destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+1].m_iRow &&
						destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+1].m_iRow == destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+2].m_iRow &&
						destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+2].m_iRow == destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+3].m_iRow &&
						destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+3].m_iRow == destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+4].m_iRow)
						||
						(destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList].m_iColumn == destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+1].m_iColumn &&
						destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+1].m_iColumn == destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+2].m_iColumn &&
						destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+2].m_iColumn == destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+3].m_iColumn &&
						destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+3].m_iColumn == destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList+4].m_iColumn))
					{
						m_ComboCountList[1]++;
						m_LinkedBlockList[i].m_eGemComboType = _GCT_COMBO5_;
					}
					else
					{
						m_ComboCountList[2]++;
						m_LinkedBlockList[i].m_eGemComboType = _GCT_COMBO6_;
					}
				}			
			}
			else
				m_LinkedBlockList[i].m_eGemComboType = _GCT_NONE_;
		}
		

		// for test
		//CountBasicCombo();

		int iRow, iColumn;

		if (iSelectedRow >= 0 || iSelectedColumn >= 0)
			for(iRow = 0; iRow < m_iRowNumber; iRow++)
				for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
					m_BoardValueMatrix[iRow][iColumn] = m_TemporaryValueMatrix[iRow][iColumn];
				
		// remove cells
		std::vector<Cell> filterDestroyedCell; //destroyCells list may duplicate!
		for(auto cell : destroyCells)
		{
			if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID >= 0)
			{
				if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType > _GCT_NONE_)
					activatedComboEffectCells.push_back(ComboEffectCell(cell, m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType));

				m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID = -1;
				m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType = _GCT_NONE_;
				filterDestroyedCell.push_back(cell);
			}
		}
		destroyCells = filterDestroyedCell;

		// active combo effect ==> destroy some cell!
		// testing combo4 effect
		for(auto cell : activatedComboEffectCells)
		{
			for(int iRow = cell.m_iRow-1; iRow <= cell.m_iRow+1; iRow++)
				for(int iColumn = cell.m_iColumn-1; iColumn <= cell.m_iColumn+1; iColumn++)
				{			
					if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber &&
						m_BoardValueMatrix[iRow][iColumn].m_iGemID >= 0)
					{
						m_BoardValueMatrix[iRow][iColumn].m_iGemID = -1;
						m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;

						destroyedByEffectCells.push_back(Cell(iRow, iColumn));
					}
				}
		}

		// create new combo cells
		for(int i=0; i< m_iLinkedBlockCount; i++)
			if (m_LinkedBlockList[i].m_iCellCount > 3)
			{
				bool bFoundCellOnActiveDragLine = false;
				Cell comboCell;
				// find position to place combo cell
				if (iSelectedRow >=0 || iSelectedColumn >=0)
				{
					for(int j=0; j< m_LinkedBlockList[i].m_iCellCount; j++)
						if ( destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList + j].m_iRow == iSelectedRow
							|| destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList + j].m_iColumn == iSelectedColumn)
						{
							bFoundCellOnActiveDragLine = true;
							comboCell = destroyCells[m_LinkedBlockList[i].m_iStartIndexInDestroyedList + j];
							break;
						}
				}
				if (!bFoundCellOnActiveDragLine)
				{
					int iRandomIndex = (rand() % m_LinkedBlockList[i].m_iCellCount) + m_LinkedBlockList[i].m_iStartIndexInDestroyedList; 
					comboCell = destroyCells[iRandomIndex];
				}
				
				m_BoardValueMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_iGemID = m_LinkedBlockList[i].m_iGemID;
				m_BoardValueMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_eGemComboType = m_LinkedBlockList[i].m_eGemComboType;

				newComboCells.push_back(ComboEffectCell(comboCell, m_LinkedBlockList[i].m_eGemComboType, m_LinkedBlockList[i].m_iGemID) );
			}

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
						m_BoardValueMatrix[iRow][iColumn].m_iGemID = -1;
						m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;
						targetMovedCells.push_back(Cell(iCheckRowIndex, iColumn));
					}
				}
			}

		// generate new gems
		for(iRow = 0; iRow < m_iRowNumber; iRow++)
			for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
				if (!m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && m_BoardValueMatrix[iRow][iColumn].m_iGemID < 0)
				{
					m_BoardValueMatrix[iRow][iColumn].m_iGemID = rand() % m_pLevelConfig->m_iNumberOfColor;
					newCells.push_back(Cell(iRow, iColumn));
				}
	}	

	return (m_iLinkedBlockCount > 0);	
}
*/
/*void GameBoardManager::LoadGameConfig()
{
	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::sharedFileUtils()->getFileData("GameConfig.txt", "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	std::string sComment;

	// read score of gem
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreOfGem;
	std::getline( inputStream, sComment);

	// read combo config
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iComboCombineRatio;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iCombEffectRatio4;
	inputStream >> m_GameConfig.m_iCombEffectRatio5;
	inputStream >> m_GameConfig.m_iCombEffectRatio6;
	inputStream >> m_GameConfig.m_iCombEffectRatio4_4;
	inputStream >> m_GameConfig.m_iCombEffectRatio4_5;
	inputStream >> m_GameConfig.m_iCombEffectRatio4_6;
	inputStream >> m_GameConfig.m_iCombEffectRatio5_5;
	inputStream >> m_GameConfig.m_iCombEffectRatio5_6;
	inputStream >> m_GameConfig.m_iCombEffectRatio6_6;
	std::getline( inputStream, sComment);

	// read word config
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreOfMainWord;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iMainWordScoreRatio;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreOfSubWord;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iSubWordScoreRatio;	

	delete[] data;
	delete[] orginalData;
}
*/

/*void GameBoardManager::LoadBasicComboPatternList()
{
	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::sharedFileUtils()->getFileData("ComboBasicPattern.txt", "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	m_iBasicComboPatternCount = 0;
	
	int iRow, iColumn;
	for(int iTypeID = 0; iTypeID <_COMBO_TYPE_COUNT_; iTypeID++)
	{
		int iCount;
		//pattern count
		inputStream >> iCount;

		for(int iPatternIndex = m_iBasicComboPatternCount; iPatternIndex < m_iBasicComboPatternCount+iCount; iPatternIndex++)
		{
			m_BasicComboPatternList[iPatternIndex].m_iTypeID = iTypeID;
			inputStream >> m_BasicComboPatternList[iPatternIndex].m_iRowNumber;
			inputStream >> m_BasicComboPatternList[iPatternIndex].m_iColumnNumber;
			
			for(iRow = 0; iRow < m_BasicComboPatternList[iPatternIndex].m_iRowNumber; iRow++)
				for(iColumn = 0; iColumn < m_BasicComboPatternList[iPatternIndex].m_iColumnNumber; iColumn++)
				{
					inputStream >>  m_BasicComboPatternList[iPatternIndex].m_Pattern[iRow][iColumn];
				}
		}

		m_iBasicComboPatternCount += iCount;
	}

	delete[] data;
	delete[] orginalData;
}

void GameBoardManager::CountBasicCombo()
{
	int iRow, iColumn, iPatternIndex, iCheckingGemID, iCheckRow, iCheckColumn;
	for(iRow = 0; iRow < m_iRowNumber; iRow++)
		for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
			for(iPatternIndex = 0; iPatternIndex < m_iBasicComboPatternCount; iPatternIndex++)
		{
			if (iRow + m_BasicComboPatternList[iPatternIndex].m_iRowNumber < m_iRowNumber
				&& iColumn + m_BasicComboPatternList[iPatternIndex].m_iColumnNumber < m_iColumnNumber)
			{
				iCheckingGemID = -1;
				BasicComboPattern& pattern = m_BasicComboPatternList[iPatternIndex];
				bool bIsPatternMissMatch = false;

				for(iCheckRow=0; iCheckRow < pattern.m_iRowNumber; iCheckRow++)
					for(iCheckColumn=0; iCheckColumn < pattern.m_iColumnNumber; iCheckColumn++)
					{
						if (pattern.m_Pattern[iCheckRow][iCheckColumn] != 0)
						{
							if (m_TemporaryValueMatrix[iRow + iCheckRow][iColumn + iCheckColumn].m_bIsBlankCell)//miss-match		
							{
								bIsPatternMissMatch = true;
							}
							else
							{
								if (iCheckingGemID < 0)
									iCheckingGemID = m_TemporaryValueMatrix[iRow + iCheckRow][iColumn + iCheckColumn].m_iGemID;
								else if (iCheckingGemID != m_TemporaryValueMatrix[iRow + iCheckRow][iColumn + iCheckColumn].m_iGemID) //miss-match
								{
									bIsPatternMissMatch  = true;
								}
							}
						}

						if (bIsPatternMissMatch) //exit checking this pattern
						{
							iCheckRow = pattern.m_iRowNumber;
							iCheckColumn = pattern.m_iColumnNumber;
						}
					}

					if (!bIsPatternMissMatch)
					{
						m_ComboCountList[pattern.m_iTypeID]++;
					}
			}
		}

}*/