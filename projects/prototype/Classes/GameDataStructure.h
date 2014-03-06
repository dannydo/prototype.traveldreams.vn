#ifndef _GAME_DATA_STRUCTURE_H_
#define _GAME_DATA_STRUCTURE_H_

#include <vector>
#include <string>

#define _GDS_WORD_MAX_LENGTH_		20
#define _GDS_WORD_MAX_COUNT_		30
#define _GDS_SUB_WORD_MAX_COUNT_	2

#define _MAX_GEM_ID_	6
#define _SPECIAL_COMBO_GEM_ID	10
#define _BONUS_QUEST_GEM_ID_	11
#define _BONUS_WORD_GEM_ID_		12
#define _BONUS_END_GAME_COMBO_GEM_ID	13
//#define _BONUS_END_GAME_CRAZY_GEM_ID	14

#define _BOARD_MAX_ROW_NUMBER_	9
#define _BOARD_MAX_COLUMN_NUMBER_	9
#define _BOARD_MAX_LENGTH_	9 //max(row, column)
#define	_BOARD_MAX_PATTERN_LENGTH_	5
#define _COMBO_TYPE_COUNT_	3
#define _MAX_LINKED_BLOCK_COUNT_	(_BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_/4)

#define _MAX_GAME_LEVEL_	9


// timing effects, temporary place here
#define _TME_BASIC_DESTROY_CELL_TIME_	0.35f
#define _TME_MOVE_CELL_TIME_			0.3f
#define _TME_DELAY_ACTIVE_COMBO_WHEN_BASIC_MATCHING_	0.1f
#define _TME_BASIC_COMBO_EXECUTE_TIME_	0.35f ///basic: 4, 5, 4-4, 4-4-4
#define _TME_COMBO_6_PHASE1_EXECUTE_TIME_		0.15f //
#define _TME_COMBO_6_PHASE2_REMOVE_CELL_AT_TIME_	0.55f
#define _TME_COMBO_6_PHASE2_EXECUTE_TIME_		0.75f //
#define _TME_COMBO_6_5_EXECUTE_TIME_			0.75f
#define _TME_COMBO_4_5_PHASE2_DELAY_TIME_	0.2f //this delay used for 5-5-5 too, execution of each phase = basic combo execute time	

#define _TME_END_GAME_BONUS_PET_RUN_TIME_ 0.35f
#define _TME_END_GAME_PET_EXPLOSION_PHASE_1_TIME_	0.06f
#define _TME_END_GAME_PET_EXPLOSION_PHASE_2_TIME_	0.35f

struct GameConfig
{
public:
	int m_iScoreOfGem;

	//int m_iComboCombineRatio;

	// ***********  combo effect - destroy cell ratio (percent)
	//simple combo 4,5,6
	int m_iCombEffectDestroyCellRatio4;
	int m_iCombEffectDestroyCellRatio5;
	int m_iCombEffectDestroyCellRatio6;
	//double combo: 4-4, 4-5, 5-5, 6-4, 6-5, 6-6
	int m_iCombEffectDestroyCellRatio4_4;
	int m_iCombEffectDestroyCellRatio4_5;
	int m_iCombEffectDestroyCellRatio5_5;
	int m_iCombEffectDestroyCellRatio6_4;
	int m_iCombEffectDestroyCellRatio6_5;
	int m_iCombEffectDestroyCellRatio6_6;
	//tripple combo: 4-4-4, 5-5-5, 6-6-6
	int m_iCombEffectDestroyCellRatio4_4_4;
	int m_iCombEffectDestroyCellRatio5_5_5;
	int m_iCombEffectDestroyCellRatio6_6_6;

	// *********** bonus score when create combo
	//simple combo 4,5,6
	int m_iBonusScoreCreateCombo4;
	int m_iBonusScoreCreateCombo5;
	int m_iBonusScoreCreateCombo6;

	// *********** bonus score when activate combo
	//simple combo 4,5,6
	int m_iBonusScoreActivateCombo4;
	int m_iBonusScoreActivateCombo5;
	int m_iBonusScoreActivateCombo6;
	//double combo: 4-4, 4-5, 5-5, 6-4, 6-5, 6-6
	int m_iBonusScoreActivateCombo4_4;
	int m_iBonusScoreActivateCombo4_5;
	int m_iBonusScoreActivateCombo5_5;
	int m_iBonusScoreActivateCombo6_4;
	int m_iBonusScoreActivateCombo6_5;
	int m_iBonusScoreActivateCombo6_6;
	//tripple combo: 4-4-4, 5-5-5, 6-6-6
	int m_iBonusScoreActivateCombo4_4_4;
	int m_iBonusScoreActivateCombo5_5_5;
	int m_iBonusScoreActivateCombo6_6_6;

	int m_iScoreLetterOfMainWord;
	int m_iScoreRatioCompleteMainWord;

	int m_iScoreLetterOfBonusWord;

	int m_iBonusScoreCompleteBonusQuest;

	int m_iBonusScoreActivateBonusEndGameCombo;
	int m_iEndGameComboEffectDestroyCellRatio;
};

struct LevelTarget
{
public:
	//int m_iNumberOfWidth, m_iNumberOfHeight;
	int m_ScoreOfStars[3];
};

struct WordGenerateConfig
{
public:
	int m_iMinimumRate; 
	int m_iMaximumRate;
	int m_iMaximumLettersAtTheSame;

	// config of main word
	//int m_iInitRateOfMainLetter; //letter of main word
	//int m_iIncreasePercentAfterEachMoveOfMainLetter;
	//int m_iDecreasePercentAfterLetterDestroyedOfMainLetter;
	//int m_iDecreasePercentAfterLetterAppearedOfMainLetter;
	//int m_iRatioBetweenLettersOfMainWord; //in the same generation

	//config of sub-words
	int m_iFixRatioOfSubWords;
	int m_iRatioBetweenLettersOfSubWord;

	//config of trash-words
	int m_iFixRatioOfTrashWords;
	int m_iRatioBetweenLettersOfTrashWord;
};

enum GemComboType_e
{
	_GCT_HAS_LETTER_ =-1,
	_GCT_NONE_ = 0,
	_GCT_COMBO4_ = 1,
	_GCT_COMBO5_ = 2,
	_GCT_COMBO6_ = 3,
	//_GCT_COMBO4_2_ = 4,
	//_GCT_COMBO5_2_ = 5,
	//_GCT_COMBO6_2_ = 6,
	_GCT_COMBO6_WAITING_TRIGGER_ = 7,
	_GCT_COMBO5_TRIGGER_SECOND_TIME_,
	_GCT_COMBO5_5_TRIGGER_SECOND_TIME_,
	_GCT_COMBO6_6_6_TRIGGER_SECOND_TIME_,
	
	_GCT_BONUS_END_GAME_SPECIAL_GEM_,
	_GCT_BONUS_END_GAME_CRAZY_PET_,

	_GCT_SINGLE_COMBO_COUNT_ = 3
};

enum ComboActivateDirection_e
{
	_CAD_LEFT_,
	_CAD_UP_,
	_CAD_RIGHT_,
	_CAD_DOWN_
};

enum ObstacleType_e : int
{
	_OT_NONE_ = 0,
	_OT_LOCK_ROW_AND_COLUMN_,
	_OT_LOCK_ROW_,
	_OT_LOCK_COLUMN_,	
	_OT_SHIELDED_ALIEN_,
	_OT_FROGEN_ALIEN_
};

enum GemType_e : int
{
	_GT_SHEEP_ = 2,
	_GT_CAT_ = 3,
	_GT_HAMSTER_ = 5,
	_GT_BIRD_ = 0,
	_GT_FROG_ = 4,
	_GT_ELEPHANT_ = 1
};

struct Cell
{
	int m_iRow, m_iColumn;	
	//int m_iCharacter;
	//int m_iObstacleBlockID; //used for destroy cell only
			
	Cell()
	{
		m_iRow = 0;
		m_iColumn = 0;		
		//m_iObstacleBlockID = -1;
	}

	Cell (int iRow, int iColumn)
	{
		m_iRow = iRow;
		m_iColumn = iColumn;	
		//m_iObstacleBlockID = -1;
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
	//ObstacleType_e m_eObstacleType;
	int m_iObstacleBlockID;

	// extra info for storing letter	
	int m_iGemLetterBlockID;
	//int m_iWordID;


	CellValue()
	{
		m_iGemID = -1;
		m_bIsBlankCell = true;
		m_eGemComboType = _GCT_NONE_;
		m_iObstacleBlockID = -1;
		//m_eObstacleType = _OT_NONE_;
		
		m_iGemLetterBlockID = -1;
	}

	void Reset()
	{
		m_iGemID = -1;
		m_eGemComboType = _GCT_NONE_;
		m_iObstacleBlockID = -1;
		//	m_eObstacleType = _OT_NONE_;

		m_iGemLetterBlockID = -1;
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

struct DestroyedByComboCell : public Cell
{
public:
	float m_fDestroyAtTime;
	int m_iDestroyPhaseIndex; //remove this???
	int m_iGroupIndex;
	bool m_bIsCompleteDestroyed; //this cell only used to trigger combo step so it's used even if not complete destroy
	bool m_bIsEarnScore; 
	// NOTE: some combo may effect 1 cell 2 times!!!! ==> used dirty flag of obstacle to check it!!!
	// 2 van de lon: - O combo 6 nam giua 2 cell trong. - Da lo keo dong chua combo 6 thi ko the huy??? ==> neu keo no tro ve vi tri ban dau thi co trigger hay ko?
	// *** Chua co hint

	DestroyedByComboCell() : Cell()
	{
		m_fDestroyAtTime = 0;
		m_iDestroyPhaseIndex = 0;
		m_iGroupIndex = 0;
		m_bIsCompleteDestroyed = true;
		m_bIsEarnScore = true;
	}

	DestroyedByComboCell (const int& iRow, const int& iColumn) : Cell(iRow, iColumn)
	{	
		m_fDestroyAtTime = 0;
		m_iDestroyPhaseIndex = 0;
		m_iGroupIndex = 0;
		m_bIsCompleteDestroyed = true;
		m_bIsEarnScore = true;
	}	

	DestroyedByComboCell(Cell cell): Cell(cell)
	{
		m_fDestroyAtTime = 0;
		m_iDestroyPhaseIndex = 0;
		m_iGroupIndex = 0;
		m_bIsCompleteDestroyed = true;
		m_bIsEarnScore = true;
	}

	DestroyedByComboCell (const int& iRow, const int& iColumn, const int& iDestroyPhaseIndex, const float& fDestroyAtTime, bool bIsEarnScore=true) : Cell(iRow, iColumn)
	{	
		m_fDestroyAtTime = 0;
		m_iDestroyPhaseIndex = iDestroyPhaseIndex;
		m_iGroupIndex = 0;
		m_bIsCompleteDestroyed = true;
		m_fDestroyAtTime = fDestroyAtTime;
		m_bIsEarnScore = bIsEarnScore;
	}	
};

struct NewCellInfo : public Cell
{
public:
	int m_iGemLetterBlockID;
	bool m_bIsUnlocked;
	int m_iGemID;	

	NewCellInfo() : Cell()
	{
		m_iGemLetterBlockID = -1;
		m_bIsUnlocked = false;
		m_iGemID = -1;
	}

	NewCellInfo(const int& iRow, const int& iColumn, const int& iGemID) : Cell(iRow, iColumn)
	{
		m_iGemLetterBlockID = -1;
		m_bIsUnlocked = false;
		m_iGemID = iGemID;
	}

	NewCellInfo(const int& iRow, const int& iColumn, const int& iGemLetterBlockID, const bool& bIsUnlocked, const int& iGemID) : Cell(iRow, iColumn)
	{
		m_iGemLetterBlockID = iGemLetterBlockID;
		m_bIsUnlocked = bIsUnlocked;
		m_iGemID = iGemID;
	}
};
/*
struct ActivatedComboInChain
{
public:
	ComboEffectCell m_ComboEffectCell;
	std::vector<Cell> m_DestroyCells;
};*/

struct Word
{
public:	
	char m_sWord[_GDS_WORD_MAX_LENGTH_];		
	int m_iWordLength;

	std::string m_sMeaning;
	std::string m_sSoundFile;
	float m_fSoundLength;
	//std::string m_sSoundVietnameseFile;
	std::string m_sFlashCardImage;

	bool m_ActivatedCharacterFlags[_GDS_WORD_MAX_LENGTH_];
	int m_iRemainInactivatedCharacterCount;
	bool m_AppearedCharacterFlags[_GDS_WORD_MAX_LENGTH_];	
	int m_iRemainNotAppearedCharacterCount;
};

struct Hint
{
public:
	int m_iRow;
	int m_iColumn;
	int m_deltaMove;
};


#endif _GAME_DATA_STRUCTURE_H_