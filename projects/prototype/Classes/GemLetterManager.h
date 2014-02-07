#ifndef _GEM_LETTER_MANAGER_H_
#define _GEM_LETTER_MANAGER_H_

#include "GameDataStructure.h"

struct GemLetterData
{
public:
	bool m_bIsActive;
	unsigned char m_iLetter;
	bool m_bIsInMainWord;

	GemLetterData()
	{
		m_bIsActive = false;
		m_iLetter = 255;
		m_bIsInMainWord = false;
	}
};

class GemLetterManager
{
private:
	GemLetterData m_GemLetterList[_BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_];
	std::vector<int> m_BlockWaitingClearList;
public:
	GemLetterManager();

	inline GemLetterData GetGemLetterData(const int& iBlockID) { return m_GemLetterList[iBlockID];}
	int AllocFreeBlock(unsigned char& iLetter, bool bIsMainWord);
};


#endif _GEM_LETTER_MANAGER_H_