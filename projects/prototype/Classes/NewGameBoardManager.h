#ifndef _NEW_GAME_BOARD_MANAGER_H_
#define _NEW_GAME_BOARD_MANAGER_H_

#include "GameBoardManager.h"
#include "GameWordManager.h"

enum ComboEffectType
{
	_CET_BASIC_MATCHING_,
	_CET_EXPLOSION_,
	_CET_DIRECTION_LINE_, //direction?
	_CET_DOUBLE_EXPLOSION_
};

struct ComboEffectDescription
{
public:
	Cell m_Position;
	ComboEffectType m_eComboEffectType;
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
	void ExecuteComboChain(std::vector<ComboEffectBundle*>& comboChainList);

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