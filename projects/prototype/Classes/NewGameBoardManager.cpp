#include "NewGameBoardManager.h"
#include "cocos2d.h"

bool NewGameBoardManager::RecheckAfterMoveV2(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboEffectBundle>& doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList,std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<Cell>& newCells)
{

	//std::vector<Cell> basicMatchingDestroyedCells;

	int iRow, iColumn, iGemID;
	int iSameValueCellCount;
	// bool bIsValidMove = false;

	// create temporary list to compute result for shifting move
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

	// ************* create double combo ****************************************************	
	std::vector<ComboEffectBundle*> chainEffectFromDoubleCombo;

	for(iRow=0; iRow < m_iRowNumber; iRow++)
	{
		for(iColumn=0; iColumn < m_iColumnNumber; iColumn++)
		{
			if ( m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_NONE_)
			{
				bool bFoundDoubleCombo = false;
				Cell nextCell;			
				if (iRow < m_iRowNumber-1 && m_TemporaryValueMatrix[iRow+1][iColumn].m_eGemComboType)
				{
					bFoundDoubleCombo = true;
					nextCell.m_iRow = iRow + 1;
					nextCell.m_iColumn = iColumn;
				}
				else if (iColumn < m_iColumnNumber-1 && m_TemporaryValueMatrix[iRow][iColumn+1].m_eGemComboType)
				{
					bFoundDoubleCombo = true;
					nextCell.m_iRow = iRow ;
					nextCell.m_iColumn = iColumn + 1;
				}

				if (bFoundDoubleCombo)
				{
					DoubleComboEffectBundle doubleCombo;
					doubleCombo.m_CreationInfo.m_Cell1 = Cell(iRow, iColumn);
					doubleCombo.m_CreationInfo.m_Cell2 = nextCell;
					doubleCombo.m_CreationInfo.m_Position = doubleCombo.m_CreationInfo.m_Cell1;
					doubleCombo.m_CreationInfo.m_eDoubleComboType = _DCT_DEFAULT;

					doubleComboList.push_back(doubleCombo);

					// disable 2 combo cell
					m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;
					m_TemporaryValueMatrix[nextCell.m_iRow][nextCell.m_iColumn].m_eGemComboType = _GCT_NONE_;
					
					// effect of double combo will be execute in combo chain!!!
					ComboEffectBundle* pComboEffectBundle = new ComboEffectBundle();
					pComboEffectBundle->m_iPhase = 1;
					pComboEffectBundle->m_ComboEffectDescription.m_eComboEffectType = _CET_DOUBLE_EXPLOSION_;
					pComboEffectBundle->m_ComboEffectDescription.m_Position = doubleCombo.m_CreationInfo.m_Position;
									
					chainEffectFromDoubleCombo.push_back(pComboEffectBundle);
				}
			}
		}
	}


	// ************* create block for basic matching ******************************************

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

	if (m_iLinkedBlockCount > 0 || doubleComboList.size() > 0)
	{
		// Update main board matrix from temporary matrix if it's valid move
		//if ( iSelectedRow >=0 || iSelectedColumn >= 0)		
		{
			int iRow, iColumn;

			for(iRow = 0; iRow < m_iRowNumber; iRow++)
				for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
					m_BoardValueMatrix[iRow][iColumn] = m_TemporaryValueMatrix[iRow][iColumn];
		}

		// remove cells by basic matching and create first combo chain
		for(auto cell : basicMatchingDestroyedCells)
		{
			if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID >= 0)
			{		
				// NOTE: destroyed cell include double combo cells!!! ????
				// 1 cell affected by 2 effects (basic matching and double combo) ==> how to display those effect???		
				if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType != _GCT_NONE_)
				{				
					ComboEffectBundle* pTriggeredComboEffectBundle = new ComboEffectBundle();
					pTriggeredComboEffectBundle->m_ComboEffectDescription.m_eComboEffectType = _CET_EXPLOSION_; //only implement explosion now
					pTriggeredComboEffectBundle->m_ComboEffectDescription.m_Position = cell;
					pTriggeredComboEffectBundle->m_iPhase = 0;														

					comboChainList.push_back(pTriggeredComboEffectBundle);
				}

				m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID = -1;
				m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType = _GCT_NONE_;				
			}
		}	
	
		// add effect of double combo to chain
		comboChainList.insert(comboChainList.end(), chainEffectFromDoubleCombo.begin(), chainEffectFromDoubleCombo.end());		

		// trigger combo chain from effect of triggered combos
		ExecuteComboChain(comboChainList);

		// create combo cells	
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
						linkedBlock.m_eGemComboType = _GCT_COMBO4_;
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
							if ( basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + j].m_iRow == iSelectedRow
								|| basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + j].m_iColumn == iSelectedColumn)
							{
								bFoundCellOnActiveDragLine = true;
								comboCell = basicMatchingDestroyedCells[linkedBlock.m_iStartIndexInDestroyedList + j];
								break;
							}
					}
					if (!bFoundCellOnActiveDragLine)
					{
						int iRandomIndex = (rand() % linkedBlock.m_iCellCount) + linkedBlock.m_iStartIndexInDestroyedList; 
						comboCell = basicMatchingDestroyedCells[iRandomIndex];
					}
				
					m_BoardValueMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_iGemID = linkedBlock.m_iGemID;
					m_BoardValueMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_eGemComboType = linkedBlock.m_eGemComboType;

					newComboCells.push_back(ComboEffectCell(comboCell, linkedBlock.m_eGemComboType, linkedBlock.m_iGemID) );					
				}
			}
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
					m_BoardValueMatrix[iRow][iColumn].m_iGemID = rand() % _MAX_GEM_ID_;
					newCells.push_back(Cell(iRow, iColumn));
				}

		return true;
	}

	return false;
}

void NewGameBoardManager::ExecuteComboChain(std::vector<ComboEffectBundle*>& comboChainList)
{
	int iComboIndexInChain = 0;
	ComboEffectBundle* pComboInChain;
	while (iComboIndexInChain < comboChainList.size())
	{
		pComboInChain = comboChainList.at(iComboIndexInChain);
		Cell position = pComboInChain->m_ComboEffectDescription.m_Position;

		// demo explosion effects
		int iRowDelta, iColumnDelta;
		if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DOUBLE_EXPLOSION_)
			iRowDelta = iColumnDelta = 2;
		else
			iRowDelta = iColumnDelta = 1;

		// trigger explostion effect: temporary implementation
		for(int iRow = position.m_iRow-iRowDelta; iRow <= position.m_iRow+iRowDelta; iRow++)
			for(int iColumn = position.m_iColumn-iColumnDelta; iColumn <= position.m_iColumn+iColumnDelta; iColumn++)
		{			
			if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber &&
				m_BoardValueMatrix[iRow][iColumn].m_iGemID >= 0)
			{				
				// this may trigger another combo ?
				if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType >=0 && m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_NONE_)
				{
					ComboEffectBundle* pNextTriggeredCombo = new ComboEffectBundle();
					pNextTriggeredCombo->m_ComboEffectDescription.m_eComboEffectType = _CET_EXPLOSION_;
					pNextTriggeredCombo->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
					pNextTriggeredCombo->m_iPhase =  pComboInChain->m_iPhase + 1;

					pComboInChain->m_TriggeredComboEfectBundleList.push_back(pNextTriggeredCombo);
								
					// add it to chain to trigger later
					comboChainList.push_back(pNextTriggeredCombo);
				}		

				m_BoardValueMatrix[iRow][iColumn].m_iGemID = -1;
				m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;

				pComboInChain->m_DestroyedCells.push_back(Cell(iRow, iColumn));
			}
		}	

		iComboIndexInChain++;
	}
}