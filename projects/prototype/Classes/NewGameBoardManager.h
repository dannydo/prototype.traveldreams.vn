#ifndef _NEW_GAME_BOARD_MANAGER_H_
#define _NEW_GAME_BOARD_MANAGER_H_

#include "GameBoardManager.h"
#include "GameWordManager.h"
#include "GameConfigManager.h"
#include "ObstacleProcessManager.h"
#include "GemLetterManager.h"

enum ComboEffectType
{
	_CET_NONE_,
	_CET_BASIC_MATCHING_,
	_CET_DESTROY_ROW_,
	_CET_DESTROY_COLUMN_,
	_CET_EXPLOSION_,
	_CET_EXPLOSION_SECOND_,
	_CET_DESTROY_COLOR_ROW_, //activate with nearby cells on row
	_CET_DESTROY_COLOR_COLUMN_, //activate with nearby cells on row	
	_CET_4_4_EFFECT_,
	_CET_4_4_4_EFFECT_,
	_CET_4_5_EFFECT_,
	_CET_5_5_EFFECT_,
	_CET_5_5_SECOND_EFFECT_,
	_CET_5_5_5_EFFECT_,
	_CET_6_4_EFFECT_,
	_CET_6_5_EFFECT_,
	_CET_6_6_EFFECT_,
	_CET_6_6_6_EFFECT_,
	_CET_6_6_6_SECOND_EFFECT_
};

struct ComboEffectDescription
{
public:
	Cell m_Position;
	DestroyedByComboCell m_Phase0CellList[15]; //cells that participate at phase0 of combo, that could be nearby other combos in case of double/tripple combo
	int m_Phase0CellCount;
	ComboEffectType m_eComboEffectType;
	int m_iGemID;

	ComboEffectDescription()
	{
		m_Phase0CellCount = 0;
		m_iGemID = -1;
		m_eComboEffectType = _CET_EXPLOSION_;
	}
};

/*enum DoubleComboType
{
	_DCT_DEFAULT
};*/

struct ComboEffectBundle
{
public:
	std::vector<DestroyedByComboCell> m_DestroyedCells;
	//std::vector<ComboEffectBundle*> m_TriggeredComboEfectBundleList;
	ComboEffectDescription m_ComboEffectDescription;

	float m_fTriggerTime;

	int m_iNormalChainPhase;
	//int m_iActivatedByCombo6Phase;

	ComboEffectBundle()
	{
		m_iNormalChainPhase = 0;
		//m_iActivatedByCombo6Phase = 0;
		m_fTriggerTime = 0;
	}
};

struct DoubleComboCreationInfo
{
public:
	Cell m_Position;
	//DoubleComboType m_eDoubleComboType;
	GemComboType_e m_eComboType;
	Cell m_Cell1, m_Cell2;
};

struct LevelBossInfo
{
public:
	bool m_bIsEnable;

	float m_fBossStateChangeAtTime;
	bool m_bJustReleaseALetter; //true: release a letter, false: decrease 1 HP
	int m_iRemainLettersCount;
	int m_iCurrentHitPoint;

	LevelBossInfo()
	{
		m_bIsEnable = false;

		m_fBossStateChangeAtTime = 0;
		m_bJustReleaseALetter = false;

		m_iRemainLettersCount = 0;
		m_iCurrentHitPoint = 0;
	}
};

/*
struct DoubleComboEffectBundle
{
public:
	DoubleComboCreationInfo m_CreationInfo;
	//std::vector<ComboEffectBundle> m_TriggeredComboEfectBundleList;
	//ComboEffectBundle m_ComboEffectBundle; //note: double combo may include a list of comboEffect!!!! ==> this effect will be added to combo chain!!!
};
*/
class NewGameBoardManager : public GameBoardManager
{
public:
	NewGameBoardManager();
	~NewGameBoardManager();

	inline GameWordManager* GetGameWordManager() { return m_pGameWordManager;}
	inline ObstacleProcessManager* GetObstacleProcessManager() { return m_pObstacleProcessManager;}
	inline const int& GetCurrentScore() { return m_iCurrentScore;}
	inline const int& GetCurrentMove() { return m_iCurrentMove;}
	inline const LevelConfig& GetLevelConfig() { return *m_pLevelConfig;}
	inline const int& GetCurrentLevel() { return m_iCurrentLevel;}

	inline void SetObstacleBlockID(const int& iRow, const int& iColumn, const int& iObstacleBlockID) { m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID = iObstacleBlockID;}	
	inline int GetObstacleBlockID(const int& iRow, const int& iColumn) { return m_BoardValueMatrix[iRow][iColumn].m_iObstacleBlockID;}
	void ClearObstacleBlockID(const int& iObstacleBlockID);

	inline int AllocFreeGemLetterBlock(unsigned char& iLetter, bool bIsMainWord) { return m_GemLetterManager.AllocFreeBlock(iLetter, bIsMainWord); }
	inline GemLetterData& GetGemLetterData(const int& iBlockID) { return m_GemLetterManager.GetGemLetterData(iBlockID);}
	inline GemLetterData FreeGemLetterBlock(const int& iBlockID) { return m_GemLetterManager.FreeGemLetterBlock(iBlockID);}

	inline const LevelBossInfo GetLevelBossInfo() { return m_LevelBossInfo;}
	inline bool IsBossStateChanged() { return m_bIsBossStateChanged;}
	inline void KillBoss() { m_LevelBossInfo.m_bIsEnable = false;}

	bool IsRowLocked(const int& iRow);
	bool IsColumnLocked(const int& iColumn);

	void GenerateGameBoard(int iLevel);

	bool RecheckAfterMoveV2(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboCreationInfo>& newDoubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList, std::vector<ComboEffectBundle*>& triggeredCombo6ChainList,
		std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<NewCellInfo>& unlockedLetterCells,
		std::vector<NewCellInfo>& newCells,
		bool bIsNewMove);		

	bool FastCheckBlocks( int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& basicMatchingDestroyedCells);

	void PreCheckComboEffect(int iSelectedRow, int iSelectedColumn, ComboActivateDirection_e eDirection, std::vector<Cell>& basicMatchingDestroyedCells);
	void ExecuteComboEffect(int iSelectedRow, int iSelectedColumn, ComboActivateDirection_e eDirection,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboCreationInfo>& newDoubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList, std::vector<ComboEffectBundle*>& triggeredCombo5ChainList,
		std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<NewCellInfo>& unlockedLetterCells,
		std::vector<NewCellInfo>& newCells);

	int DecreaseMove() { return (m_iCurrentMove--);}

	// score and stars
	int GetScoreForUnlockLetterInMainWord();
	int GetScoreForUnlockLetterInSubWord();

	int IncreaseScoreForLetterInMainWord();
	int IncreaseScoreForLetterInSubWords(const int& iLetterCount);
	int IncreaseScoreForUnlockMainWord();
	int IncreaseScoreForUnlockSubWord(const int& iSubWordID);
	
	int IncreaseScoreForDestroyCells(const int& iGemCount, const ComboEffectType& eComboEffectType);
	//int IncreaseScoreComboEffect(const int& iGemCount, const GemComboType_e& eComboType)

	int GetEarnedStars();


	// Bonus end game
	bool ExecuteEndGameBonus(
		std::vector<ComboEffectCell>& convertedToComboCells,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboCreationInfo>& doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList, std::vector<ComboEffectBundle*>& triggeredCombo5ChainList,
		std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<NewCellInfo>& unlockedLetterCells,
		std::vector<NewCellInfo>& newCells);		
protected:
	inline ComboEffectType GetComboEffectTypeFromComboType(GemComboType_e eGemComboType, const int& iDeltaMoveRow, const int& iDeltaMoveColumn);	

	inline bool IsCellDestroyable(const int& iRow, const int& iColumn) { return (m_BoardValueMatrix[iRow][iColumn].m_iGemID >= 0 && (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType < _GCT_COMBO6_WAITING_TRIGGER_ 
																				|| m_BoardValueMatrix[iRow][iColumn].m_eGemComboType > _GCT_COMBO6_6_6_TRIGGER_SECOND_TIME_ ));}
	inline bool DestroySingleCellUtil(const int& iRow, const int& iColumn, const float fDestroyAtTime);	
	inline void DestroySingleCellByComboUtil(const int& iRow, const int& iColumn, ComboEffectBundle* pTriggerCombo, std::vector<ComboEffectBundle*>& nextComboChainList, const float& fDestroyAtTime,
		const float& fActivateSubsequenceComboAtTime);

	inline bool IsSimpleCombo(const GemComboType_e& eGemComboType) { return (eGemComboType >= _GCT_COMBO4_ && eGemComboType <= _GCT_COMBO6_);}

	// Execute move util methods
	void CopyDataToTempBoardMatrixAndResetFlags(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn);
	void CreateBlockForBasicMatching(bool& bIsNewBlockCreated, std::vector<Cell>& basicMatchingDestroyedCells);
	void RemoveCellsByBasicMatching( std::vector<Cell>& basicMatchingDestroyedCells, std::vector<ComboEffectBundle*>& comboChainList, const int& iMoveRow, const int& iMoveColumn);
	void CreateComboCells(const int& iSelectedRow, const int& iSelectedColumn, const std::vector<Cell>& basicMatchingDestroyedCells, std::vector<ComboEffectCell>& newComboCells);

	void ExecuteComboChain(std::vector<ComboEffectBundle*>& comboChainList);

	void CalculateMoveCells(std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells); //, std::vector<Cell>& newCells);
	void CreateUnlockedGemLetterFromWaitingList(std::vector<NewCellInfo>& newCells);
	void GenerateNewGems(std::vector<NewCellInfo>& newCells, bool bIsNewMove);

	void CalculateMoveEffectOnBoss(bool& bIsBossStateChanged);

	int GetBonusScoreForUnlockMainWord(bool bIncludeIndividualLetters=false);
	int GetBonusScoreForUnlockSubWord(const int& iSubWordID, bool bIncludeIndividualLetters=false);

	//  trigger combos at its second phase
	void TriggerWaitingCombo6(const ComboEffectDescription& comboDescription, ComboEffectBundle* &pTriggeredCombo, std::vector<ComboEffectBundle*>& comboChainList);

	// methods for processing double and tripple combos
	void FindSortAndFilterAdvanceCombos(std::vector<ComboEffectDescription>& advanceComboList);
	ComboEffectType GetComboEffectTypeOfAdvanceCombo(ComboEffectCell (&linkCellList)[3], const int& iLinkCellCount);

	void TriggerAdvanceComboList(const std::vector<ComboEffectDescription>& advanceComboList, std::vector<ComboEffectBundle*>& nextComboChainList, std::vector<ComboEffectBundle*>& outputComboChainList, const bool& bIsTriggerComboSecondTimeList);
protected:
	GameWordManager* m_pGameWordManager;	
	ObstacleProcessManager* m_pObstacleProcessManager;
	GemLetterManager m_GemLetterManager;

	//LevelTarget m_LevelConfig;
	int m_iCurrentLevel;
	int m_iCurrentScore;// current score
	int m_iCurrentMove;

	LevelBossInfo m_LevelBossInfo;
	bool m_bIsBossStateChanged;

	//std::vector<ComboEffectDescription> m_WaitingTriggerCombo6List;
	std::vector<ComboEffectDescription> m_WaitingTriggerSecondTimeComboList; //combo 5, combo 5-5, combo 6-6-6, this include waiting combo 6 too!!!!
	std::vector<NewCellInfo> m_UnlockedGemLetterCellList;
private:
	// utility methods to find hint on temporary matrix
	bool haveCellMatch3(const Cell& cell);
	int haveMatch3WHenMoveRow(const int& iRow);
	int haveMatch3WHenMoveColumn(const int& iColumn);
public:
	Hint findHintForGame(); 
};



#endif _NEW_GAME_BOARD_MANAGER_H_