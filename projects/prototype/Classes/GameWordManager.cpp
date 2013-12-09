#include "GameWordManager.h"
#include "cocos2d.h"
#include "GameBoardManager.h"

using namespace cocos2d;

GameWordManager* GameWordManager::m_pInstance = NULL;

GameWordManager::GameWordManager()
{
}

void GameWordManager::LoadWords()
{
	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::sharedFileUtils()->getFileData("WordList.txt", "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	inputStream >> m_iWordCount;
	std::string sTemp;
	for(int iWordIndex = 0; iWordIndex < m_iWordCount; iWordIndex++)
	{
		inputStream >> m_WordList[iWordIndex].m_sWord;		
		
		// read \r to temp
		std::getline(inputStream, sTemp);		
		std::getline(inputStream, m_WordList[iWordIndex].m_sMeaning);
		//strcpy( m_WordList[iWordIndex].m_sMeaning, sTemp.c_str());

		inputStream >> m_WordList[iWordIndex].m_sSoundFile;
		inputStream >> m_WordList[iWordIndex].m_sSoundVietnameseFile;
		//std::getline(inputStream, m_WordList[iWordIndex].m_sSoundFile);
		//std::getline(inputStream, m_WordList[iWordIndex].m_sSoundVietnameseFile);
	}
}

void GameWordManager::GenerateWordForNewLevel()
{
	int iPreviousWordIndex = UserDefault::getInstance()->getIntegerForKey("wordIndex", -1);
	m_iMainWordIndex = (iPreviousWordIndex + 1) % m_iWordCount;

	UserDefault::getInstance()->setIntegerForKey("wordIndex", m_iMainWordIndex);

	// generate sub words from remain words
	int wordIndexList[_GDS_WORD_MAX_COUNT_];	
	for(int i=0; i < m_iWordCount; i++)
		wordIndexList[i] = i;

	int iWordRemainCount = m_iWordCount-1;
	wordIndexList[m_iMainWordIndex] = iWordRemainCount;

	srand(time(NULL ));
	int iRandomIndex;
	for(int i=0; i< _GDS_SUB_WORD_MAX_COUNT_; i++)
	{
		iRandomIndex = rand() % iWordRemainCount;
		m_SubWordList[i] = wordIndexList[iRandomIndex];
		wordIndexList[iRandomIndex] = wordIndexList[iWordRemainCount-1];
		iWordRemainCount--;
	}

	// reset flags of words
	memset( m_WordList[m_iMainWordIndex].m_ActivedCharacterFlags, 1, _GDS_WORD_MAX_LENGTH_);
	for(int i=0; i< _GDS_SUB_WORD_MAX_COUNT_; i++)
	{
		memset( m_WordList[m_SubWordList[i]].m_ActivedCharacterFlags, 1, _GDS_WORD_MAX_LENGTH_);
	}
}


bool GameWordManager::UnlockWord(const char& iLetter, int& iUnlockedLetterIndexOfMainWord, std::vector<int> (&unlockedLettersIndexOfSubWords)[_GDS_SUB_WORD_MAX_COUNT_])
{
	return true;
}
