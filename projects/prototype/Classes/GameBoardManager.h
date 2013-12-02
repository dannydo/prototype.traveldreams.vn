#ifndef _GAME_BOARD_MANAGER_H_
#define _GAME_BOARD_MANAGER_H_

#include <vector>

#define _MAX_GEM_ID_	6
#define _BOARD_MAX_ROW_NUMBER_	9
#define _BOARD_MAX_COLUMN_NUMBER_	9
#define _BOARD_MAX_LENGTH_	9 //max(row, column)
#define	_BOARD_MAX_PATTERN_LENGTH_	5
#define _COMBO_TYPE_COUNT_	3
#define _MAX_LINKED_BLOCK_COUNT_	(_BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_/4)

enum GemComboType_e
{
	_GCT_NONE_,
	_GCT_COMBO4_,
	_GCT_COMBO5_,
	_GCT_COMBO6_
};

struct Cell
{
	int m_iRow, m_iColumn;	
	int m_iCharacter;
			
	Cell()
	{
		m_iRow = 0;
		m_iColumn = 0;		
	}

	Cell (int iRow, int iColumn)
	{
		m_iRow = iRow;
		m_iColumn = iColumn;		
	}	

	bool operator ==(const Cell &input)
	{
		return (m_iRow == input.m_iRow && m_iColumn == input.m_iColumn);
	}

	bool operator !=(const Cell &input)
	{
		return (m_iRow != input.m_iRow || m_iColumn != input.m_iColumn);
	}
};

struct CellValue
{
public:
	int m_iGemID;
	bool m_bIsBlankCell;
	GemComboType_e m_eGemComboType;
	CellValue()
	{
		m_iGemID = -1;
		m_bIsBlankCell = true;
		m_eGemComboType = _GCT_NONE_;
	}
};

struct BasicComboPattern
{
public:
	int m_iTypeID;
	int m_iRowNumber, m_iColumnNumber;
	int m_Pattern[_BOARD_MAX_PATTERN_LENGTH_][_BOARD_MAX_PATTERN_LENGTH_];
};

struct LinkedBlockDescription
{
public:
	int m_iCellCount;
	int m_iStartIndexInDestroyedList;
	int m_iGemID;
	GemComboType_e m_eGemComboType;
};

struct ComboEffectCell : public Cell
{
public:
	GemComboType_e m_eGemComboType;
	int m_iGemID;

	ComboEffectCell() : Cell(), m_eGemComboType(_GCT_NONE_)
	{
		m_iGemID = -1;
	}

	ComboEffectCell(Cell cell, GemComboType_e eGemComboType) : Cell(cell), m_eGemComboType(eGemComboType)
	{
		m_iGemID = -1;
	}

	ComboEffectCell(Cell cell, GemComboType_e eGemComboType, int iGemID) : Cell(cell), m_eGemComboType(eGemComboType), m_iGemID(iGemID)
	{		
	}
};

struct ActivatedComboInChain
{
public:
	ComboEffectCell m_ComboEffectCell;
	std::vector<Cell> m_DestroyCells;
};

class GameBoardManager
{
public:
	GameBoardManager();
	void GenerateGameBoard(int iRowNumber, int iColumnNumber, int iLevel);

	inline int GetRowNumber() { return m_iRowNumber;}
	inline int GetColumnNumber() { return m_iColumnNumber;}
	inline int GetCellValue(const int& iRow, const int& iColumn) { return m_BoardValueMatrix[iRow][iColumn].m_iGemID;}
	inline GemComboType_e GetCellGemComboType(const int& iRow, const int& iColumn) { return m_BoardValueMatrix[iRow][iColumn].m_eGemComboType;}

	inline bool IsBlankCell(const int& iRow, const int& iColumn) { return m_BoardValueMatrix[iRow][iColumn].m_bIsBlankCell;}

	// only iSelectedRow or iSelectedColumn > 1 at one time
	/*bool DoAMove(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn, std::vector<Cell>& destroyCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells, std::vector<Cell>& newCells);*/
	bool RecheckAfterMove(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& destroyCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells, std::vector<Cell>& newCells);

	bool RecheckAfterMoveV2(int iSelectedRow, int iSelectedColumn, int iDeltaRow, int iDeltaColumn,
		std::vector<Cell>& destroyCells, std::vector<ComboEffectCell>& activatedComboEffectCells, std::vector<Cell>& destroyedByEffectCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells, std::vector<Cell>& newCells, std::vector<ComboEffectCell>& newComboCells);

	int GetComboCount(const int& iTypeID) { return m_ComboCountList[iTypeID];}
private:
	void LoadBasicComboPatternList();

	void CountBasicCombo(); 
private:
	int m_iRowNumber, m_iColumnNumber;
	CellValue m_BoardValueMatrix[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	int m_iScore;

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
	
	// combo count based on type, just for test
	int m_ComboCountList[_COMBO_TYPE_COUNT_];
};

#endif
