#ifndef _NEW_GAME_BOARD_MANAGER_H_
#define _NEW_GAME_BOARD_MANAGER_H_

#include "GameBoardManager.h"
#include "GameWordManager.h"

enum ComboEffectType
{
	_CET_BASIC_MATCHING_,
	_CET_EXPLOSION_,
	_CET_DESTROY_COLOR_, //direction?
	_CET_DOUBLE_EXPLOSION_
};

struct ComboEffectDescription
{
public:
	Cell m_Position;
	ComboEffectType m_eComboEffectType;
	int m_iGemID;
};

enum DoubleComboType
{
	_DCT_DEFAULT
};

struct ComboEffectBundle
{
public:
	std::vector<Cell> m_DestroyedCells;
	std::vector<ComboEffectBundle*> m_TriggeredComboEfectBundleList;
	ComboEffectDescription m_ComboEffectDescription;
	int m_iPhase;
};

struct DoubleComboCreationInfo
{
public:
	Cell m_Position;
	DoubleComboType m_eDoubleComboType;
	Cell m_Cell1, m_Cell2;
};

struct DoubleComboEffectBundle
{
public:
	DoubleComboCreationInfo m_CreationInfo;
	//std::vector<ComboEffectBundle> m_TriggeredComboEfectBundleList;
	//ComboEffectBundle m_ComboEffectBundle; //note: double combo may include a list of comboEffect!!!! ==> this effect will be added to combo chain!!!
};

class NewGameBoardManager : public GameBoardManager
{
public:
	NewGameBoardManager();

	inline GameWordManager* GetGameWordManager() { return m_pGameWordManager;}
	inline const int& GetCurrentScore() { return m_iCurrentScore;}
	inline const int& GetCurrentMove() { return m_iCurrentMove;}
	inline const LevelConfig& GetLevelConfig() { return m_LevelConfig;}
	inline const int& GetCurrentLevel() { return m_iCurrentLevel;}


	bool IsRowLocked(const int& iRow);
	bool IsColumnLocked(const int& iColumn);

	void GenerateGameBoard(int iRowNumber, int iColumnNumber, int iLevel);

	bool RecheckAfterMoveV2(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboEffectBundle>& doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList,std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<Cell>& newCells);		

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
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboEffectBundle>& doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList,std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<Cell>& newCells);		
protected:
	inline ComboEffectType GetComboEffectTypeFromComboType(GemComboType_e eGemComboType);

	// Execute move util methods
	void CopyDataToTempBoardMatrixAndResetFlags(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn);
	void CreateBlockForBasicMatching(bool& bIsNewBlockCreated, std::vector<Cell>& basicMatchingDestroyedCells);
	void RemoveCellsByBasicMatching( std::vector<Cell>& basicMatchingDestroyedCells, std::vector<ComboEffectBundle*>& comboChainList);
	void CreateComboCells(const int& iSelectedRow, const int& iSelectedColumn, const std::vector<Cell>& basicMatchingDestroyedCells, std::vector<ComboEffectCell>& newComboCells);

	void ExecuteComboChain(std::vector<ComboEffectBundle*>& comboChainList);

	void CalculateMoveCells(std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells, std::vector<Cell>& newCells);

	int GetBonusScoreForUnlockMainWord(bool bIncludeIndividualLetters=false);
	int GetBonusScoreForUnlockSubWord(const int& iSubWordID, bool bIncludeIndividualLetters=false);
protected:
	GameWordManager* m_pGameWordManager;

	LevelConfig m_LevelConfig;
	int m_iCurrentLevel;
	int m_iCurrentScore;// current score
	int m_iCurrentMove;
};



#endif _NEW_GAME_BOARD_MANAGER_H_