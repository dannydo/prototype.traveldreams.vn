#include "NewGameBoardManager.h"
#include "cocos2d.h"

#define _MIN_(a,b) ((a)<(b))?(a):(b)
#define _MAX_(a,b) ((a)>(b))?(a):(b)

NewGameBoardManager::NewGameBoardManager() : GameBoardManager()
{
	m_iCurrentScore = 0;		
	m_pObstacleProcessManager = new ObstacleProcessManager(this);
	m_iPhaseMoveInComboChain = 0;
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


	// generate bonus quest
	m_BonusQuestManager.m_pGameBoardManager = this;
	m_BonusQuestManager.InitLevel();
	if (m_pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount > 0 && m_pLevelConfig->m_BonusQuestConfig.m_bIsBonusGemAppearOnStartGame)
	{
		for(int i=0; i< m_pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount; i++)
		{
			CellValue& cellValue = m_BoardValueMatrix[m_pLevelConfig->m_BonusQuestConfig.m_PositionOfBonusGemAtStartGame[i].m_iRow][m_pLevelConfig->m_BonusQuestConfig.m_PositionOfBonusGemAtStartGame[i].m_iColumn];
			if (!cellValue.m_bIsBlankCell && cellValue.m_eGemComboType == _GCT_NONE_ && cellValue.m_iGemLetterBlockID < 0 && cellValue.m_iObstacleBlockID < 0)
			{
				cellValue.m_iGemID = _BONUS_QUEST_GEM_ID_;
			}
		}
	}

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


bool NewGameBoardManager::HasBonusQuestGemOnBoard()
{
	int iRow, iColumn;
	for(iRow =0; iRow < m_iRowNumber; iRow++)
		for(iColumn =0; iColumn < m_iColumnNumber; iColumn++)
		{
			if (!m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && m_BoardValueMatrix[iRow][iColumn].m_iGemID == _BONUS_QUEST_GEM_ID_)
			{
				return true;
			}
		}
	return false;
}

void NewGameBoardManager::ClearBonusQuestGemOnBoard(std::vector<Cell>& basicMatchingDestroyedCells, 		
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,	std::vector<NewCellInfo>& newCells)
{
	m_iPhaseMoveInComboChain = 1;

	int iRow, iColumn;
	for(iRow =0; iRow < m_iRowNumber; iRow++)
		for(iColumn =0; iColumn < m_iColumnNumber; iColumn++)
		{
			if (!m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && m_BoardValueMatrix[iRow][iColumn].m_iGemID == _BONUS_QUEST_GEM_ID_)
			{
				m_BoardValueMatrix[iRow][iColumn].Reset();
				basicMatchingDestroyedCells.push_back(Cell(iRow, iColumn));				
			}
		}
			

	// calculate move cells and create new cells
	CalculateMoveCells( originalMovedCells, targetMovedCells);

	// generate new cells
	GenerateNewGems(newCells, false);
}

bool NewGameBoardManager::RecheckAfterMoveV2(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& basicMatchingDestroyedCells, //std::vector<DoubleComboCreationInfo>& newDoubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList,// std::vector<ComboEffectBundle*>& triggeredCombo6ChainList,
		std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<NewCellInfo>& unlockedLetterCells,
		std::vector<NewCellInfo>& newCells, bool bIsNewMove)
{
	if (bIsNewMove)
		m_iPhaseMoveInComboChain = 1;
	else
		m_iPhaseMoveInComboChain++;

	// reset
	m_iLinkedBlockCount = 0;
	m_UnlockedGemLetterCellList.clear();
	m_bIsBossStateChanged = false;
	m_LevelBossInfo.m_bJustReleaseALetter = false;
	

	int iRow, iColumn;

	// if this phase trigger only by waiting combo 6 then not need to create temporary list
	if (!(iSelectedRow < 0 && iSelectedColumn < 0 && m_WaitingTriggerSecondTimeComboList.size()>0))
	{
		// create temporary list to compute result for shifting move
		CopyDataToTempBoardMatrixAndResetFlags( iSelectedRow, iSelectedColumn, iDeltaRow, iDeltaColumn);

		// process double combo and tripple combo		
		std::vector<ComboEffectDescription> advanceComboList;
		FindSortAndFilterAdvanceCombos(advanceComboList, iSelectedRow, iSelectedColumn);	

		if (advanceComboList.size() > 0)
		{
			// compy back values of tempMatrix to boardMatrix
			memcpy( m_BoardValueMatrix, m_TemporaryValueMatrix, sizeof(m_TemporaryValueMatrix));		

			std::vector<ComboEffectBundle*> triggerAdvanceComboList;
			TriggerAdvanceComboList( advanceComboList, comboChainList, triggerAdvanceComboList, false);
			comboChainList.insert(comboChainList.begin(), triggerAdvanceComboList.begin(), triggerAdvanceComboList.end());		
			
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
	}	


	// check for execution of combo 6 (only activated directly by user)
	if ((iSelectedRow >=0 && iDeltaColumn !=0) || (iSelectedColumn >= 0 && iDeltaRow != 0))
	{
		// ******** note: temporary hardcode and fix condition for combo 6 ************* 					
		if (iSelectedRow >= 0)
			for(iColumn =0; iColumn < m_iColumnNumber; iColumn++)
			{
				if (m_TemporaryValueMatrix[iSelectedRow][iColumn].m_eGemComboType == _GCT_COMBO6_
					&& ( (iSelectedRow>0 &&  m_TemporaryValueMatrix[iSelectedRow-1][iColumn].m_bIsBlankCell == false) ||
					(iSelectedRow< m_iRowNumber-1 &&  m_TemporaryValueMatrix[iSelectedRow+1][iColumn].m_bIsBlankCell == false)))					
				{										
					ComboEffectDescription comboEffectDescription;
					comboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_TemporaryValueMatrix[iSelectedRow][iColumn].m_eGemComboType,  iSelectedRow, iSelectedColumn); // _CET_EXPLOSION_;
					comboEffectDescription.m_Position = Cell(iSelectedRow, iColumn);
					comboEffectDescription.m_iGemID = m_TemporaryValueMatrix[iSelectedRow][iColumn].m_iGemID;					
					m_WaitingTriggerSecondTimeComboList.push_back(comboEffectDescription);

					m_TemporaryValueMatrix[iSelectedRow][iColumn].m_eGemComboType = _GCT_COMBO6_WAITING_TRIGGER_;
				}
			}
		else if (iSelectedColumn >= 0)
			for(iRow =0; iRow < m_iRowNumber; iRow++)
			{
				if (m_TemporaryValueMatrix[iRow][iSelectedColumn].m_eGemComboType == _GCT_COMBO6_
					&& ( (iSelectedColumn>0 &&  m_TemporaryValueMatrix[iRow][iSelectedColumn-1].m_bIsBlankCell == false) ||
					(iSelectedColumn< m_iColumnNumber-1 &&  m_TemporaryValueMatrix[iRow][iSelectedColumn+1].m_bIsBlankCell == false)))
				{						
					ComboEffectDescription comboEffectDescription;
					comboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_TemporaryValueMatrix[iRow][iSelectedColumn].m_eGemComboType, iSelectedRow, iSelectedColumn); // _CET_EXPLOSION_;
					comboEffectDescription.m_Position = Cell(iRow, iSelectedColumn);
					comboEffectDescription.m_iGemID = m_TemporaryValueMatrix[iRow][iSelectedColumn].m_iGemID;					
					m_WaitingTriggerSecondTimeComboList.push_back(comboEffectDescription);

					m_TemporaryValueMatrix[iRow][iSelectedColumn].m_eGemComboType = _GCT_COMBO6_WAITING_TRIGGER_;
				}
			}

		if (m_WaitingTriggerSecondTimeComboList.size() > 0)
		{		
			memcpy( m_BoardValueMatrix, m_TemporaryValueMatrix, sizeof(m_TemporaryValueMatrix));		
		}
	}

	// activate marked combo 6 from last check
	//if (iSelectedRow < 0 && iSelectedColumn < 0 &&)
	if (m_WaitingTriggerSecondTimeComboList.size()>0)
	{
		//TriggerWaitingCombo6List(comboChainList, triggeredCombo6ChainList);

		std::vector<ComboEffectBundle*> triggerAdvanceComboList;
		TriggerAdvanceComboList( m_WaitingTriggerSecondTimeComboList, comboChainList, triggerAdvanceComboList, true);
		comboChainList.insert(comboChainList.begin(), triggerAdvanceComboList.begin(), triggerAdvanceComboList.end());				

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
	
	//int iSameValueCellCount;
	// bool bIsValidMove = false;

	// create temporary list to compute result for shifting move
	//CopyDataToTempBoardMatrixAndResetFlags( iSelectedRow, iSelectedColumn, iDeltaRow, iDeltaColumn);


	// ************* create block for basic matching ******************************************	
	CreateBlockForBasicMatching( basicMatchingDestroyedCells);

	// **********************************
	if (m_iLinkedBlockCount > 0)
	{
		// for test
		//CountBasicCombo();

		// Update main board matrix from temporary matrix if it's valid move
		memcpy( m_BoardValueMatrix, m_TemporaryValueMatrix, sizeof(m_TemporaryValueMatrix));
		//if ( iSelectedRow >=0 || iSelectedColumn >= 0)				

		// remove cells by basic matching and create first combo chain		
		RemoveCellsByBasicMatching( basicMatchingDestroyedCells, comboChainList, iSelectedRow, iSelectedColumn);
	
		// trigger combo chain from effect of triggered combos
		ExecuteComboChain(comboChainList);
		
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
		std::vector<ComboEffectCell>& candidateDestroyedCells)
{
	// create temporary list to compute result for shifting move
	CopyDataToTempBoardMatrixAndResetFlags( iSelectedRow, iSelectedColumn, iDeltaRow, iDeltaColumn);

	// checking
	int iRow, iColumn, iGemID, iSameValueCellCount;
	Cell currentCheckCell, loopCell( iSelectedRow, iSelectedColumn);
	iRow = iSelectedRow;
	iColumn = iSelectedColumn;
	int iLimitRow = iSelectedRow, iLimitColumn = iSelectedColumn;

	if (iSelectedRow >= 0)
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

	// ************** first, check double/triple combo *******************
	// step: find all tripple/double combos => sort combos ==> clean combos (not tripple 6 and double 6 is very special and should be executed alone
	std::vector<ComboEffectDescription> tempAdvanceComboList;
	ComboEffectCell linkCellList[3];	
	int iLinkCellCount;
	int iIndexRow = iRow, iIndexColumn = iColumn;
	ComboEffectDescription tempComboDescription;
	int i,j;

	while (iIndexRow != iLimitRow || iIndexColumn != iLimitColumn)
	{
		if (IsSimpleCombo(m_TemporaryValueMatrix[iIndexRow][iIndexColumn].m_eGemComboType))
		{	
			linkCellList[0].m_iRow = iIndexRow;
			linkCellList[0].m_iColumn = iIndexColumn;
			linkCellList[0].m_iGemID = m_TemporaryValueMatrix[iIndexRow][iIndexColumn].m_iGemID;
			linkCellList[0].m_eGemComboType = m_TemporaryValueMatrix[iIndexRow][iIndexColumn].m_eGemComboType;
			iLinkCellCount = 1;

			// check horizontal
			if (iIndexColumn < m_iColumnNumber-1 && IsSimpleCombo(m_TemporaryValueMatrix[iIndexRow][iIndexColumn+1].m_eGemComboType))
			{
				linkCellList[iLinkCellCount].m_iRow = iIndexRow;
				linkCellList[iLinkCellCount].m_iColumn = iIndexColumn+1;
				linkCellList[iLinkCellCount].m_iGemID = m_TemporaryValueMatrix[iIndexRow][iIndexColumn+1].m_iGemID;
				linkCellList[iLinkCellCount].m_eGemComboType = m_TemporaryValueMatrix[iIndexRow][iIndexColumn+1].m_eGemComboType;
				iLinkCellCount++;
			}

			if (iIndexColumn > 0 && IsSimpleCombo(m_TemporaryValueMatrix[iIndexRow][iIndexColumn-1].m_eGemComboType))
			{
				linkCellList[iLinkCellCount].m_iRow = iIndexRow;
				linkCellList[iLinkCellCount].m_iColumn = iIndexColumn-1;
				linkCellList[iLinkCellCount].m_iGemID = m_TemporaryValueMatrix[iIndexRow][iIndexColumn-1].m_iGemID;
				linkCellList[iLinkCellCount].m_eGemComboType = m_TemporaryValueMatrix[iIndexRow][iIndexColumn-1].m_eGemComboType;
				iLinkCellCount++;
			}

			if (iLinkCellCount >= 2)
			{					
				tempComboDescription.m_eComboEffectType = GetComboEffectTypeOfAdvanceCombo(linkCellList, iLinkCellCount, iSelectedRow, iSelectedColumn);
				tempComboDescription.m_Position = (Cell)linkCellList[0];
				tempComboDescription.m_iGemID = linkCellList[0].m_iGemID;
				tempComboDescription.m_Phase0CellCount = iLinkCellCount-1;
				for(i=0; i < iLinkCellCount-1;i++)
				{
					tempComboDescription.m_Phase0CellList[i] = (Cell)linkCellList[i+1];
					tempComboDescription.m_Phase0CellList[i].m_iGroupIndex = i;
				}
				tempAdvanceComboList.push_back(tempComboDescription);
			}

			// reset
			iLinkCellCount = 1;

			// check vertical
			if (iIndexRow < m_iRowNumber-1 && IsSimpleCombo(m_TemporaryValueMatrix[iIndexRow+1][iIndexColumn].m_eGemComboType))
			{
				linkCellList[iLinkCellCount].m_iRow = iIndexRow+1;
				linkCellList[iLinkCellCount].m_iColumn = iIndexColumn;
				linkCellList[iLinkCellCount].m_iGemID = m_TemporaryValueMatrix[iIndexRow+1][iIndexColumn].m_iGemID;
				linkCellList[iLinkCellCount].m_eGemComboType = m_TemporaryValueMatrix[iIndexRow+1][iIndexColumn].m_eGemComboType;
				iLinkCellCount++;
			}
			if (iIndexRow > 0 && IsSimpleCombo(m_TemporaryValueMatrix[iIndexRow-1][iIndexColumn].m_eGemComboType))
			{
				linkCellList[iLinkCellCount].m_iRow = iIndexRow - 1;
				linkCellList[iLinkCellCount].m_iColumn = iIndexColumn;
				linkCellList[iLinkCellCount].m_iGemID = m_TemporaryValueMatrix[iIndexRow-1][iIndexColumn].m_iGemID;
				linkCellList[iLinkCellCount].m_eGemComboType = m_TemporaryValueMatrix[iIndexRow-1][iIndexColumn].m_eGemComboType;
				iLinkCellCount++;
			}
		
			if (iLinkCellCount >= 2)
			{					
				tempComboDescription.m_eComboEffectType = GetComboEffectTypeOfAdvanceCombo(linkCellList, iLinkCellCount, iSelectedRow, iSelectedColumn);
				tempComboDescription.m_Position = (Cell)linkCellList[0];
				tempComboDescription.m_iGemID = linkCellList[0].m_iGemID;
				tempComboDescription.m_Phase0CellCount = iLinkCellCount-1;
				for(i=0; i < iLinkCellCount-1;i++)
				{
					tempComboDescription.m_Phase0CellList[i] = (Cell)linkCellList[i+1];
					tempComboDescription.m_Phase0CellList[i].m_iGroupIndex = i;
				}
				tempAdvanceComboList.push_back(tempComboDescription);
			}			
		}

		iIndexRow += iDeltaRow;
		iIndexColumn += iDeltaColumn;
	}
	// sort advance combo list
	int iListSize = tempAdvanceComboList.size();	
	for(i=0; i< iListSize-1; i++)
		for(j=i+1; j< iListSize; j++)
			if (tempAdvanceComboList[i].m_eComboEffectType < tempAdvanceComboList[j].m_eComboEffectType)
			{
				tempComboDescription = tempAdvanceComboList[i];
				tempAdvanceComboList[i] = tempAdvanceComboList[j];
				tempAdvanceComboList[j] = tempComboDescription;
			}
	// clean list
	if (iListSize > 0)
	{
		//std::vector<ComboEffectDescription> advanceComboList;

		// if there's combo 6-6-6 or 6-6 at the first of list then disable all other advance combos!!!!
		if (tempAdvanceComboList[0].m_eComboEffectType == _CET_6_6_6_EFFECT_ || tempAdvanceComboList[0].m_eComboEffectType == _CET_6_6_EFFECT_)
		{
			tempComboDescription = tempAdvanceComboList[0];
			tempAdvanceComboList.clear();
			tempAdvanceComboList.push_back(tempComboDescription);
		}

		for(auto& comboDescription : tempAdvanceComboList)
		{
			if (m_TemporaryValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn].m_eGemComboType != _GCT_NONE_
				&& m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[0].m_iRow][comboDescription.m_Phase0CellList[0].m_iColumn].m_eGemComboType != _GCT_NONE_
				&& (comboDescription.m_Phase0CellCount == 1 || m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[1].m_iRow][comboDescription.m_Phase0CellList[1].m_iColumn].m_eGemComboType != _GCT_NONE_))
			{
				// update combo count of bonus quest
				m_BonusQuestManager.IncreaseComboCellCountForBonusQuest( m_TemporaryValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn].m_eGemComboType);
				m_BonusQuestManager.IncreaseComboCellCountForBonusQuest( m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[0].m_iRow][comboDescription.m_Phase0CellList[0].m_iColumn].m_eGemComboType);
				if (comboDescription.m_Phase0CellCount > 1)
				{
					m_BonusQuestManager.IncreaseComboCellCountForBonusQuest( m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[1].m_iRow][comboDescription.m_Phase0CellList[1].m_iColumn].m_eGemComboType);					
				}

				// must add to list before clear cell
				candidateDestroyedCells.push_back( ComboEffectCell( comboDescription.m_Position, m_TemporaryValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn].m_eGemComboType, m_TemporaryValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn].m_iGemID));
				for(i=0; i< comboDescription.m_Phase0CellCount; i++)
					candidateDestroyedCells.push_back( ComboEffectCell( comboDescription.m_Phase0CellList[i], m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[i].m_iRow][comboDescription.m_Phase0CellList[i].m_iColumn].m_eGemComboType, m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[i].m_iRow][comboDescription.m_Phase0CellList[i].m_iColumn].m_iGemID));

				// clear cells
				m_TemporaryValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn].Reset();
				m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[0].m_iRow][comboDescription.m_Phase0CellList[0].m_iColumn].Reset();
				if (comboDescription.m_Phase0CellCount > 1)
				{
					m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[1].m_iRow][comboDescription.m_Phase0CellList[1].m_iColumn].Reset();
				}

				//advanceComboList.push_back(comboDescription);				
			}		
		}

		return true;
	}

	// ********************** if not, check combo 6 (if delta = 0 then not check combo 6) ********************** 	
	if ((iSelectedRow >=0 && iDeltaColumn !=0) || (iSelectedColumn >= 0 && iDeltaRow != 0))
	{		
		// ******** note: temporary hardcode and fix condition for combo 6 ************* 					
		if (iSelectedRow >= 0)
			for(iIndexColumn =0; iIndexColumn < m_iColumnNumber; iIndexColumn++)
			{
				if (m_TemporaryValueMatrix[iSelectedRow][iIndexColumn].m_eGemComboType == _GCT_COMBO6_
					&& ( (iSelectedRow>0 &&  m_TemporaryValueMatrix[iSelectedRow-1][iIndexColumn].m_bIsBlankCell == false) ||
					(iSelectedRow< m_iRowNumber-1 &&  m_TemporaryValueMatrix[iSelectedRow+1][iIndexColumn].m_bIsBlankCell == false)))
				{					
					candidateDestroyedCells.push_back(ComboEffectCell( Cell(iSelectedRow, iIndexColumn), m_TemporaryValueMatrix[iSelectedRow][iIndexColumn].m_eGemComboType,
									m_TemporaryValueMatrix[iSelectedRow][iIndexColumn].m_iGemID));
					if (iSelectedRow>0 &&  m_TemporaryValueMatrix[iSelectedRow-1][iIndexColumn].m_bIsBlankCell == false)
						candidateDestroyedCells.push_back( ComboEffectCell( Cell(iSelectedRow-1, iIndexColumn), 
									m_TemporaryValueMatrix[iSelectedRow-1][iIndexColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[iSelectedRow-1][iIndexColumn].m_eGemComboType,
									m_TemporaryValueMatrix[iSelectedRow-1][iIndexColumn].m_iGemID));
					if (iSelectedRow< m_iRowNumber-1 &&  m_TemporaryValueMatrix[iSelectedRow+1][iIndexColumn].m_bIsBlankCell == false)
						candidateDestroyedCells.push_back( ComboEffectCell( Cell(iSelectedRow+1, iIndexColumn), 
									m_TemporaryValueMatrix[iSelectedRow+1][iIndexColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[iSelectedRow+1][iIndexColumn].m_eGemComboType,
									m_TemporaryValueMatrix[iSelectedRow+1][iIndexColumn].m_iGemID));
					/*ComboEffectDescription comboEffectDescription;
					comboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_TemporaryValueMatrix[iSelectedRow][iColumn].m_eGemComboType,  iSelectedRow, iSelectedColumn); // _CET_EXPLOSION_;
					comboEffectDescription.m_Position = Cell(iSelectedRow, iColumn);
					comboEffectDescription.m_iGemID = m_TemporaryValueMatrix[iSelectedRow][iColumn].m_iGemID;					
					m_WaitingTriggerSecondTimeComboList.push_back(comboEffectDescription);

					m_TemporaryValueMatrix[iSelectedRow][iColumn].m_eGemComboType = _GCT_COMBO6_WAITING_TRIGGER_;*/
				}
			}
		else if (iSelectedColumn >= 0)
			for(iIndexRow =0; iIndexRow < m_iRowNumber; iIndexRow++)
			{
				if (m_TemporaryValueMatrix[iIndexRow][iSelectedColumn].m_eGemComboType == _GCT_COMBO6_
					&& ( (iSelectedColumn>0 &&  m_TemporaryValueMatrix[iIndexRow][iSelectedColumn-1].m_bIsBlankCell == false) ||
					(iSelectedColumn< m_iColumnNumber-1 &&  m_TemporaryValueMatrix[iIndexRow][iSelectedColumn+1].m_bIsBlankCell == false)))
				{				
					candidateDestroyedCells.push_back(ComboEffectCell( Cell(iIndexRow, iSelectedColumn), m_TemporaryValueMatrix[iIndexRow][iSelectedColumn].m_eGemComboType,
									m_TemporaryValueMatrix[iIndexRow][iSelectedColumn].m_iGemID));
					if (iSelectedColumn>0 &&  m_TemporaryValueMatrix[iIndexRow][iSelectedColumn-1].m_bIsBlankCell == false)
						candidateDestroyedCells.push_back(ComboEffectCell( Cell(iIndexRow, iSelectedColumn-1), 
									m_TemporaryValueMatrix[iIndexRow][iSelectedColumn-1].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[iIndexRow][iSelectedColumn-1].m_eGemComboType,
									m_TemporaryValueMatrix[iIndexRow][iSelectedColumn-1].m_iGemID));
					if (iSelectedColumn< m_iColumnNumber-1 &&  m_TemporaryValueMatrix[iIndexRow][iSelectedColumn+1].m_bIsBlankCell == false)
						candidateDestroyedCells.push_back(ComboEffectCell( Cell(iIndexRow, iSelectedColumn+1), 
									m_TemporaryValueMatrix[iIndexRow][iSelectedColumn+1].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[iIndexRow][iSelectedColumn+1].m_eGemComboType,
									m_TemporaryValueMatrix[iIndexRow][iSelectedColumn+1].m_iGemID));
					/*ComboEffectDescription comboEffectDescription;
					comboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_TemporaryValueMatrix[iRow][iSelectedColumn].m_eGemComboType, iSelectedRow, iSelectedColumn); // _CET_EXPLOSION_;
					comboEffectDescription.m_Position = Cell(iRow, iSelectedColumn);
					comboEffectDescription.m_iGemID = m_TemporaryValueMatrix[iRow][iSelectedColumn].m_iGemID;					
					m_WaitingTriggerSecondTimeComboList.push_back(comboEffectDescription);

					m_TemporaryValueMatrix[iRow][iSelectedColumn].m_eGemComboType = _GCT_COMBO6_WAITING_TRIGGER_;*/
				}
			}

		if (candidateDestroyedCells.size() >0)
			return true;
	}

	// ********************** final, check basic matching ********************** 
	bool bIsNewBlockCreated;

	while (iRow != iLimitRow || iColumn != iLimitColumn)
	{	
		if (!m_TemporaryValueMatrix[iRow][iColumn].m_bIsBlankCell && m_TemporaryValueMatrix[iRow][iColumn].m_iGemID != _SPECIAL_COMBO_GEM_ID) //not blank cell
		{
			currentCheckCell.m_iRow = iRow;
			currentCheckCell.m_iColumn = iColumn;				
			int iCurrentDestroyedCellIndex = candidateDestroyedCells.size();
			
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

								candidateDestroyedCells.push_back( ComboEffectCell( Cell(currentCheckCell.m_iRow, currentCheckCell.m_iColumn),
									m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_eGemComboType,  
									m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemID));

								iCurrentDestroyedCellIndex++;
							}
						}

						for(int iRowIndex = currentCheckCell.m_iRow - 1; iRowIndex > iRowBelow; iRowIndex--)
						{
							if (!m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn])
							{
								m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn] = true;

								candidateDestroyedCells.push_back( ComboEffectCell( Cell(iRowIndex, currentCheckCell.m_iColumn),
									m_TemporaryValueMatrix[iRowIndex][currentCheckCell.m_iColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[iRowIndex][currentCheckCell.m_iColumn].m_eGemComboType,  
									m_TemporaryValueMatrix[iRowIndex][currentCheckCell.m_iColumn].m_iGemID));								
							}
						}				
																	
						for(int iRowIndex = currentCheckCell.m_iRow + 1; iRowIndex < iRowAbove; iRowIndex++)
						{
							if (!m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn])
							{
								m_FlagDestroyedMatrix[iRowIndex][currentCheckCell.m_iColumn] = true;

								candidateDestroyedCells.push_back( ComboEffectCell( Cell(iRowIndex, currentCheckCell.m_iColumn),
									m_TemporaryValueMatrix[iRowIndex][currentCheckCell.m_iColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[iRowIndex][currentCheckCell.m_iColumn].m_eGemComboType,  
									m_TemporaryValueMatrix[iRowIndex][currentCheckCell.m_iColumn].m_iGemID));								
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

								candidateDestroyedCells.push_back( ComboEffectCell( Cell(currentCheckCell.m_iRow, currentCheckCell.m_iColumn),
									m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_eGemComboType, 
									m_TemporaryValueMatrix[currentCheckCell.m_iRow][currentCheckCell.m_iColumn].m_iGemID));

								iCurrentDestroyedCellIndex++;
							}
						}

						// calculate destroyed cells							
						for(int iColumnIndex= currentCheckCell.m_iColumn- 1; iColumnIndex > iColumnLeft; iColumnIndex--)
						{
							if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][ iColumnIndex])
							{
								candidateDestroyedCells.push_back( ComboEffectCell( Cell(currentCheckCell.m_iRow, iColumnIndex),
									m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnIndex].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnIndex].m_eGemComboType, 
									m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnIndex].m_iGemID));					
							}
						}

						for(int iColumnIndex= currentCheckCell.m_iColumn+ 1; iColumnIndex <iColumnRight; iColumnIndex++)
						{
							if (!m_FlagDestroyedMatrix[currentCheckCell.m_iRow][ iColumnIndex])
							{
								candidateDestroyedCells.push_back( ComboEffectCell( Cell(currentCheckCell.m_iRow, iColumnIndex),
									m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnIndex].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnIndex].m_eGemComboType, 
									m_TemporaryValueMatrix[currentCheckCell.m_iRow][iColumnIndex].m_iGemID));
							}
						}
					}
				} //end vertical check			

				if (iCurrentDestroyedCellIndex == candidateDestroyedCells.size()) //no new cells added to list ==> stop
				{
					break;
				}
				else{
					currentCheckCell = candidateDestroyedCells[iCurrentDestroyedCellIndex];
					iCurrentDestroyedCellIndex++;
				}
			}
		}

		iRow += iDeltaRow;
		iColumn += iDeltaColumn;
	}

	if (candidateDestroyedCells.size() > 0)
		return true;
	else
		return false;

}

bool NewGameBoardManager::DestroySingleCellUtil(const int& iRow, const int& iColumn, const float& fDestroyAtTime)
{	
	// this is bonus quest gem that's activated directly by combo
	if (m_BoardValueMatrix[iRow][iColumn].m_iGemID ==  _BONUS_QUEST_GEM_ID_)
	{
		m_BonusQuestManager.ActivateBonusQuest();
		m_DestroyBonusQuestGemList.push_back(DestroyedByComboCell(iRow, iColumn,-1,fDestroyAtTime));
		m_BoardValueMatrix[iRow][iColumn].Reset();
		return false;
	}



	if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType >= _GCT_COMBO4_ && m_BoardValueMatrix[iRow][iColumn].m_eGemComboType <= _GCT_COMBO6_)
	{
		// count combo gems
		m_BonusQuestManager.IncreaseComboCellCountForBonusQuest(m_BoardValueMatrix[iRow][iColumn].m_eGemComboType);
	}

	

	// combo 5 cell wil be activated/destroyed later
	if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType == _GCT_COMBO6_)
	{
		m_BoardValueMatrix[iRow][iColumn].m_eGemComboType =	_GCT_COMBO6_WAITING_TRIGGER_;						
		return false;
	}
	else if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType == _GCT_COMBO5_)
	{
		m_BoardValueMatrix[iRow][iColumn].m_eGemComboType =	_GCT_COMBO5_TRIGGER_SECOND_TIME_;
		return false;
	}
	else// if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_COMBO6_)
	{
		if (m_BoardValueMatrix[iRow][iColumn].m_iGemLetterBlockID >= 0 && 
			m_GemLetterManager.GetGemLetterData(m_BoardValueMatrix[iRow][iColumn].m_iGemLetterBlockID).m_bIsInMainWord)
		{						
			m_UnlockedGemLetterCellList.push_back(NewCellInfo(iRow, iColumn, m_BoardValueMatrix[iRow][iColumn].m_iGemLetterBlockID, true, m_BoardValueMatrix[iRow][iColumn].m_iGemID));
			m_BoardValueMatrix[iRow][iColumn].Reset();
			return false;
		}

		// note: destroy combo 5,6 wont activate boss or bonus quest game !!! ==> BUGS?
		if (m_pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount > 0)
		{
			if ( iRow > 0 && m_BoardValueMatrix[iRow-1][iColumn].m_iGemID == _BONUS_QUEST_GEM_ID_)
			{
				m_BonusQuestManager.ActivateBonusQuest();
				m_DestroyBonusQuestGemList.push_back(DestroyedByComboCell(iRow-1, iColumn,-1,fDestroyAtTime));
				m_BoardValueMatrix[iRow-1][iColumn].Reset();
			}
			if ( iRow < m_iRowNumber-1 && m_BoardValueMatrix[iRow+1][iColumn].m_iGemID == _BONUS_QUEST_GEM_ID_)
			{
				m_BonusQuestManager.ActivateBonusQuest();
				m_DestroyBonusQuestGemList.push_back(DestroyedByComboCell(iRow+1, iColumn, -1, fDestroyAtTime));
				m_BoardValueMatrix[iRow+1][iColumn].Reset();
			}
			if ( iColumn > 0 && m_BoardValueMatrix[iRow][iColumn-1].m_iGemID == _BONUS_QUEST_GEM_ID_)
			{
				m_BonusQuestManager.ActivateBonusQuest();
				m_DestroyBonusQuestGemList.push_back(DestroyedByComboCell(iRow, iColumn-1, -1, fDestroyAtTime));
				m_BoardValueMatrix[iRow][iColumn-1].Reset();
			}
			if ( iColumn < m_iColumnNumber-1 && m_BoardValueMatrix[iRow][iColumn+1].m_iGemID == _BONUS_QUEST_GEM_ID_)
			{
				m_BonusQuestManager.ActivateBonusQuest();
				m_DestroyBonusQuestGemList.push_back(DestroyedByComboCell(iRow, iColumn+1, -1, fDestroyAtTime));
				m_BoardValueMatrix[iRow][iColumn+1].Reset();
			}
		}

		// count normal gems
		m_BonusQuestManager.IncreaseBasicCellCountForBonusQuest(m_BoardValueMatrix[iRow][iColumn].m_iGemID);


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
					m_LevelBossInfo.m_fBossStateChangeAtTime = fDestroyAtTime;

					if (m_LevelBossInfo.m_iRemainLettersCount > 0)
						m_LevelBossInfo.m_iCurrentHitPoint = m_pLevelConfig->m_BossConfig.m_HitPointPerLetter;										
				}				
			}
		}

		m_BoardValueMatrix[iRow][iColumn].Reset(); //m_iGemID = -1;	m_BoardValueMatrix[iRow][iColumn].m_eGemComboType = _GCT_NONE_;
		return true;		
	}	
}

void NewGameBoardManager::CheckAndActivateBonusWordQuest(std::vector<NewCellInfo>& bonusWordCellList)
{
	//if (m_BonusQuestManager.IsQuestJustActivated(_BQT_COLLECT_BONUS_WORD_, true))
	{
		std::vector<Cell> normalCellList;

		// step 1: find all "normal" cell that can be converted to bonus-word-gem
		int iRow, iColumn;  
		for( iRow=0; iRow < m_iRowNumber; iRow++)
			for(iColumn=0; iColumn < m_iColumnNumber; iColumn++)
			{
				CellValue& cellValue = m_BoardValueMatrix[iRow][iColumn];
				if (!cellValue.m_bIsBlankCell && cellValue.m_iGemID < _MAX_GEM_ID_ && cellValue.m_eGemComboType == _GCT_NONE_ && cellValue.m_iObstacleBlockID < 0
					&& cellValue.m_iGemLetterBlockID < 0)
				{
					normalCellList.push_back(Cell(iRow, iColumn));
				}
			}

		// step 2: get letters from gameWordManager
		std::vector<char> letterList;
		m_BonusQuestManager.GetLettersOfCollectBonusWordQuest( letterList, normalCellList.size());

		// step 3: transform chosen cells
		int iCellListSize = normalCellList.size();
		int iLetterListSize = letterList.size();
		int iCellIndex, iGemLetterBlockID;
		unsigned char iLetter;
		for(int i=0; i< iLetterListSize; i++)
		{
			iCellIndex = rand() % iCellListSize;
			iLetter= (unsigned char)letterList[i];
			iGemLetterBlockID = m_GemLetterManager.AllocFreeBlock( iLetter, false);
			m_BoardValueMatrix[normalCellList[iCellIndex].m_iRow][normalCellList[iCellIndex].m_iColumn].m_iGemLetterBlockID = iGemLetterBlockID;
			bonusWordCellList.push_back( NewCellInfo( normalCellList[iCellIndex].m_iRow, normalCellList[iCellIndex].m_iColumn, iGemLetterBlockID, true,
				m_BoardValueMatrix[normalCellList[iCellIndex].m_iRow][normalCellList[iCellIndex].m_iColumn].m_iGemID));

			normalCellList[iCellIndex] = normalCellList[iCellListSize-1];
			iCellListSize--;
		}
	}
}

void NewGameBoardManager::ExecuteComboChain(std::vector<ComboEffectBundle*>& comboChainList)
{
	int iComboIndexInChain = 0;
	ComboEffectBundle* pComboInChain;
	while (iComboIndexInChain < comboChainList.size())
	{
		pComboInChain = comboChainList.at(iComboIndexInChain);
		Cell position = pComboInChain->m_ComboEffectDescription.m_Position;

		if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_ROW_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLUMN_
			|| pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_EXPLOSION_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_BONUS_END_GAME__SPECIAL_GEM_EFFECT_
			|| pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_BIRD_EXPLOSION_EFFECT_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_FROG_EXPLOSION_EFFECT_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_ELEPHANT_EXPLOSION_EFFECT_
			|| pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_SHEEP_RUN_LEFT_EFFECT_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_SHEEP_RUN_RIGHT_EFFECT_
			|| pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_CAT_RUN_UP_EFFECT_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_CAT_RUN_DOWN_EFFECT_)
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
			else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_EXPLOSION_ // EXPLOSION
				|| pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_BIRD_EXPLOSION_EFFECT_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_FROG_EXPLOSION_EFFECT_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_ELEPHANT_EXPLOSION_EFFECT_)
			{
				iRowDeltaD = iColumnDeltaD = -1;
				iRowDeltaA = iColumnDeltaA = 1;
				
				if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_EXPLOSION_)
				{
					// prepare for trigger second time!
					m_BoardValueMatrix[position.m_iRow][position.m_iColumn].m_eGemComboType = _GCT_COMBO5_5_TRIGGER_SECOND_TIME_;
					m_BoardValueMatrix[position.m_iRow][position.m_iColumn].m_iGemID = _SPECIAL_COMBO_GEM_ID;				

					// add second time trigger to waiting list
					ComboEffectDescription comboDescription2ndTime = pComboInChain->m_ComboEffectDescription;
					comboDescription2ndTime.m_eComboEffectType = _CET_EXPLOSION_SECOND_;
					comboDescription2ndTime.m_Phase0CellCount = 0;
					m_WaitingTriggerSecondTimeComboList.push_back(comboDescription2ndTime);
				}
			}
			else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_BONUS_END_GAME__SPECIAL_GEM_EFFECT_)
			{
				iColumnDeltaA = iColumnDeltaD = 0;
				iRowDeltaD = 0;
				iRowDeltaA = m_iRowNumber - position.m_iRow-1;
			}
			else if( pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_SHEEP_RUN_LEFT_EFFECT_)
			{
				iRowDeltaD = iRowDeltaA = 0;
				iColumnDeltaD = -position.m_iColumn;
				iColumnDeltaA = 0;
			}
			else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_SHEEP_RUN_RIGHT_EFFECT_)
			{
				iRowDeltaD = iRowDeltaA = 0;
				iColumnDeltaA = position.m_iColumn -position.m_iColumn - 1;
				iColumnDeltaD = 0;
			}
			else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_CAT_RUN_UP_EFFECT_)
			{
				iColumnDeltaD = iColumnDeltaA = 0;
				iRowDeltaD = 0;
				iRowDeltaA = m_iRowNumber - position.m_iRow-1;				
			}
			else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_CAT_RUN_DOWN_EFFECT_)
			{
				iColumnDeltaD = iColumnDeltaA = 0;
				iRowDeltaD = -position.m_iRow;
				iRowDeltaA = 0;
			}

			int iActivationComboRatio = GetComboDestroyCellRatio(pComboInChain->m_ComboEffectDescription.m_eComboEffectType);

			// trigger explostion effect: temporary implementation
			for(int iRow = position.m_iRow+iRowDeltaD; iRow <= position.m_iRow+iRowDeltaA; iRow++)
				for(int iColumn = position.m_iColumn+iColumnDeltaD; iColumn <= position.m_iColumn+iColumnDeltaA; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && IsCellDestroyable(iRow, iColumn))				
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pComboInChain, comboChainList, pComboInChain->m_fTriggerTime + _TME_DELAY_ACTIVE_COMBO_WHEN_BASIC_MATCHING_, pComboInChain->m_fTriggerTime + _TME_BASIC_COMBO_EXECUTE_TIME_);
					
					/*if (m_pObstacleProcessManager->DestroyCellWithObstacle(m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID))
					{				
						// this may trigger another combo ?
						if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType >=0 && m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_NONE_)						
						{
							int iRandomDirection = rand() % 2;

							ComboEffectBundle* pNextTriggeredCombo = new ComboEffectBundle();
							pNextTriggeredCombo->m_fTriggerTime = pComboInChain->m_fTriggerTime + _TME_BASIC_COMBO_EXECUTE_TIME_;
							pNextTriggeredCombo->m_ComboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_BoardValueMatrix[iRow][iColumn].m_eGemComboType, -1 * iRandomDirection, -1 * (1-iRandomDirection)); // _CET_EXPLOSION_;
							pNextTriggeredCombo->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
							pNextTriggeredCombo->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
							pNextTriggeredCombo->m_iNormalChainPhase =  pComboInChain->m_iNormalChainPhase + 1;

							//pComboInChain->m_TriggeredComboEfectBundleList.push_back(pNextTriggeredCombo);
								
							// add it to chain to trigger later
							comboChainList.push_back(pNextTriggeredCombo);
						}		

						// combo 5 cell wil be activated/destroyed later
						//int iObstacleBlockID = m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID;
						if (DestroySingleCellUtil(iRow, iColumn, pComboInChain->m_fTriggerTime))
						{							
							pComboInChain->m_DestroyedCells.push_back(DestroyedByComboCell(iRow, iColumn, 1, pComboInChain->m_fTriggerTime));
						}						
					}*/
				}
			}	

			// add combo cell to destroy list
			if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType != _CET_EXPLOSION_
				&& pComboInChain->m_ComboEffectDescription.m_eComboEffectType < _CET_SHEEP_RUN_LEFT_EFFECT_)
			{
				pComboInChain->m_DestroyedCells.push_back(DestroyedByComboCell( position.m_iRow, position.m_iColumn, -1, pComboInChain->m_fTriggerTime, false));
			}
		}
		else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_HAMSTER_RUN_LEFT_UP_EFFECT_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_HAMSTER_RUN_RIGHT_DOWN_EFFECT_)		
		{
			int iMinD, iMinA;			 
			if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_HAMSTER_RUN_LEFT_UP_EFFECT_)
			{
				iMinD = 0;
				iMinA = _MIN_(m_iRowNumber - position.m_iRow- 1, position.m_iColumn);				
			}
			else //_CET_HAMSTER_RUN_RIGHT_DOWN_EFFECT_
			{
				iMinD = _MIN_(position.m_iRow, m_iColumnNumber - position.m_iColumn -1);
				iMinA = 0;
			}
			
			int iActivationComboRatio = GetComboDestroyCellRatio(pComboInChain->m_ComboEffectDescription.m_eComboEffectType);

			for(int iDelta = -iMinD; iDelta <= iMinA; iDelta++)
				if ( !m_BoardValueMatrix[position.m_iRow + iDelta][position.m_iColumn - iDelta].m_bIsBlankCell && IsCellDestroyable(position.m_iRow + iDelta, position.m_iColumn - iDelta))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, position.m_iRow + iDelta, position.m_iColumn - iDelta, pComboInChain, comboChainList, pComboInChain->m_fTriggerTime + _TME_DELAY_ACTIVE_COMBO_WHEN_BASIC_MATCHING_, pComboInChain->m_fTriggerTime + _TME_BASIC_COMBO_EXECUTE_TIME_);
				}

			pComboInChain->m_DestroyedCells.push_back(DestroyedByComboCell( position.m_iRow, position.m_iColumn, -1, pComboInChain->m_fTriggerTime, false));
		}
		else if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_ROW_ || pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_COLUMN_)
		{
			pComboInChain->m_iNormalChainPhase = -1; //trigger later
			m_WaitingTriggerSecondTimeComboList.push_back(pComboInChain->m_ComboEffectDescription);
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

void NewGameBoardManager::TriggerWaitingCombo6(const ComboEffectDescription& comboDescription, ComboEffectBundle* &pTriggeredCombo, std::vector<ComboEffectBundle*>& comboChainList)
{				  
	Cell position = comboDescription.m_Position; //pComboInChain->m_ComboEffectDescription.m_Position;

	//if (pComboInChain->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_)					
	
	int iGemID1=-1, iGemID2=-1;
	Cell cell1(-1, -1), cell2(-2, -2);
	if (comboDescription.m_eComboEffectType == _CET_DESTROY_COLOR_COLUMN_)
	{
		if (comboDescription.m_Position.m_iColumn > 0 && m_BoardValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn-1].m_bIsBlankCell == false)
		{
			iGemID1 = m_BoardValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn-1].m_iGemID;				
			cell1 = Cell(comboDescription.m_Position.m_iRow, comboDescription.m_Position.m_iColumn-1);

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount] = cell1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 0;				
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;
		}

		if (comboDescription.m_Position.m_iColumn < m_iColumnNumber-1 && m_BoardValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn+1].m_bIsBlankCell == false)
		{
			iGemID2 = m_BoardValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn+1].m_iGemID;
			cell2 = Cell(comboDescription.m_Position.m_iRow, comboDescription.m_Position.m_iColumn+1);

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount] = cell2;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;
		}
	}
	else //row
	{
		if (comboDescription.m_Position.m_iRow > 0 && m_BoardValueMatrix[comboDescription.m_Position.m_iRow-1][comboDescription.m_Position.m_iColumn].m_bIsBlankCell == false)
		{
			iGemID1 = m_BoardValueMatrix[comboDescription.m_Position.m_iRow-1][comboDescription.m_Position.m_iColumn].m_iGemID;
			cell1 = Cell(comboDescription.m_Position.m_iRow-1, comboDescription.m_Position.m_iColumn);

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount] = cell1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 0;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;
		}

		if (comboDescription.m_Position.m_iRow < m_iRowNumber-1 && m_BoardValueMatrix[comboDescription.m_Position.m_iRow+1][comboDescription.m_Position.m_iColumn].m_bIsBlankCell == false)
		{
			iGemID2 = m_BoardValueMatrix[comboDescription.m_Position.m_iRow+1][comboDescription.m_Position.m_iColumn].m_iGemID;
			cell2 = Cell(comboDescription.m_Position.m_iRow+1, comboDescription.m_Position.m_iColumn);

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount] = cell2;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;
		}
	}
	
	//for test
	//iGemID1 = iGemID2 =-1;
	//int iGemID = comboDescription.m_iGemID;

	int iActivationComboRatio = GetComboDestroyCellRatio(comboDescription.m_eComboEffectType);
	bool bIsComboActivated = false; //if activate combo by destroy this cell then it wont earn score at this phase but at later (when activate its combo effect)

	for(int iRow = 0; iRow <= m_iRowNumber; iRow++)
		for(int iColumn = 0; iColumn <= m_iColumnNumber; iColumn++)
	{			
		if ( !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && ( m_BoardValueMatrix[iRow][iColumn].m_iGemID == iGemID1 || m_BoardValueMatrix[iRow][iColumn].m_iGemID == iGemID2)
			&& IsCellDestroyable(iRow, iColumn))
		{
			//&& m_BoardValueMatrix[iRow][iColumn].m_eGemComboType !=_GCT_COMBO5_WAITING_TRIGGER_)
			//if (m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID < 0 || m_pObstacleProcessManager->DestroyCell(m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID))
			bool bIsPreventedDestroyByObstacle = !m_pObstacleProcessManager->DestroyCellWithObstacle(m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID);
			if (!bIsPreventedDestroyByObstacle)
			{
				bIsComboActivated =  false;
				// this may trigger another combo ?
				if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType >=0 && m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_NONE_)
				{
					bIsComboActivated = true;

					int iRandomDirection = rand() % 2;

					ComboEffectBundle* pNextTriggeredCombo = new ComboEffectBundle();					 
					pNextTriggeredCombo->m_fTriggerTime = pTriggeredCombo->m_fTriggerTime + _TME_COMBO_6_PHASE1_EXECUTE_TIME_ + _TME_COMBO_6_PHASE2_REMOVE_CELL_AT_TIME_;
					
					pNextTriggeredCombo->m_ComboEffectDescription.m_iActivationScoreRatio = iActivationComboRatio;
					pNextTriggeredCombo->m_ComboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_BoardValueMatrix[iRow][iColumn].m_eGemComboType, -1 * iRandomDirection, -1 * (1-iRandomDirection)); // _CET_EXPLOSION_;						
					pNextTriggeredCombo->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
					pNextTriggeredCombo->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;

					//pTriggeredCombo->m_TriggeredComboEfectBundleList.push_back(pNextTriggeredCombo);
								
					// add it to chain to trigger later
					comboChainList.push_back(pNextTriggeredCombo);
				}		
			}
				
			int iBackupGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
			bool bIsCompleteDestroyed = !bIsPreventedDestroyByObstacle;
			if (!bIsPreventedDestroyByObstacle)
				bIsCompleteDestroyed = DestroySingleCellUtil(iRow, iColumn, pTriggeredCombo->m_fTriggerTime + _TME_COMBO_6_PHASE1_EXECUTE_TIME_ + _TME_COMBO_6_PHASE2_REMOVE_CELL_AT_TIME_);
			else
				bIsCompleteDestroyed = false;
			
			DestroyedByComboCell destroyedCell(iRow, iColumn);
			destroyedCell.m_bIsCompleteDestroyed = bIsCompleteDestroyed;
			destroyedCell.m_bIsEarnScore = (!bIsComboActivated)&&(!bIsPreventedDestroyByObstacle);
				
			if (iBackupGemID == iGemID1)
			{
				destroyedCell.m_iGroupIndex = 0;
				if (cell1 == destroyedCell)
				{
					destroyedCell.m_iDestroyPhaseIndex = 0;
					destroyedCell.m_fDestroyAtTime = pTriggeredCombo->m_fTriggerTime + _TME_COMBO_6_PHASE1_EXECUTE_TIME_;
				}
				else
				{
					destroyedCell.m_iDestroyPhaseIndex = 1;
					destroyedCell.m_fDestroyAtTime = pTriggeredCombo->m_fTriggerTime + _TME_COMBO_6_PHASE1_EXECUTE_TIME_ + _TME_COMBO_6_PHASE2_REMOVE_CELL_AT_TIME_;
				}
			}
			else
			{
				destroyedCell.m_iGroupIndex = 1;
				if (cell2 == destroyedCell)
				{
					destroyedCell.m_iDestroyPhaseIndex = 0;
					destroyedCell.m_fDestroyAtTime = pTriggeredCombo->m_fTriggerTime + _TME_COMBO_6_PHASE1_EXECUTE_TIME_;
				}
				else
				{
					destroyedCell.m_iDestroyPhaseIndex = 1;
					destroyedCell.m_fDestroyAtTime = pTriggeredCombo->m_fTriggerTime + _TME_COMBO_6_PHASE1_EXECUTE_TIME_ + _TME_COMBO_6_PHASE2_REMOVE_CELL_AT_TIME_;
				}
			}
			pTriggeredCombo->m_DestroyedCells.push_back(destroyedCell);					
		}
	}

	// reset combo cell
	m_BoardValueMatrix[position.m_iRow][position.m_iColumn].Reset();		
	pTriggeredCombo->m_DestroyedCells.push_back(DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
}


bool NewGameBoardManager::ExecuteEndGameBonus(
		std::vector<ComboEffectCell>& convertedToComboCells,
		std::vector<Cell>& basicMatchingDestroyedCells, //std::vector<DoubleComboCreationInfo>& doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList, //std::vector<ComboEffectBundle*>& triggeredCombo5ChainList,
		std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<NewCellInfo>& unlockedLetterCells,
		std::vector<NewCellInfo>& newCells)
{	
	m_iPhaseMoveInComboChain++;

	// reset
	m_iLinkedBlockCount = 0;
	m_UnlockedGemLetterCellList.clear();

	int iRow, iColumn;

	// NOTE: temporary disable double/triple at end game bonus
	// if this phase trigger only by waiting combo 6 then not need to create temporary list
	if ( m_WaitingTriggerSecondTimeComboList.size() ==0)
	{
		// create temporary list to compute result for shifting move
		CopyDataToTempBoardMatrixAndResetFlags( -1, -1, 0, 0);

		// process double combo and tripple combo		
		std::vector<ComboEffectDescription> advanceComboList;
		FindSortAndFilterAdvanceCombos(advanceComboList, -1, -1);	

		if (advanceComboList.size() > 0)
		{
			// compy back values of tempMatrix to boardMatrix
			memcpy( m_BoardValueMatrix, m_TemporaryValueMatrix, sizeof(m_TemporaryValueMatrix));		

			std::vector<ComboEffectBundle*> triggerAdvanceComboList;
			TriggerAdvanceComboList( advanceComboList, comboChainList, triggerAdvanceComboList, false);
			comboChainList.insert(comboChainList.begin(), triggerAdvanceComboList.begin(), triggerAdvanceComboList.end());		
			
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
	}	

	// activate marked combo 6 and other trigger-2-times combos from last check
	if (m_WaitingTriggerSecondTimeComboList.size()>0)
	{
		//TriggerWaitingCombo6List(comboChainList, triggeredCombo6ChainList);

		std::vector<ComboEffectBundle*> triggerAdvanceComboList;
		TriggerAdvanceComboList( m_WaitingTriggerSecondTimeComboList, comboChainList, triggerAdvanceComboList, true);
		comboChainList.insert(comboChainList.begin(), triggerAdvanceComboList.begin(), triggerAdvanceComboList.end());				

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


	int iSameValueCellCount;
	// bool bIsValidMove = false;
	bool bHasExistingCombo = false;

	// create temporary list to compute result for shifting move
	//CopyDataToTempBoardMatrixAndResetFlags( -1, -1, 0, 0);

	// ************* create block for basic matching ******************************************
	CreateBlockForBasicMatching( basicMatchingDestroyedCells);

	if (basicMatchingDestroyedCells.size() == 0)
	{
		m_iPhaseMoveInComboChain = 1; //reset phase index


		// convert some cells to special combo based on remain move count
		std::vector<Cell> notComboCells;
		std::vector<Cell> randomCellEachColumnList;
		for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
		{
			randomCellEachColumnList.clear();
			for(iRow = 0; iRow < m_iRowNumber; iRow++)
			{
				if (!m_TemporaryValueMatrix[iRow][iColumn].m_bIsBlankCell && m_TemporaryValueMatrix[iRow][iColumn].m_iGemLetterBlockID < 0 && m_TemporaryValueMatrix[iRow][iColumn].m_iObstacleBlockID < 0
					&& m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType == _GCT_NONE_ && m_TemporaryValueMatrix[iRow][iColumn].m_iGemID < _MAX_GEM_ID_)
					randomCellEachColumnList.push_back(Cell(iRow, iColumn));							
			}
			if ( randomCellEachColumnList.size() == 0)
				continue;

			int iRandomIndex = rand() % randomCellEachColumnList.size();
			notComboCells.push_back( randomCellEachColumnList[iRandomIndex]);		
		}

		int iRandomIndex;
		int iMaxConvertedCell = MIN( MIN(m_iCurrentMove, 5), notComboCells.size());
		m_iCurrentMove -= iMaxConvertedCell;

		for(int i=0; i< iMaxConvertedCell; i++)
		{
			if (notComboCells.size() == 0)
				break;

			iRandomIndex = rand() % notComboCells.size();
			if (m_pLevelConfig->m_eEndGameBonusType == _EGBT_SPECIAL_GEMS_)
			{
				m_TemporaryValueMatrix[notComboCells[iRandomIndex].m_iRow][notComboCells[iRandomIndex].m_iColumn].m_eGemComboType = _GCT_BONUS_END_GAME_SPECIAL_GEM_;
				m_TemporaryValueMatrix[notComboCells[iRandomIndex].m_iRow][notComboCells[iRandomIndex].m_iColumn].m_iGemID = _BONUS_END_GAME_COMBO_GEM_ID;		
			}
			else //_EGBT_CRAZY_PETS_
				m_TemporaryValueMatrix[notComboCells[iRandomIndex].m_iRow][notComboCells[iRandomIndex].m_iColumn].m_eGemComboType = _GCT_BONUS_END_GAME_CRAZY_PET_;			
		
			ComboEffectCell comboCell(notComboCells[iRandomIndex], m_TemporaryValueMatrix[notComboCells[iRandomIndex].m_iRow][notComboCells[iRandomIndex].m_iColumn].m_eGemComboType,
				m_TemporaryValueMatrix[notComboCells[iRandomIndex].m_iRow][notComboCells[iRandomIndex].m_iColumn].m_iGemID);
			convertedToComboCells.push_back(comboCell);
		
			notComboCells[iRandomIndex] = notComboCells[notComboCells.size()-1];
			notComboCells.pop_back();
		}

		// ************* check if there's a combo at the end of bonus ****************************************************	
		if (convertedToComboCells.size() == 0) //no more bonus move
		{
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
		}	
	}

	// ********************* 
	if (m_iLinkedBlockCount > 0 ||  convertedToComboCells.size()>0 || bHasExistingCombo)
	{
		// for test
		//CountBasicCombo();

		// Update main board matrix from temporary matrix if it's valid move
		//if ( iSelectedRow >=0 || iSelectedColumn >= 0)		
		/*{
			int iRow, iColumn;

			for(iRow = 0; iRow < m_iRowNumber; iRow++)
				for(iColumn = 0; iColumn < m_iColumnNumber; iColumn++)
					m_BoardValueMatrix[iRow][iColumn] = m_TemporaryValueMatrix[iRow][iColumn];
		}*/
		memcpy( m_BoardValueMatrix, m_TemporaryValueMatrix, sizeof(m_TemporaryValueMatrix));
		
		// add effect of double combo to first of chain
		//comboChainList.insert(comboChainList.begin(), chainEffectFromDoubleCombo.begin(), chainEffectFromDoubleCombo.end());

		// remove cells by basic matching and create first combo chain
		RemoveCellsByBasicMatching( basicMatchingDestroyedCells, comboChainList, 0, 0);
		

		// add converted combo from bonus and existing combo to list
		int iConvertedComboIndex = 0;
		for(auto cell: convertedToComboCells)
		{
			if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType == _GCT_BONUS_END_GAME_SPECIAL_GEM_
				|| m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType == _GCT_BONUS_END_GAME_CRAZY_PET_) //!= _GCT_NONE_)
			{				

				ComboEffectBundle* pTriggeredComboEffectBundle = new ComboEffectBundle();
				pTriggeredComboEffectBundle->m_fTriggerTime = _TME_BASIC_COMBO_EXECUTE_TIME_ + iConvertedComboIndex * _TME_BASIC_COMBO_EXECUTE_TIME_ * 0.3f;
				pTriggeredComboEffectBundle->m_ComboEffectDescription.m_eComboEffectType = GetEndGameBonusComboEffectTypeFrom( m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType, m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID);
					//_CET_BONUS_END_GAME__SPECIAL_GEM_EFFECT_; //(rand() %2==0)?_CET_DESTROY_ROW_:_CET_DESTROY_COLUMN_; //only implement explosion now
				pTriggeredComboEffectBundle->m_ComboEffectDescription.m_Position = cell;
				pTriggeredComboEffectBundle->m_ComboEffectDescription.m_iGemID = m_TemporaryValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID;
				pTriggeredComboEffectBundle->m_iNormalChainPhase = comboChainList.size(); //0;														

				comboChainList.push_back(pTriggeredComboEffectBundle);

				iConvertedComboIndex++;
				m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].Reset();
			}
		}

		// if all remain move are used then activate existing combo gem on board
		if (convertedToComboCells.size() == 0  &&  bHasExistingCombo)
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

void NewGameBoardManager::CreateBlockForBasicMatching(std::vector<Cell>& basicMatchingDestroyedCells)
{
	for(int i=0; i< _MAX_LINKED_BLOCK_COUNT_; i++)
	{
		m_LinkedBlockList[i].m_iCellCount = 0;
	}	

	int iRow, iColumn, iGemID, iSameValueCellCount;
	Cell currentCheckCell;
	m_iLinkedBlockCount = 0;
	bool bIsNewBlockCreated;

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
						if (m_iLinkedBlockCount > 0) //update state for lastest block
						{
							LinkedBlockDescription& block = m_LinkedBlockList[m_iLinkedBlockCount-1];
							if ( (basicMatchingDestroyedCells[block.m_iStartIndexInDestroyedList].m_iRow == basicMatchingDestroyedCells[block.m_iStartIndexInDestroyedList+1].m_iRow
								&& basicMatchingDestroyedCells[block.m_iStartIndexInDestroyedList].m_iRow == basicMatchingDestroyedCells[block.m_iStartIndexInDestroyedList + block.m_iCellCount-1].m_iRow)
								|| (basicMatchingDestroyedCells[block.m_iStartIndexInDestroyedList].m_iColumn == basicMatchingDestroyedCells[block.m_iStartIndexInDestroyedList+1].m_iColumn
								&& basicMatchingDestroyedCells[block.m_iStartIndexInDestroyedList].m_iColumn == basicMatchingDestroyedCells[block.m_iStartIndexInDestroyedList + block.m_iCellCount-1].m_iColumn))
							{
								block.m_bAllCellOnTheSameLine = true;
							}
							else
								block.m_bAllCellOnTheSameLine = false;
						}

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
	bool bIsComboActivated = false;

	for(auto& cell : basicMatchingDestroyedCells)
	{
		if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID >= 0)
			//if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iObstacleBlockID < 0 || m_pObstacleProcessManager->DestroyCell(m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iObstacleBlockID))
			if (m_pObstacleProcessManager->DestroyCellWithObstacle(m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iObstacleBlockID))
			{				
				bIsComboActivated = false;

				// NOTE: destroyed cell include double combo cells!!! ????
				// 1 cell affected by 2 effects (basic matching and double combo) ==> how to display those effect???		
				if (m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType >= _GCT_COMBO4_ && m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType <= _GCT_COMBO6_)
				{							
					bIsComboActivated = true;

					ComboEffectBundle* pTriggeredComboEffectBundle = new ComboEffectBundle();
					pTriggeredComboEffectBundle->m_fTriggerTime = comboChainList.size() * _TME_BASIC_COMBO_EXECUTE_TIME_; //_TME_DELAY_ACTIVE_COMBO_WHEN_BASIC_MATCHING_;
					pTriggeredComboEffectBundle->m_ComboEffectDescription.m_eComboEffectType = GetComboEffectTypeFromComboType(m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_eGemComboType, iMoveRow, iMoveColumn);	
					pTriggeredComboEffectBundle->m_ComboEffectDescription.m_Position = cell;
					pTriggeredComboEffectBundle->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[cell.m_iRow][cell.m_iColumn].m_iGemID;
					pTriggeredComboEffectBundle->m_iNormalChainPhase = comboChainList.size(); //0;														

					comboChainList.push_back(pTriggeredComboEffectBundle);
				}			

				// combo 5 will be activated/destroyed later				
				if (!DestroySingleCellUtil(cell.m_iRow, cell.m_iColumn, 0) || bIsComboActivated)
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
					if (linkedBlock.m_bAllCellOnTheSameLine)
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
	for(auto& waitingTriggerCombo : m_WaitingTriggerSecondTimeComboList)
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

	// check and generate bonus quest gem
	int iNewBonusQuestGemCount = 0;
	if (bIsNewMove)
	{
		if (m_pLevelConfig->m_BonusQuestConfig.m_BonusGemAppearAtMoves)
		{
			int iMoveElapse = m_pLevelConfig->m_iNumberOfMove - m_iCurrentMove+1;
			
			for(int i=0; i < m_pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount; i++)
				if (iMoveElapse == m_pLevelConfig->m_BonusQuestConfig.m_BonusGemAppearAtMoves[i])
				{
					iNewBonusQuestGemCount++;
					m_BoardValueMatrix[newCells[newCells.size()-iNewBonusQuestGemCount].m_iRow][newCells[newCells.size()-iNewBonusQuestGemCount].m_iColumn].m_iGemID = _BONUS_QUEST_GEM_ID_;
					newCells[newCells.size()-iNewBonusQuestGemCount].m_iGemID = _BONUS_QUEST_GEM_ID_;
				}
		}
	}

	std::vector<GemLetterData> outputLettersForGems;
	std::vector<bool> gemCanContainLetterFlagList;	
	int iGemCanContainLetterCount = 0;
	for(auto cell: newCells)
	{
		if (cell.m_iGemID < _MAX_GEM_ID_)
		{
			if (m_pLevelConfig->m_bMainWordCanDropOnAllColumn || m_pLevelConfig->m_MainWordDropOnColumnsFlagList[cell.m_iColumn])
			{
				gemCanContainLetterFlagList.push_back(true);
				iGemCanContainLetterCount++;
			}
			else
				gemCanContainLetterFlagList.push_back(false);
		}
	}

	m_pGameWordManager->GenerateNewLetters( gemCanContainLetterFlagList, iGemCanContainLetterCount, outputLettersForGems, bIsNewMove);	

	unsigned char iLetter = 255;	
	for(int iIndex = 0; iIndex < newCells.size()-iNewBonusQuestGemCount; iIndex++)	
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

			// generate obstacle that can be dropped!!!
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
/*int NewGameBoardManager::GetScoreForUnlockLetterInMainWord()
{
	return m_GameConfig.m_iScoreOfMainWord;
}

int NewGameBoardManager::GetScoreForUnlockLetterInSubWord()
{
	return m_GameConfig.m_iScoreOfSubWord;
}
*/
int NewGameBoardManager::IncreaseScoreForLetterInMainWord()
{
	m_iCurrentScore += m_GameConfig.m_iScoreLetterOfMainWord;

	CCLOG("Increase score unlock letter of main word combo, score incr: %d", m_GameConfig.m_iScoreLetterOfMainWord);

	return m_GameConfig.m_iScoreLetterOfMainWord;
}

int NewGameBoardManager::IncreaseScoreForLetterInBonusWords(const int& iLetterCount)
{
	int iTotalScoreIncrement = m_GameConfig.m_iScoreLetterOfBonusWord * iLetterCount;
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
	int iRealLetterCount = m_pGameWordManager->GetTotalCollectibleLettersOfMainWord();		
	int iBonusScore = iRealLetterCount *  m_GameConfig.m_iScoreRatioCompleteMainWord;	

	return iBonusScore;
}

int NewGameBoardManager::IncreaseScoreForCompleteBonusQuest()
{
	int iBonusScore = GetBonusScoreForCompleteBonusQuest();
	m_iCurrentScore += iBonusScore;

	return iBonusScore;
}

int NewGameBoardManager::GetBonusScoreForCompleteBonusQuest()
{
	int iBonusScore = m_GameConfig.m_iBonusScoreCompleteBonusQuest; 
	return iBonusScore;
}


int NewGameBoardManager::IncreaseScoreForCreateCombo(const GemComboType_e& eComboType)
{	
	int iCreateBonusScore = 0;
	switch (eComboType)
	{
		// single
		case GemComboType_e::_GCT_COMBO4_:
		{			
			iCreateBonusScore = m_GameConfig.m_iBonusScoreCreateCombo4;
			break;
		}
		case GemComboType_e::_GCT_COMBO5_:
		{			
			iCreateBonusScore = m_GameConfig.m_iBonusScoreCreateCombo5;
			break;
		}
		case GemComboType_e::_GCT_COMBO6_:
		{			
			iCreateBonusScore = m_GameConfig.m_iBonusScoreCreateCombo6;
			break;
		}		
	}
	int iIncrementScore = iCreateBonusScore * m_iPhaseMoveInComboChain;
	m_iCurrentScore += iIncrementScore;

	CCLOG("Increase score create combo: %d, score incr: %d", (int)eComboType, iIncrementScore);

	return iIncrementScore;
}

int NewGameBoardManager::GetComboDestroyCellRatio(const ComboEffectType& eComboEffectType)
{
	int iRatio = 100;
	switch (eComboEffectType)
	{
		// single
		case ComboEffectType::_CET_DESTROY_ROW_ :
		case ComboEffectType::_CET_DESTROY_COLUMN_:
		{
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio4;		
			break;
		}
		case ComboEffectType::_CET_EXPLOSION_: 
		case ComboEffectType::_CET_EXPLOSION_SECOND_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio5;			
			break;
		case ComboEffectType::_CET_DESTROY_COLOR_ROW_:
		case ComboEffectType::_CET_DESTROY_COLOR_COLUMN_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio6;			
			break;

		// double
		case ComboEffectType:: _CET_4_4_EFFECT_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio4_4;			
			break;
		case ComboEffectType::_CET_4_5_EFFECT_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio4_5;			
			break;
		case ComboEffectType::_CET_5_5_EFFECT_:			
		case ComboEffectType::_CET_5_5_SECOND_EFFECT_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio5_5;
			break;
		case ComboEffectType::_CET_6_4_EFFECT_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio6_4;			
			break;
		case ComboEffectType::_CET_6_5_EFFECT_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio6_5;			
			break;
		case ComboEffectType::_CET_6_6_EFFECT_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio6_6;
			break;

		// triple
		case ComboEffectType:: _CET_4_4_4_EFFECT_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio4_4_4;			
			break;
		case ComboEffectType::_CET_5_5_5_EFFECT_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio5_5_5;			
			break;
		case ComboEffectType::_CET_6_6_6_EFFECT_:
		case ComboEffectType::_CET_6_6_6_SECOND_EFFECT_:
			iRatio = m_GameConfig.m_iCombEffectDestroyCellRatio6_6_6;			
			break;
		case ComboEffectType::_CET_BONUS_END_GAME__SPECIAL_GEM_EFFECT_:
		case ComboEffectType::_CET_BIRD_EXPLOSION_EFFECT_:
		case ComboEffectType::_CET_FROG_EXPLOSION_EFFECT_:
		case ComboEffectType::_CET_ELEPHANT_EXPLOSION_EFFECT_:
		case ComboEffectType::_CET_SHEEP_RUN_LEFT_EFFECT_:
		case ComboEffectType::_CET_SHEEP_RUN_RIGHT_EFFECT_:
		case ComboEffectType::_CET_CAT_RUN_UP_EFFECT_:
		case ComboEffectType::_CET_CAT_RUN_DOWN_EFFECT_:
		case ComboEffectType::_CET_HAMSTER_RUN_LEFT_UP_EFFECT_:
		case ComboEffectType::_CET_HAMSTER_RUN_RIGHT_DOWN_EFFECT_:
			iRatio = m_GameConfig.m_iEndGameComboEffectDestroyCellRatio;
			break;
	}

	return iRatio;
}

int NewGameBoardManager::GetScorePerGemDestroyByCombo(const ComboEffectType& eComboEffectType)
{
	int iRatio = GetComboDestroyCellRatio(eComboEffectType);
	return m_GameConfig.m_iScoreOfGem * iRatio/100 * m_iPhaseMoveInComboChain;
}

int NewGameBoardManager::IncreaseScoreForDestroyCells(const int& iGemCount, const ComboEffectType& eComboEffectType, int iActivateComboRatio)
{	
	int iRatio = GetComboDestroyCellRatio(eComboEffectType);
	int iActivationBonusScore = 0;
	switch (eComboEffectType)
	{
		// single
		case ComboEffectType::_CET_DESTROY_ROW_ :
		case ComboEffectType::_CET_DESTROY_COLUMN_:
		{			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo4;
			break;
		}
		//case ComboEffectType::_CET_EXPLOSION_:   			
		case ComboEffectType::_CET_EXPLOSION_SECOND_:
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo5;
			break;
		case ComboEffectType::_CET_DESTROY_COLOR_ROW_:
		case ComboEffectType::_CET_DESTROY_COLOR_COLUMN_:			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo6;
			break;

		// double
		case ComboEffectType:: _CET_4_4_EFFECT_:			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo4_4;
			break;
		case ComboEffectType::_CET_4_5_EFFECT_:			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo4_5;
			break;
		//case ComboEffectType::_CET_5_5_EFFECT_:			
		case ComboEffectType::_CET_5_5_SECOND_EFFECT_:
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo5_5;
			break;
		case ComboEffectType::_CET_6_4_EFFECT_:			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo6_4;
			break;
		case ComboEffectType::_CET_6_5_EFFECT_:			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo6_5;
			break;
		case ComboEffectType::_CET_6_6_EFFECT_:			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo6_6;
			break;

		// triple
		case ComboEffectType:: _CET_4_4_4_EFFECT_:			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo4_4_4;
			break;
		case ComboEffectType::_CET_5_5_5_EFFECT_:			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo5_5_5;
			break;
		//case ComboEffectType::_CET_6_6_6_EFFECT_:
		case ComboEffectType::_CET_6_6_6_SECOND_EFFECT_:			
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateCombo6_6_6;
			break;
		case ComboEffectType::_CET_BONUS_END_GAME__SPECIAL_GEM_EFFECT_:
			iActivationBonusScore = m_GameConfig.m_iBonusScoreActivateBonusEndGameCombo;
			break;
	}
	int iIncrementScore = (m_GameConfig.m_iScoreOfGem * iRatio * iGemCount/100 + iActivationBonusScore * iActivateComboRatio/100) * m_iPhaseMoveInComboChain;	
	m_iCurrentScore += iIncrementScore;

	//CCLOG("Increase score gem count: %d, combo effect: %d, score incr: %d", iGemCount, (int)eComboEffectType, iIncrementScore);

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

ComboEffectType NewGameBoardManager::GetComboEffectTypeFromComboType(const GemComboType_e& eGemComboType, const int& iMoveRow, const int& iMoveColumn)
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

ComboEffectType NewGameBoardManager::GetEndGameBonusComboEffectTypeFrom(const GemComboType_e& eGemComboType, const int& iGemID)
{
	if (eGemComboType == _GCT_BONUS_END_GAME_SPECIAL_GEM_)
		return _CET_BONUS_END_GAME__SPECIAL_GEM_EFFECT_;
	else // _EBGT_CRAZY_GEMS_
	{
		switch ((GemType_e)iGemID)
		{
			default:
			case _GT_BIRD_:
				return _CET_BIRD_EXPLOSION_EFFECT_;
			case _GT_FROG_:
				return _CET_FROG_EXPLOSION_EFFECT_;
			case _GT_ELEPHANT_:
				return _CET_ELEPHANT_EXPLOSION_EFFECT_;
			case _GT_SHEEP_:
				return (rand() %2 ==0)?_CET_SHEEP_RUN_LEFT_EFFECT_:_CET_SHEEP_RUN_RIGHT_EFFECT_;
			case _GT_CAT_:
				return (rand() %2 ==0)?_CET_CAT_RUN_UP_EFFECT_:_CET_CAT_RUN_DOWN_EFFECT_;
			case _GT_HAMSTER_:
				return (rand() %2 ==0)?_CET_HAMSTER_RUN_LEFT_UP_EFFECT_:_CET_HAMSTER_RUN_RIGHT_DOWN_EFFECT_;
		}
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

void NewGameBoardManager::FindSortAndFilterAdvanceCombos(std::vector<ComboEffectDescription>& advanceComboList, const int& iMoveRow, const int& iMoveColumn)
{
	std::vector<ComboEffectDescription> tempAdvanceComboList;

	// step: find all tripple/double combos => sort combos ==> clean combos ==> then execute combo at main method
	ComboEffectCell linkCellList[3];	
	int iLinkCellCount, iRow, iColumn;
	int i,j;
	GemComboType_e eMaxComboType;	
	ComboEffectDescription tempComboDescription;

	for(iRow=0; iRow < m_iRowNumber; iRow++)	
		for(iColumn=0; iColumn < m_iColumnNumber; iColumn++)
			if (IsSimpleCombo(m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType))
			{					
				linkCellList[0].m_iRow = iRow;
				linkCellList[0].m_iColumn = iColumn;
				linkCellList[0].m_iGemID = m_TemporaryValueMatrix[iRow][iColumn].m_iGemID;
				linkCellList[0].m_eGemComboType = m_TemporaryValueMatrix[iRow][iColumn].m_eGemComboType;
				iLinkCellCount = 1;

				// check horizontal
				if (iColumn < m_iColumnNumber-1 && IsSimpleCombo(m_TemporaryValueMatrix[iRow][iColumn+1].m_eGemComboType))
				{
					linkCellList[iLinkCellCount].m_iRow = iRow;
					linkCellList[iLinkCellCount].m_iColumn = iColumn+1;
					linkCellList[iLinkCellCount].m_iGemID = m_TemporaryValueMatrix[iRow][iColumn+1].m_iGemID;
					linkCellList[iLinkCellCount].m_eGemComboType = m_TemporaryValueMatrix[iRow][iColumn+1].m_eGemComboType;
					iLinkCellCount++;

					if (iColumn < m_iColumnNumber-2 && IsSimpleCombo(m_TemporaryValueMatrix[iRow][iColumn+2].m_eGemComboType))
					{
						linkCellList[iLinkCellCount].m_iRow = iRow;
						linkCellList[iLinkCellCount].m_iColumn = iColumn+2;
						linkCellList[iLinkCellCount].m_iGemID = m_TemporaryValueMatrix[iRow][iColumn+2].m_iGemID;
						linkCellList[iLinkCellCount].m_eGemComboType = m_TemporaryValueMatrix[iRow][iColumn+2].m_eGemComboType;
						iLinkCellCount++;
					}
				}

				if (iLinkCellCount >= 2)
				{					
					tempComboDescription.m_eComboEffectType = GetComboEffectTypeOfAdvanceCombo(linkCellList, iLinkCellCount, iMoveRow, iMoveColumn);
					tempComboDescription.m_Position = (Cell)linkCellList[0];
					tempComboDescription.m_iGemID = linkCellList[0].m_iGemID;
					tempComboDescription.m_Phase0CellCount = iLinkCellCount-1;
					for(i=0; i < iLinkCellCount-1;i++)
					{
						tempComboDescription.m_Phase0CellList[i] = (Cell)linkCellList[i+1];
						tempComboDescription.m_Phase0CellList[i].m_iGroupIndex = i;
					}
					tempAdvanceComboList.push_back(tempComboDescription);
				}

				// reset				
				iLinkCellCount = 1;

				// check vertical
				if (iRow < m_iRowNumber-1 && IsSimpleCombo(m_TemporaryValueMatrix[iRow+1][iColumn].m_eGemComboType))
				{
					linkCellList[iLinkCellCount].m_iRow = iRow+1;
					linkCellList[iLinkCellCount].m_iColumn = iColumn;
					linkCellList[iLinkCellCount].m_iGemID = m_TemporaryValueMatrix[iRow+1][iColumn].m_iGemID;
					linkCellList[iLinkCellCount].m_eGemComboType = m_TemporaryValueMatrix[iRow+1][iColumn].m_eGemComboType;
					iLinkCellCount++;

					if (iRow < m_iRowNumber-2 && IsSimpleCombo(m_TemporaryValueMatrix[iRow+2][iColumn].m_eGemComboType))
					{
						linkCellList[iLinkCellCount].m_iRow = iRow + 2;
						linkCellList[iLinkCellCount].m_iColumn = iColumn;
						linkCellList[iLinkCellCount].m_iGemID = m_TemporaryValueMatrix[iRow+2][iColumn].m_iGemID;
						linkCellList[iLinkCellCount].m_eGemComboType = m_TemporaryValueMatrix[iRow+2][iColumn].m_eGemComboType;
						iLinkCellCount++;
					}
				}

				if (iLinkCellCount >= 2)
				{					
					tempComboDescription.m_eComboEffectType = GetComboEffectTypeOfAdvanceCombo(linkCellList, iLinkCellCount, iMoveRow, iMoveColumn);
					tempComboDescription.m_Position = (Cell)linkCellList[0];
					tempComboDescription.m_iGemID = linkCellList[0].m_iGemID;
					tempComboDescription.m_Phase0CellCount = iLinkCellCount-1;

					for(i=0; i < iLinkCellCount-1;i++)
					{
						tempComboDescription.m_Phase0CellList[i] = (Cell)linkCellList[i+1];
						tempComboDescription.m_Phase0CellList[i].m_iGroupIndex = i;
					}
					tempAdvanceComboList.push_back(tempComboDescription);
				}
			}

	// step 2: sort combo based on their priorites
	int iListSize = tempAdvanceComboList.size();
	
	for(i=0; i< iListSize-1; i++)
		for(j=i+1; j< iListSize; j++)
			if (tempAdvanceComboList[i].m_eComboEffectType < tempAdvanceComboList[j].m_eComboEffectType)
			{
				tempComboDescription = tempAdvanceComboList[i];
				tempAdvanceComboList[i] = tempAdvanceComboList[j];
				tempAdvanceComboList[j] = tempComboDescription;
			}

	// step 3: clean/filter combo list
	if (iListSize > 0)
	{
		// if there's combo 6-6-6 or 6-6 at the first of list then disable all other advance combos!!!!
		if (tempAdvanceComboList[0].m_eComboEffectType == _CET_6_6_6_EFFECT_ || tempAdvanceComboList[0].m_eComboEffectType == _CET_6_6_EFFECT_)
		{
			tempComboDescription = tempAdvanceComboList[0];
			tempAdvanceComboList.clear();
			tempAdvanceComboList.push_back(tempComboDescription);
		}

		for(auto& comboDescription : tempAdvanceComboList)
		{
			if (m_TemporaryValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn].m_eGemComboType != _GCT_NONE_
				&& m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[0].m_iRow][comboDescription.m_Phase0CellList[0].m_iColumn].m_eGemComboType != _GCT_NONE_
				&& (comboDescription.m_Phase0CellCount == 1 || m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[1].m_iRow][comboDescription.m_Phase0CellList[1].m_iColumn].m_eGemComboType != _GCT_NONE_))
			{
				// update combo count of bonus quest
				m_BonusQuestManager.IncreaseComboCellCountForBonusQuest( m_TemporaryValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn].m_eGemComboType);
				m_BonusQuestManager.IncreaseComboCellCountForBonusQuest( m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[0].m_iRow][comboDescription.m_Phase0CellList[0].m_iColumn].m_eGemComboType);
				if (comboDescription.m_Phase0CellCount > 1)
				{
					m_BonusQuestManager.IncreaseComboCellCountForBonusQuest( m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[1].m_iRow][comboDescription.m_Phase0CellList[1].m_iColumn].m_eGemComboType);					
				}

				// clear cells
				m_TemporaryValueMatrix[comboDescription.m_Position.m_iRow][comboDescription.m_Position.m_iColumn].Reset();
				m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[0].m_iRow][comboDescription.m_Phase0CellList[0].m_iColumn].Reset();
				if (comboDescription.m_Phase0CellCount > 1)
				{
					m_TemporaryValueMatrix[comboDescription.m_Phase0CellList[1].m_iRow][comboDescription.m_Phase0CellList[1].m_iColumn].Reset();
				}

				advanceComboList.push_back(comboDescription);
			}		
		}

	}
}

ComboEffectType NewGameBoardManager::GetComboEffectTypeOfAdvanceCombo(ComboEffectCell (&linkCellList)[3], const int& iLinkCellCount, const int& iSelectedRow, const int& iSelectedColumn)
{
	// sort first	
	ComboEffectCell tempCell;
	if (iLinkCellCount == 3)
	{
		int iSwapIndex = 1;
		tempCell = linkCellList[0];
		linkCellList[0] = linkCellList[iSwapIndex];
		linkCellList[iSwapIndex] = tempCell;
	}
	else
	{
		if (linkCellList[1].m_iRow == iSelectedRow || linkCellList[1].m_iColumn == iSelectedColumn)  //linkCellList[1].m_eGemComboType > linkCellList[0].m_eGemComboType)
		{
			tempCell = linkCellList[0];
			linkCellList[0] = linkCellList[1];
			linkCellList[1] = tempCell;
		}
		/*if (linkCellList[2].m_iRow == iSelectedRow || linkCellList[2].m_iColumn == iSelectedColumn)  //if (iLinkCellCount > 2 && linkCellList[2].m_eGemComboType > linkCellList[0].m_eGemComboType)
		{
			tempCell = linkCellList[0];
			linkCellList[0] = linkCellList[2];
			linkCellList[2] = tempCell;
		}*/
	}	
		
	// user other sorted list to get combo type
	ComboEffectCell sortCellList[3];
	memcpy( sortCellList, linkCellList, sizeof(linkCellList));
	if (sortCellList[1].m_eGemComboType > sortCellList[0].m_eGemComboType)
	{
		tempCell = sortCellList[0];
		sortCellList[0] = sortCellList[1];
		sortCellList[1] = tempCell;
	}
	if (iLinkCellCount > 2 && sortCellList[2].m_eGemComboType > sortCellList[0].m_eGemComboType)
	{
		tempCell = sortCellList[0];
		sortCellList[0] = sortCellList[2];
		sortCellList[2] = tempCell;
	}     

	// test:
	//linkCellList[0].m_iGemID = sortCellList[1].m_iGemID;
	//return _CET_4_4_4_EFFECT_;


	// note: we use gemID of cell0 to use for combo if this combo need color
	linkCellList[0].m_iGemID = _SPECIAL_COMBO_GEM_ID;

	// get combo type
	if (iLinkCellCount == 2) // double combo
	{
		if ( sortCellList[0].m_eGemComboType == _GCT_COMBO6_)
		{
			if (sortCellList[1].m_eGemComboType == _GCT_COMBO6_)
				return _CET_6_6_EFFECT_;
			else if (sortCellList[1].m_eGemComboType == _GCT_COMBO5_)
			{
				linkCellList[0].m_iGemID = sortCellList[1].m_iGemID;
				return _CET_6_5_EFFECT_;
			}
			else // combo 6-4
			{
				linkCellList[0].m_iGemID = sortCellList[1].m_iGemID;
				return _CET_6_4_EFFECT_;
			}
		}
		else if ( sortCellList[0].m_eGemComboType == _GCT_COMBO5_)
		{
			if (sortCellList[1].m_eGemComboType == _GCT_COMBO5_)
				return _CET_5_5_EFFECT_;			
			else // combo 4-5							
				return _CET_4_5_EFFECT_;			
		}
		else // 4-5
			return _CET_4_4_EFFECT_;
	}
	else //tripple
	{
		if ( sortCellList[0].m_eGemComboType == _GCT_COMBO6_)
		{
			return _CET_6_6_6_EFFECT_;
		}
		else if (sortCellList[0].m_eGemComboType == _GCT_COMBO5_)
		{
			return _CET_5_5_5_EFFECT_;
		}
		else //4-4-4
			return _CET_4_4_4_EFFECT_;
	}	
}


void NewGameBoardManager::TriggerAdvanceComboList(const std::vector<ComboEffectDescription>& advanceComboList, std::vector<ComboEffectBundle*>& nextComboChainList,
												  std::vector<ComboEffectBundle*>& outputComboChainList, const bool& bIsTriggerComboSecondTimeList)
{
	int iRow, iColumn;
	
	float fDelayPerComboInOrder = _TME_DELAY_ACTIVE_COMBO_WHEN_BASIC_MATCHING_;
	int iActivationComboRatio;

	for(auto& comboDescription: advanceComboList)
	{
		Cell position = comboDescription.m_Position; //pComboInChain->m_ComboEffectDescription.m_Position;
		
		ComboEffectBundle* pTriggeredCombo = new ComboEffectBundle();
		pTriggeredCombo->m_fTriggerTime = fDelayPerComboInOrder;
		pTriggeredCombo->m_ComboEffectDescription = comboDescription;
		pTriggeredCombo->m_iNormalChainPhase = 1;

		outputComboChainList.push_back(pTriggeredCombo);

		fDelayPerComboInOrder += _TME_DELAY_ACTIVE_COMBO_WHEN_BASIC_MATCHING_;
		iActivationComboRatio = GetComboDestroyCellRatio(comboDescription.m_eComboEffectType);

		// **************** implement trigger second time of combo 6	****************
		if (comboDescription.m_eComboEffectType == _CET_DESTROY_COLOR_ROW_ || comboDescription.m_eComboEffectType == _CET_DESTROY_COLOR_COLUMN_)
		{
			TriggerWaitingCombo6(comboDescription, pTriggeredCombo, nextComboChainList);
		}
		// **************** implement combo 4-4	****************
		if (comboDescription.m_eComboEffectType == _CET_4_4_EFFECT_)
		{
			float fDelayAfterActivate = pTriggeredCombo->m_fTriggerTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_;
			
			// horizontal
			for( iColumn=0; iColumn < m_iColumnNumber; iColumn++)
				if ( !m_BoardValueMatrix[position.m_iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(position.m_iRow, iColumn))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, position.m_iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate+_TME_BASIC_DESTROY_CELL_TIME_);
				}
			// vertical 
			for( iRow=0; iRow < m_iRowNumber; iRow++)
				if ( !m_BoardValueMatrix[iRow][position.m_iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, position.m_iColumn))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, position.m_iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			
			// add combo cells to destroy list
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[0].m_iRow, comboDescription.m_Phase0CellList[0].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
		}
		// **************** implement combo 4-4-4	****************
		else if (comboDescription.m_eComboEffectType == _CET_4_4_4_EFFECT_)
		{
			float fDelayAfterActivate = pTriggeredCombo->m_fTriggerTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_;

			// horizontal
			for( iColumn=0; iColumn < m_iColumnNumber; iColumn++)
				if ( !m_BoardValueMatrix[position.m_iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(position.m_iRow, iColumn))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, position.m_iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			// vertical 
			for( iRow=0; iRow < m_iRowNumber; iRow++)
				if ( !m_BoardValueMatrix[iRow][position.m_iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, position.m_iColumn))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, position.m_iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate+_TME_BASIC_DESTROY_CELL_TIME_);
				}
			// cross line from bottom left to top right
			int iMinD = _MIN_(position.m_iRow, position.m_iColumn);
			int iMinA = _MIN_( m_iRowNumber - position.m_iRow- 1, m_iColumnNumber - position.m_iColumn -1);
			for(int iDelta = -iMinD; iDelta <= iMinA; iDelta++)
				if ( !m_BoardValueMatrix[position.m_iRow + iDelta][position.m_iColumn + iDelta].m_bIsBlankCell && IsCellDestroyable(position.m_iRow + iDelta, position.m_iColumn + iDelta))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, position.m_iRow + iDelta, position.m_iColumn + iDelta, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate+_TME_BASIC_DESTROY_CELL_TIME_);
				}			

			// cross line from top left to bottom right
			iMinD = _MIN_(position.m_iRow, m_iColumnNumber - position.m_iColumn -1);
			iMinA = _MIN_(m_iRowNumber - position.m_iRow- 1, position.m_iColumn);
			for(int iDelta = -iMinD; iDelta <= iMinA; iDelta++)
				if ( !m_BoardValueMatrix[position.m_iRow + iDelta][position.m_iColumn - iDelta].m_bIsBlankCell && IsCellDestroyable(position.m_iRow + iDelta, position.m_iColumn - iDelta))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, position.m_iRow + iDelta, position.m_iColumn - iDelta, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate+_TME_BASIC_DESTROY_CELL_TIME_);
				}			

			// add combo cells to destroy list
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[0].m_iRow, comboDescription.m_Phase0CellList[0].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
			//if (comboDescription.m_Phase0CellCount > 1)
				//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[1].m_iRow, comboDescription.m_Phase0CellList[1].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
		}
		// ****************	implement combo 4-5	****************
		else if (comboDescription.m_eComboEffectType == _CET_4_5_EFFECT_)
		{
			int iRow, iColumn;
			float fDelayAfterActivate = pTriggeredCombo->m_fTriggerTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_;

			// first explosion
			for(iRow = position.m_iRow-1; iRow <= position.m_iRow+1; iRow++)
				for(iColumn = position.m_iColumn-1; iColumn <= position.m_iColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			// 2 line 
			// horizontal
			for( iColumn=0; iColumn < m_iColumnNumber; iColumn++)
				if ( !m_BoardValueMatrix[position.m_iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(position.m_iRow, iColumn))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, position.m_iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE1_LINE_DELAY_TIME_, fDelayAfterActivate+_TME_BASIC_DESTROY_CELL_TIME_ + _TME_COMBO_4_5_PHASE1_LINE_DELAY_TIME_);
				}
			// vertical 
			for( iRow=0; iRow < m_iRowNumber; iRow++)
				if ( !m_BoardValueMatrix[iRow][position.m_iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, position.m_iColumn))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, position.m_iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE1_LINE_DELAY_TIME_, fDelayAfterActivate + _TME_BASIC_DESTROY_CELL_TIME_ + _TME_COMBO_4_5_PHASE1_LINE_DELAY_TIME_);
				}

			// explosion on top
			int iTopRow = m_iRowNumber-1;
			while (iTopRow >=0 && m_BoardValueMatrix[iTopRow][position.m_iColumn].m_bIsBlankCell)
				iTopRow--;			

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = iTopRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = position.m_iColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;


			for(iRow = iTopRow-1; iRow <= iTopRow+1; iRow++)
				for(iColumn = position.m_iColumn-1; iColumn <= position.m_iColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			// explosion on bot
			int iBotRow = 0;
			while (iBotRow < m_iRowNumber && m_BoardValueMatrix[iBotRow][position.m_iColumn].m_bIsBlankCell)
				iBotRow++;	

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = iBotRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = position.m_iColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 2;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = iBotRow-1; iRow <= iBotRow+1; iRow++)
				for(iColumn = position.m_iColumn-1; iColumn <= position.m_iColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			// explosion of right
			int iTopColumn = m_iColumnNumber-1;
			while (iTopColumn >=0 && m_BoardValueMatrix[position.m_iRow][iTopColumn].m_bIsBlankCell)
				iTopColumn--;	

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = position.m_iRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = iTopColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 3;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = position.m_iRow-1; iRow <= position.m_iRow+1; iRow++)
				for(iColumn = iTopColumn-1; iColumn <= iTopColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			// explosion of left
			int iBotColumn = 0;
			while (iBotColumn < m_iColumnNumber && m_BoardValueMatrix[position.m_iRow][iBotColumn].m_bIsBlankCell)
				iBotColumn++;			

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = position.m_iRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = iBotColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 4;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = position.m_iRow-1; iRow <= position.m_iRow+1; iRow++)
				for(iColumn = iBotColumn-1; iColumn <= iBotColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
		

			// add combo cells to destroy list
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[0].m_iRow, comboDescription.m_Phase0CellList[0].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
		}
		// **************** implement combo 5-5	****************
		else if (comboDescription.m_eComboEffectType == _CET_EXPLOSION_SECOND_ || comboDescription.m_eComboEffectType == _CET_5_5_EFFECT_ || comboDescription.m_eComboEffectType == _CET_5_5_SECOND_EFFECT_)
		{
			float fDelayAfterActivate = pTriggeredCombo->m_fTriggerTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_;

			int iDelta = 2;
			if (comboDescription.m_eComboEffectType == _CET_EXPLOSION_SECOND_)
				iDelta = 1;

			for(int iRow = position.m_iRow-iDelta; iRow <= position.m_iRow+iDelta; iRow++)
				for(int iColumn = position.m_iColumn-iDelta; iColumn <= position.m_iColumn+iDelta; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate+_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			// add combo cells to destroy list						
			if (comboDescription.m_eComboEffectType == _CET_5_5_EFFECT_) //first time trigger of combo 5-5
			{
				m_BoardValueMatrix[position.m_iRow][position.m_iColumn].m_eGemComboType = _GCT_COMBO5_5_TRIGGER_SECOND_TIME_;
				m_BoardValueMatrix[position.m_iRow][position.m_iColumn].m_iGemID = _SPECIAL_COMBO_GEM_ID;
				//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[0].m_iRow, comboDescription.m_Phase0CellList[0].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));

				// add second time trigger to waiting list
				ComboEffectDescription comboDescription2ndTime = comboDescription;
				comboDescription2ndTime.m_eComboEffectType = _CET_5_5_SECOND_EFFECT_;
				comboDescription2ndTime.m_Phase0CellCount = 0;
				m_WaitingTriggerSecondTimeComboList.push_back(comboDescription2ndTime);
			}
			else //second time trigger of 5 and 5-5
			{
				m_BoardValueMatrix[position.m_iRow][position.m_iColumn].Reset();	
				 
				if (comboDescription.m_eComboEffectType == _CET_EXPLOSION_SECOND_) //second trigger of 5
					pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
			}
		}
		// **************** implement combo 5-5-5	****************
		else if (comboDescription.m_eComboEffectType == _CET_5_5_5_EFFECT_)
		{
			float fDelayAfterActivate = pTriggeredCombo->m_fTriggerTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_;

			int iRow, iColumn;

			// first explosion
			for(iRow = position.m_iRow-1; iRow <= position.m_iRow+1; iRow++)
				for(iColumn = position.m_iColumn-1; iColumn <= position.m_iColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			// 4 line 
			// horizontal
			for( iColumn=0; iColumn < m_iColumnNumber; iColumn++)
				if ( !m_BoardValueMatrix[position.m_iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(position.m_iRow, iColumn))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, position.m_iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			// vertical 
			for( iRow=0; iRow < m_iRowNumber; iRow++)
				if ( !m_BoardValueMatrix[iRow][position.m_iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, position.m_iColumn))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, position.m_iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate +_TME_BASIC_DESTROY_CELL_TIME_);
				}

			// cross line from bottom left to top right
			int iMinD1 = _MIN_(position.m_iRow, position.m_iColumn);
			int iMinA1 = _MIN_( m_iRowNumber - position.m_iRow- 1, m_iColumnNumber - position.m_iColumn -1);
			for(int iDelta = -iMinD1; iDelta <= iMinA1; iDelta++)
				if ( !m_BoardValueMatrix[position.m_iRow + iDelta][position.m_iColumn + iDelta].m_bIsBlankCell && IsCellDestroyable(position.m_iRow + iDelta, position.m_iColumn + iDelta))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, position.m_iRow + iDelta, position.m_iColumn + iDelta, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate+_TME_BASIC_DESTROY_CELL_TIME_);
				}			

			// cross line from bot right to top left
			int iMinD2 = _MIN_(position.m_iRow, m_iColumnNumber - position.m_iColumn -1);
			int iMinA2 = _MIN_(m_iRowNumber - position.m_iRow- 1, position.m_iColumn);
			for(int iDelta = -iMinD2; iDelta <= iMinA2; iDelta++)
				if ( !m_BoardValueMatrix[position.m_iRow + iDelta][position.m_iColumn - iDelta].m_bIsBlankCell && IsCellDestroyable(position.m_iRow + iDelta, position.m_iColumn - iDelta))
				{					
					DestroySingleCellByComboUtil(iActivationComboRatio, position.m_iRow + iDelta, position.m_iColumn - iDelta, pTriggeredCombo, nextComboChainList, fDelayAfterActivate, fDelayAfterActivate +_TME_BASIC_DESTROY_CELL_TIME_);
				}			

			// explosion on top
			int iTopRow = m_iRowNumber-1;
			while (iTopRow >=0 && m_BoardValueMatrix[iTopRow][position.m_iColumn].m_bIsBlankCell)
				iTopRow--;			

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = iTopRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = position.m_iColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;


			for(iRow = iTopRow-1; iRow <= iTopRow+1; iRow++)
				for(iColumn = position.m_iColumn-1; iColumn <= position.m_iColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			// explosion on bot
			int iBotRow = 0;
			while (iBotRow < m_iRowNumber && m_BoardValueMatrix[iBotRow][position.m_iColumn].m_bIsBlankCell)
				iBotRow++;	

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = iBotRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = position.m_iColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 2;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = iBotRow-1; iRow <= iBotRow+1; iRow++)
				for(iColumn = position.m_iColumn-1; iColumn <= position.m_iColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			// explosion of right
			int iTopColumn = m_iColumnNumber-1;
			while (iTopColumn >=0 && m_BoardValueMatrix[position.m_iRow][iTopColumn].m_bIsBlankCell)
				iTopColumn--;	

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = position.m_iRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = iTopColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 3;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = position.m_iRow-1; iRow <= position.m_iRow+1; iRow++)
				for(iColumn = iTopColumn-1; iColumn <= iTopColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			// explosion of left
			int iBotColumn = 0;
			while (iBotColumn < m_iColumnNumber && m_BoardValueMatrix[position.m_iRow][iBotColumn].m_bIsBlankCell)
				iBotColumn++;	

			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = position.m_iRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = iBotColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 4;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = position.m_iRow-1; iRow <= position.m_iRow+1; iRow++)
				for(iColumn = iBotColumn-1; iColumn <= iBotColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}
			

			// explosion on bot left
			iBotRow = position.m_iRow - iMinD1;
			iBotColumn = position.m_iColumn - iMinD1;

			while (iBotRow < m_iRowNumber && iBotColumn < m_iColumnNumber && m_BoardValueMatrix[iBotRow][iBotColumn].m_bIsBlankCell)
			{
				iBotRow++;
				iBotColumn++;
			}
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = iBotRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = iBotColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 5;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = iBotRow-1; iRow <= iBotRow+1; iRow++)
				for(iColumn = iBotColumn-1; iColumn <= iBotColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}

			// explosion on top right
			iTopRow = position.m_iRow + iMinA1;
			iTopColumn = position.m_iColumn + iMinA1;

			while (iTopRow>=0 && iTopColumn >=0 && m_BoardValueMatrix[iTopRow][iTopColumn].m_bIsBlankCell)
			{
				iTopRow--;
				iTopColumn--;
			}
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = iTopRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = iTopColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 6;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = iTopRow-1; iRow <= iTopRow+1; iRow++)
				for(iColumn = iTopColumn-1; iColumn <= iTopColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}

			// explosion on  bot right
			iBotRow = position.m_iRow - iMinD2;
			iTopColumn = position.m_iColumn + iMinD2;
			while (iBotRow < m_iRowNumber && iTopColumn >=0 && m_BoardValueMatrix[iTopRow][iTopColumn].m_bIsBlankCell)
			{
				iBotRow++;
				iTopColumn--;
			}
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = iBotRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = iTopColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 7;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = iBotRow-1; iRow <= iBotRow+1; iRow++)
				for(iColumn = iTopColumn-1; iColumn <= iTopColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}

			// explosion on  top left
			iTopRow = position.m_iRow + iMinA2;
			iBotColumn = position.m_iColumn - iMinA2;

			while (iTopRow>=0 && iBotColumn < m_iColumnNumber && m_BoardValueMatrix[iTopRow][iTopColumn].m_bIsBlankCell)
			{
				iTopRow--;
				iBotColumn++;
			}
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iRow = iTopRow;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iColumn = iBotColumn;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iDestroyPhaseIndex = 1;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellList[pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount].m_iGroupIndex = 8;
			pTriggeredCombo->m_ComboEffectDescription.m_Phase0CellCount++;

			for(iRow = iTopRow-1; iRow <= iTopRow+1; iRow++)
				for(iColumn = iBotColumn-1; iColumn <= iBotColumn+1; iColumn++)
			{			
				if (iRow >= 0 && iRow < m_iRowNumber && iColumn >=0 && iColumn < m_iColumnNumber && !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
				{
					DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, fDelayAfterActivate + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ +_TME_BASIC_DESTROY_CELL_TIME_);
				}
			}

			// add combo cells to destroy list
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1, pTriggeredCombo->m_fTriggerTime , false));
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[0].m_iRow, comboDescription.m_Phase0CellList[0].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false ));
			//if (comboDescription.m_Phase0CellCount > 1)
				//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[1].m_iRow, comboDescription.m_Phase0CellList[1].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false ));
		}
		// **************** implement combo 6-4		****************
		else if (comboDescription.m_eComboEffectType == _CET_6_4_EFFECT_)
		{
			float fDelayAfterActivate = pTriggeredCombo->m_fTriggerTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_;

			// only convert normal cells (not letter, no obstacle, no combo) to combo 4 
			for( iRow=0; iRow < m_iRowNumber; iRow++)
				for( iColumn=0; iColumn < m_iColumnNumber; iColumn++)
					if ( !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
						if (m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID < 0 && m_BoardValueMatrix[iRow][iColumn].m_iGemLetterBlockID < 0 &&  m_BoardValueMatrix[iRow][iColumn].m_eGemComboType == _GCT_NONE_
							&& m_BoardValueMatrix[iRow][iColumn].m_iGemID == comboDescription.m_iGemID)
						{
							// convert this cell to combo 4 and trigger it later
							int iRandomDirection = rand() % 2;

							ComboEffectBundle* pNextTriggeredCombo = new ComboEffectBundle();
							pNextTriggeredCombo->m_fTriggerTime = fDelayAfterActivate + _TME_BASIC_COMBO_EXECUTE_TIME_;
							pNextTriggeredCombo->m_ComboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(_GCT_COMBO4_, -1 * iRandomDirection, -1 * (1-iRandomDirection)); // _CET_EXPLOSION_;						
							pNextTriggeredCombo->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
							pNextTriggeredCombo->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
							pNextTriggeredCombo->m_iNormalChainPhase =  pTriggeredCombo->m_iNormalChainPhase+1;
								
							// this cell will be real destroyed at latter phase
							pNextTriggeredCombo->m_DestroyedCells.push_back(DestroyedByComboCell(iRow, iColumn, -1, fDelayAfterActivate + _TME_BASIC_COMBO_EXECUTE_TIME_, false));

							// add it to chain to trigger later
							nextComboChainList.push_back(pNextTriggeredCombo);											

							pTriggeredCombo->m_DestroyedCells.push_back(DestroyedByComboCell(iRow, iColumn, 1, pTriggeredCombo->m_fTriggerTime));
							m_BoardValueMatrix[iRow][iColumn].Reset();
						}

			// add combo cells to destroy list
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[0].m_iRow, comboDescription.m_Phase0CellList[0].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
		}
		// ****************  implement combo 6-5	****************
		else if (comboDescription.m_eComboEffectType == _CET_6_5_EFFECT_)
		{
			float fDelayAfterActivate = pTriggeredCombo->m_fTriggerTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_;

			// only convert normal cells (not letter, no obstacle, no combo) to combo 4 
			for( iRow=0; iRow < m_iRowNumber; iRow++)
				for( iColumn=0; iColumn < m_iColumnNumber; iColumn++)
					if ( !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))
						if (m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID < 0 && m_BoardValueMatrix[iRow][iColumn].m_iGemLetterBlockID < 0 &&  m_BoardValueMatrix[iRow][iColumn].m_eGemComboType == _GCT_NONE_
							&& m_BoardValueMatrix[iRow][iColumn].m_iGemID == comboDescription.m_iGemID)
						{
							// convert this cell to combo 4 and trigger it later
							int iRandomDirection = rand() % 2;

							ComboEffectBundle* pNextTriggeredCombo = new ComboEffectBundle();
							pNextTriggeredCombo->m_fTriggerTime = fDelayAfterActivate + _TME_BASIC_COMBO_EXECUTE_TIME_;
							pNextTriggeredCombo->m_ComboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(_GCT_COMBO5_, -1 * iRandomDirection, -1 * (1-iRandomDirection)); // _CET_EXPLOSION_;						
							pNextTriggeredCombo->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
							pNextTriggeredCombo->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
							pNextTriggeredCombo->m_iNormalChainPhase =  pTriggeredCombo->m_iNormalChainPhase+1;

							// this new combo cell only get destroyed when activate at second time
							//pNextTriggeredCombo->m_DestroyedCells.push_back(DestroyedByComboCell(iRow, iColumn, -1, pTriggeredCombo->m_fTriggerTime + _TME_BASIC_COMBO_EXECUTE_TIME_));
								
							// add it to chain to trigger later
							nextComboChainList.push_back(pNextTriggeredCombo);											

							pTriggeredCombo->m_DestroyedCells.push_back(DestroyedByComboCell(iRow, iColumn, 1, fDelayAfterActivate, false));
							m_BoardValueMatrix[iRow][iColumn].Reset();
						}

			// add combo cells to destroy list
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[0].m_iRow, comboDescription.m_Phase0CellList[0].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
		}	
		// ****************  implement combo 6-6	****************
		else if (comboDescription.m_eComboEffectType == _CET_6_6_EFFECT_)
		{
			float fDelayAfterActivate = pTriggeredCombo->m_fTriggerTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_;

			// destroy all gem
			for( iRow=0; iRow < m_iRowNumber; iRow++)
				for( iColumn=0; iColumn < m_iColumnNumber; iColumn++)
					if ( !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))						
					{											
						DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_6_PHASE2_REMOVE_CELL_AT_TIME_, fDelayAfterActivate + _TME_COMBO_6_PHASE2_REMOVE_CELL_AT_TIME_);
					}

			// add combo cells to destroy list
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1,  pTriggeredCombo->m_fTriggerTime, false));
			//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[0].m_iRow, comboDescription.m_Phase0CellList[0].m_iColumn, -1,  pTriggeredCombo->m_fTriggerTime, false));
		}	
		// ****************  implement combo 6-6-6  ****************
		else if (comboDescription.m_eComboEffectType == _CET_6_6_6_EFFECT_ || comboDescription.m_eComboEffectType == _CET_6_6_6_SECOND_EFFECT_)
		{
			float fDelayAfterActivate = pTriggeredCombo->m_fTriggerTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_;

			// destroy all gem
			for( iRow=0; iRow < m_iRowNumber; iRow++)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
				for( iColumn=0; iColumn < m_iColumnNumber; iColumn++)
					if ( !m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell && IsCellDestroyable(iRow, iColumn))						
					{											
						DestroySingleCellByComboUtil(iActivationComboRatio, iRow, iColumn, pTriggeredCombo, nextComboChainList, fDelayAfterActivate + _TME_COMBO_6_PHASE2_REMOVE_CELL_AT_TIME_, fDelayAfterActivate + _TME_COMBO_6_PHASE2_REMOVE_CELL_AT_TIME_);
					}

			// add combo cells to destroy list						
			if (comboDescription.m_eComboEffectType == _CET_6_6_6_EFFECT_) //first time trigger of combo 5-5
			{
				// add combo cells to destroy list				
				//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[0].m_iRow, comboDescription.m_Phase0CellList[0].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
				//if (comboDescription.m_Phase0CellCount > 1)
					//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(comboDescription.m_Phase0CellList[1].m_iRow, comboDescription.m_Phase0CellList[1].m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));

				// hightest combo cell still remain to activate again later
				m_BoardValueMatrix[position.m_iRow][position.m_iColumn].m_eGemComboType = _GCT_COMBO6_6_6_TRIGGER_SECOND_TIME_;
				m_BoardValueMatrix[position.m_iRow][position.m_iColumn].m_iGemID = _SPECIAL_COMBO_GEM_ID;				

				// add second time trigger to waiting list
				ComboEffectDescription comboDescription2ndTime = comboDescription;
				comboDescription2ndTime.m_eComboEffectType = _CET_6_6_6_SECOND_EFFECT_;
				comboDescription2ndTime.m_Phase0CellCount = 0;
				m_WaitingTriggerSecondTimeComboList.push_back(comboDescription2ndTime);
			}
			else //second time trigger of 6-6-6
			{
				m_BoardValueMatrix[position.m_iRow][position.m_iColumn].Reset();				
				//pTriggeredCombo->m_DestroyedCells.push_back( DestroyedByComboCell(position.m_iRow, position.m_iColumn, -1, pTriggeredCombo->m_fTriggerTime, false));
			}
			
		}	
	}	

	if (bIsTriggerComboSecondTimeList)
		m_WaitingTriggerSecondTimeComboList.clear();

	// activate next chain
	ExecuteComboChain( nextComboChainList);	
}

void NewGameBoardManager::DestroySingleCellByComboUtil(const int& iActivateComboRatio, const int& iRow, const int& iColumn, ComboEffectBundle* pTriggeredCombo, std::vector<ComboEffectBundle*>& nextComboChainList, const float& fDestroyAtTime,
													   const float& fActivateSubsequenceComboAtTime)
{	
	if (m_pObstacleProcessManager->DestroyCellWithObstacle(m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID))
	{
		bool bIsComboActivated = false; //if activate combo by destroy this cell then it wont earn score at this phase but at later (when activate its combo effect)
		if (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType >=_GCT_COMBO4_ && m_BoardValueMatrix[iRow][iColumn].m_eGemComboType <= _GCT_COMBO6_)
		{
			bIsComboActivated = true;

			int iRandomDirection = rand() % 2;

			ComboEffectBundle* pNextTriggeredCombo = new ComboEffectBundle();
			pNextTriggeredCombo->m_fTriggerTime = fActivateSubsequenceComboAtTime;
			pNextTriggeredCombo->m_ComboEffectDescription.m_iActivationScoreRatio = iActivateComboRatio;
			pNextTriggeredCombo->m_ComboEffectDescription.m_eComboEffectType =  GetComboEffectTypeFromComboType(m_BoardValueMatrix[iRow][iColumn].m_eGemComboType, -1 * iRandomDirection, -1 * (1-iRandomDirection)); // _CET_EXPLOSION_;						
			pNextTriggeredCombo->m_ComboEffectDescription.m_Position = Cell(iRow, iColumn);
			pNextTriggeredCombo->m_ComboEffectDescription.m_iGemID = m_BoardValueMatrix[iRow][iColumn].m_iGemID;
			pNextTriggeredCombo->m_iNormalChainPhase =  pTriggeredCombo->m_iNormalChainPhase+1;
								
			// add it to chain to trigger later
			nextComboChainList.push_back(pNextTriggeredCombo);											
		}		

		// combo 5 cell wil be activated/destroyed later
		if (DestroySingleCellUtil(iRow, iColumn, fDestroyAtTime))
		{							
			pTriggeredCombo->m_DestroyedCells.push_back(DestroyedByComboCell(iRow, iColumn, 1, fDestroyAtTime, !bIsComboActivated));
		}	
	}
	else
	{
		DestroyedByComboCell destroyedCell(iRow, iColumn, 1, fDestroyAtTime, false);
		destroyedCell.m_bIsCompleteDestroyed = false;
		destroyedCell.m_bIsEarnScore = false;
		pTriggeredCombo->m_DestroyedCells.push_back(destroyedCell);
	}
}