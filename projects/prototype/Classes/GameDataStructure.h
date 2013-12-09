#ifndef _GAME_DATA_STRUCTURE_H_
#define _GAME_DATA_STRUCTURE_H_

#include <vector>
#include <string>

#define _GDS_WORD_MAX_LENGTH_		12
#define _GDS_WORD_MAX_COUNT_		20
#define _GDS_SUB_WORD_MAX_COUNT_	3

#define _MAX_GEM_ID_	6
#define _BOARD_MAX_ROW_NUMBER_	9
#define _BOARD_MAX_COLUMN_NUMBER_	9
#define _BOARD_MAX_LENGTH_	9 //max(row, column)
#define	_BOARD_MAX_PATTERN_LENGTH_	5
#define _COMBO_TYPE_COUNT_	3
#define _MAX_LINKED_BLOCK_COUNT_	(_BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_/4)


struct GameConfig
{
public:
	int m_iScoreOfGem;

	int m_iComboCombineRatio;
	int m_iCombEffectRatio4;
	int m_iCombEffectRatio5;
	int m_iCombEffectRatio6;
	int m_iCombEffectRatio4_4;
	int m_iCombEffectRatio4_5;
	int m_iCombEffectRatio4_6;
	int m_iCombEffectRatio5_5;
	int m_iCombEffectRatio5_6;
	int m_iCombEffectRatio6_6;

	int m_iScoreOfMainWord;
	int m_iMainWordScoreRatio;
	int m_iScoreOfSubWord;
	int m_iSubWordScoreRatio;
};

struct LevelConfig
{
public:
	//int m_iNumberOfWidth, m_iNumberOfHeight;
	int m_ScoreOfStars[3];
};

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

struct Word
{
public:	
	char m_sWord[_GDS_WORD_MAX_LENGTH_];	
	std::string m_sMeaning;
	std::string m_sSoundFile;
	std::string m_sSoundVietnameseFile;

	bool m_ActivedCharacterFlags[_GDS_WORD_MAX_LENGTH_];
};


#endif _GAME_DATA_STRUCTURE_H_