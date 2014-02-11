#ifndef _NEW_GAME_BOARD_MANAGER_H_
#define _NEW_GAME_BOARD_MANAGER_H_

#include "GameBoardManager.h"
#include "GameWordManager.h"
#include "GameConfigManager.h"
#include "ObstacleProcessManager.h"
#include "GemLetterManager.h"

enum ComboEffectType
{
	_CET_BASIC_MATCHING_,
	_CET_EXPLOSION_,
	_CET_DESTROY_COLOR_,
	_CET_DOUBLE_EXPLOSION_
};

struct ComboEffectDescription
{
public:
	Cell m_Position;
	ComboEffectType m_eComboEffectType;
	int m_iGemID;
};

/*enum DoubleComboType
{
	_DCT_DEFAULT
};*/

struct ComboEffectBundle
{
public:
	std::vector<Cell> m_DestroyedCells;
	//std::vector<ComboEffectBundle*> m_TriggeredComboEfectBundleList;
	ComboEffectDescription m_ComboEffectDescription;
	int m_iNormalChainPhase;
	int m_iActivatedByCombo5Phase;

	ComboEffectBundle()
	{
		m_iNormalChainPhase = 0;
		m_iActivatedByCombo5Phase = 0;
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

	bool IsRowLocked(const int& iRow);
	bool IsColumnLocked(const int& iColumn);

	void GenerateGameBoard(int iLevel);

	bool RecheckAfterMoveV2(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboCreationInfo>& newDoubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList, std::vector<ComboEffectBundle*>& triggeredCombo5ChainList,
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
	inline ComboEffectType GetComboEffectTypeFromComboType(GemComboType_e eGemComboType);

	bool CanComboBeUpgraded(GemComboType_e eComboType);
	GemComboType_e UpgradeCombo(GemComboType_e eComboType);

	inline bool IsCellDestroyable(const int& iRow, const int& iColumn) { return (m_BoardValueMatrix[iRow][iColumn].m_iGemID >= 0 && m_BoardValueMatrix[iRow][iColumn].m_eGemComboType != _GCT_COMBO5_WAITING_TRIGGER_);}
	inline bool DestroySingleCellUtil(const int& iRow, const int& iColumn);

	// Execute move util methods
	void CopyDataToTempBoardMatrixAndResetFlags(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn);
	void CreateBlockForBasicMatching(bool& bIsNewBlockCreated, std::vector<Cell>& basicMatchingDestroyedCells);
	void RemoveCellsByBasicMatching( std::vector<Cell>& basicMatchingDestroyedCells, std::vector<ComboEffectBundle*>& comboChainList);
	void CreateComboCells(const int& iSelectedRow, const int& iSelectedColumn, const std::vector<Cell>& basicMatchingDestroyedCells, std::vector<ComboEffectCell>& newComboCells);

	void ExecuteComboChain(std::vector<ComboEffectBundle*>& comboChainList);

	void CalculateMoveCells(std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells); //, std::vector<Cell>& newCells);
	void CreateUnlockedGemLetterFromWaitingList(std::vector<NewCellInfo>& newCells);
	void GenerateNewGems(std::vector<NewCellInfo>& newCells, bool bIsNewMove);

	int GetBonusScoreForUnlockMainWord(bool bIncludeIndividualLetters=false);
	int GetBonusScoreForUnlockSubWord(const int& iSubWordID, bool bIncludeIndividualLetters=false);


	void TriggerWaitingCombo5List(std::vector<ComboEffectBundle*>& comboChainList, std::vector<ComboEffectBundle*>& triggeredCombo5ChainList);
protected:
	GameWordManager* m_pGameWordManager;	
	ObstacleProcessManager* m_pObstacleProcessManager;
	GemLetterManager m_GemLetterManager;

	//LevelTarget m_LevelConfig;
	int m_iCurrentLevel;
	int m_iCurrentScore;// current score
	int m_iCurrentMove;

	std::vector<ComboEffectDescription> m_WaitingTriggerCombo5List;
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