#ifndef _GAME_BOARD_MANAGER_H_
#define _GAME_BOARD_MANAGER_H_

#include <vector>

#include "GameDataStructure.h"
#include "GameConfigManager.h"

class GameBoardManager
{
public:
	GameBoardManager();
	void GenerateGameBoard(int iLevel);

	inline const LevelConfig* GetLevelConfig() { return m_pLevelConfig;}
	inline int GetRowNumber() { return m_iRowNumber;}
	inline int GetColumnNumber() { return m_iColumnNumber;}
	inline int GetCellValue(const int& iRow, const int& iColumn) { return m_BoardValueMatrix[iRow][iColumn].m_iGemID;}
	inline GemComboType_e GetCellGemComboType(const int& iRow, const int& iColumn) { return m_BoardValueMatrix[iRow][iColumn].m_eGemComboType;}
	inline bool IsComboCell(const int& iRow, const int& iColumn) { return (m_BoardValueMatrix[iRow][iColumn].m_eGemComboType > _GCT_NONE_);}

	//inline ObstacleType_e GetCellObstacleType(const int& iRow, const int& iColumn) { return m_BoardValueMatrix[iRow][iColumn].m_eObstacleType;}

	inline bool IsBlankCell(const int& iRow, const int& iColumn) { return m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell;}

	// only iSelectedRow or iSelectedColumn 0 1 at one time	 (iDeltaRow and iDeltaColumn != 0 corresponding)
	bool RecheckAfterMoveV2(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& destroyCells, std::vector<ComboEffectCell>& activatedComboEffectCells, std::vector<Cell>& destroyedByEffectCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells, std::vector<Cell>& newCells, std::vector<ComboEffectCell>& newComboCells);

	int GetComboCount(const int& iTypeID) { return m_ComboCountList[iTypeID];}
	
protected:
	//void LoadGameConfig();
	void LoadBasicComboPatternList();

	void CountBasicCombo(); 
protected:
	const GameConfig& m_GameConfig;	
	LevelConfig* m_pLevelConfig;	

	int m_iRowNumber, m_iColumnNumber;
	CellValue m_BoardValueMatrix[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];		

	// use this matrix to check board after move
	CellValue m_TemporaryValueMatrix[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	// checked flag used to mark cell that already check horizontal or vertical
	bool m_CheckedHorizotalMatrix[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	bool m_CheckedVerticalMatrix[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	bool m_FlagDestroyedMatrix[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	LinkedBlockDescription m_LinkedBlockList[_MAX_LINKED_BLOCK_COUNT_];
	int m_iLinkedBlockCount;

	BasicComboPattern m_BasicComboPatternList[20];
	int m_iBasicComboPatternCount;

	// word manager
	
	// combo count based on type, just for test
	int m_ComboCountList[_COMBO_TYPE_COUNT_];
};

#include <istream>>
#include <streambuf>

struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
	}
};

#endif
