#include "GemLetterManager.h"


GemLetterManager::GemLetterManager()
{
}

int GemLetterManager::AllocFreeBlock(unsigned char& iLetter, bool bIsMainWord)
{
	for(int i=0; i< _BOARD_MAX_ROW_NUMBER_*_BOARD_MAX_COLUMN_NUMBER_; i++)
		if (!m_GemLetterList[i].m_bIsActive)
		{
			m_GemLetterList[i].m_bIsActive = true;
			m_GemLetterList[i].m_iLetter = iLetter;
			m_GemLetterList[i].m_bIsInMainWord = bIsMainWord;
			return i;
		}
	return -1;
}