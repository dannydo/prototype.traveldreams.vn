#include "GameWordManager.h"
#include "cocos2d.h"
#include "GameBoardManager.h"

using namespace cocos2d;

GameWordManager* GameWordManager::m_pInstance = NULL;

GameWordManager::GameWordManager()
{
	m_iCountOfLettersOnBoard = 0;
}

void GameWordManager::LoadWordGenerateConfig()
{
	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData("GenerateWordConfig.txt", "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	std::string sTemp;

	// general config
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iMinimumRate;
	std::getline(inputStream, sTemp);		
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iMaximumRate;
	std::getline(inputStream, sTemp);		
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iMaximumLettersAtTheSame;
	std::getline(inputStream, sTemp);	

	// config of main word
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iInitRateOfMainLetter;
	std::getline(inputStream, sTemp);		
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iIncreasePercentAfterEachMoveOfMainLetter;
	std::getline(inputStream, sTemp);		
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iDecreasePercentAfterLetterDestroyedOfMainLetter;
	std::getline(inputStream, sTemp);	
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iDecreasePercentAfterLetterAppearedOfMainLetter;
	std::getline(inputStream, sTemp);	
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iRatioBetweenLettersOfMainWord;
	std::getline(inputStream, sTemp);	

	// config of sub words
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iFixRatioOfSubWords;
	std::getline(inputStream, sTemp);		
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iRatioBetweenLettersOfSubWord;
	std::getline(inputStream, sTemp);		

	// config of trash words
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iFixRatioOfTrashWords;
	std::getline(inputStream, sTemp);		
	std::getline(inputStream, sTemp);		
	inputStream >> m_WordGenerateConfig.m_iRatioBetweenLettersOfTrashWord;
	std::getline(inputStream, sTemp);	
}

void GameWordManager::LoadWords()
{
	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData("WordList.txt", "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	inputStream >> m_iWordCount;
	std::string sTemp;
	for(int iWordIndex = 0; iWordIndex < m_iWordCount; iWordIndex++)
	{
		inputStream >> m_WordList[iWordIndex].m_sWord;		
		m_WordList[iWordIndex].m_iWordLength = strlen( m_WordList[iWordIndex].m_sWord);

		// read \r to temp
		std::getline(inputStream, sTemp);		
		std::getline(inputStream, m_WordList[iWordIndex].m_sMeaning);
		//strcpy( m_WordList[iWordIndex].m_sMeaning, sTemp.c_str());

		inputStream >> m_WordList[iWordIndex].m_sSoundFile;
		inputStream >> m_WordList[iWordIndex].m_sSoundVietnameseFile;		
		//std::getline(inputStream, m_WordList[iWordIndex].m_sSoundFile);
		//std::getline(inputStream, m_WordList[iWordIndex].m_sSoundVietnameseFile);

		inputStream >> m_WordList[iWordIndex].m_sFlashCardImage;
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

		// log for test
		CCLOG( "Bonus word %d: %s", i, m_WordList[m_SubWordList[i]].m_sWord);
	}	

	// reset data
	ResetDataForNewPlay();
}

void GameWordManager::RetryCurrentLevel()
{
	// reset data
	ResetDataForNewPlay();
}

void GameWordManager::ResetDataForNewPlay()
{
	// reset flags of words
	memset( m_WordList[m_iMainWordIndex].m_ActivatedCharacterFlags, 0, _GDS_WORD_MAX_LENGTH_);
	memset( m_WordList[m_iMainWordIndex].m_AppearedCharacterFlags, 0, _GDS_WORD_MAX_LENGTH_);
	m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount = m_WordList[m_iMainWordIndex].m_iWordLength;
	m_WordList[m_iMainWordIndex].m_iRemainNotAppearedCharacterCount = m_WordList[m_iMainWordIndex].m_iWordLength;

	for(int i=0; i< _GDS_SUB_WORD_MAX_COUNT_; i++)
	{
		// note: currently, already-appear-flag only used on main word
		memset( m_WordList[m_SubWordList[i]].m_ActivatedCharacterFlags, 0, _GDS_WORD_MAX_LENGTH_);
		memset( m_WordList[m_SubWordList[i]].m_AppearedCharacterFlags, 0, _GDS_WORD_MAX_LENGTH_);		
		
		m_WordList[m_SubWordList[i]].m_iRemainInactivatedCharacterCount = m_WordList[m_iMainWordIndex].m_iWordLength;
		m_WordList[m_SubWordList[i]].m_iRemainNotAppearedCharacterCount = m_WordList[m_iMainWordIndex].m_iWordLength;
	}

	// reset counter
	m_iCountOfLettersOnBoard = 0;

	// reset ratio
	m_iMainWordGenerateRate = m_WordGenerateConfig.m_iInitRateOfMainLetter;
	m_iSubWordGenerateRate = m_WordGenerateConfig.m_iFixRatioOfSubWords;
	m_iTrashWordGenerateRate = m_WordGenerateConfig.m_iFixRatioOfTrashWords;

	// reset trash letters collection
	m_TrashLettersCollection.clear();

	// create sub-word letters collection
	m_SubWordLettersCollection.clear();
	for(int i=0; i < _GDS_SUB_WORD_MAX_COUNT_; i++)
		for(int j=0; j< m_WordList[m_SubWordList[i]].m_iWordLength; j++)
		{
			bool bAlreadyInList = false;
			for(auto sLetter : m_SubWordLettersCollection)
			{
				if (sLetter == m_WordList[m_SubWordList[i]].m_sWord[j])
				{
					bAlreadyInList = true;
					break;
				}
			}

			if (!bAlreadyInList) //check in main word
			{
				for(int iIndex=0; iIndex < m_WordList[m_iMainWordIndex].m_iWordLength; iIndex++)
					if (m_WordList[m_iMainWordIndex].m_sWord[iIndex] == m_WordList[m_SubWordList[i]].m_sWord[j])
					{
						bAlreadyInList = true;
						break;
					}
			}

			if (!bAlreadyInList)
				m_SubWordLettersCollection.push_back( m_WordList[m_SubWordList[i]].m_sWord[j]);
		}
}

bool GameWordManager::UnlockLetter(const unsigned char& iLetter, int& iUnlockedLetterIndexOfMainWord, std::vector<int> (&unlockedLettersIndexOfSubWords)[_GDS_SUB_WORD_MAX_COUNT_],
								   bool& bIsMainWordJustUnlocked, bool (&justUnlockedSubWords)[_GDS_SUB_WORD_MAX_COUNT_])
{
	bool bNewCharacterIsUnlocked = false;
	bIsMainWordJustUnlocked = false;
	memset( justUnlockedSubWords, 0, sizeof(justUnlockedSubWords));

	// check main word
	iUnlockedLetterIndexOfMainWord = -1;
	if (m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount > 0)
	{
		for(int i=0; i< m_WordList[m_iMainWordIndex].m_iWordLength; i++)
		{
			if (m_WordList[m_iMainWordIndex].m_sWord[i] != 0 && m_WordList[m_iMainWordIndex].m_ActivatedCharacterFlags[i]== false)		
				if (iLetter == m_WordList[m_iMainWordIndex].m_sWord[i])
				{
					iUnlockedLetterIndexOfMainWord = i;
					m_WordList[m_iMainWordIndex].m_ActivatedCharacterFlags[i] = true;					
					m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount--;

					if (!m_WordList[m_iMainWordIndex].m_AppearedCharacterFlags[i])
					{
						m_WordList[m_iMainWordIndex].m_AppearedCharacterFlags[i] = true; //note: a letter can appear in both main word and sub-word
						m_WordList[m_iMainWordIndex].m_iRemainNotAppearedCharacterCount--;
					}

					// check if main word is just unlocked
					bIsMainWordJustUnlocked = (m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount == 0);

					bNewCharacterIsUnlocked = true;							 
					break;
				}		
		}	
	}
	// if letter of main word is unlocked then decrease the appear rate of remain letters
	if (iUnlockedLetterIndexOfMainWord >=0)
		m_iMainWordGenerateRate = MAX( m_iMainWordGenerateRate- m_WordGenerateConfig.m_iDecreasePercentAfterLetterDestroyedOfMainLetter, m_WordGenerateConfig.m_iMinimumRate);

	for(int i=0; i < _GDS_SUB_WORD_MAX_COUNT_; i++)
	{
		int iSubWordID = m_SubWordList[i];
		for(int j=0; j < m_WordList[iSubWordID].m_iWordLength; j++)		
		{			
			if (m_WordList[iSubWordID].m_sWord[j] != 0 && m_WordList[iSubWordID].m_ActivatedCharacterFlags[j]== false)		
			{
				if (iLetter == m_WordList[iSubWordID].m_sWord[j])
				{					
					unlockedLettersIndexOfSubWords[i].push_back(j);

					m_WordList[iSubWordID].m_ActivatedCharacterFlags[j] = true;					
					m_WordList[iSubWordID].m_iRemainInactivatedCharacterCount--;

					if (!m_WordList[iSubWordID].m_AppearedCharacterFlags[i])
					{
						m_WordList[iSubWordID].m_AppearedCharacterFlags[i] = true; //note: a letter can appear in both main word and sub-word
						m_WordList[iSubWordID].m_iRemainNotAppearedCharacterCount--;
					}

					// check if sub word is just unlocked
					justUnlockedSubWords[i] = (m_WordList[iSubWordID].m_iRemainInactivatedCharacterCount == 0);

					bNewCharacterIsUnlocked = true;
				}				
			}
		}
	}
		
	m_iCountOfLettersOnBoard--;
	if (bNewCharacterIsUnlocked)
		AddLetterToTrashCollection(iLetter);

	return bNewCharacterIsUnlocked;
}


bool SuccessWithPercentRatio(const int& iPercentRatio)
{
	if (rand() % 100 <= iPercentRatio)
		return true;
	else
		return false;
}

void GameWordManager::GenerateNewLetters(const int& iGemCount, std::vector<unsigned char>& outputLettersForGems, bool bIsNewMove)
{
	// init outputLettersForGems as empty list
	outputLettersForGems.clear();
	for(int i=0; i< iGemCount; i++)
		outputLettersForGems.push_back(-1);	

	// check maximum possible new letters
	int iMaxNewLetters = MAX( MIN( iGemCount, m_WordGenerateConfig.m_iMaximumLettersAtTheSame - m_iCountOfLettersOnBoard), 0);	
	//if (iMaxNewLetters == 0)
		//return;	

	// generate new letters
	std::vector<unsigned char> characterOutput;

	bool bShouldGenerateLetter = true;
	unsigned char sLetter;

	srand(time(NULL));

	// generate letters from main word
	int iAppearRatio = m_iMainWordGenerateRate;	
	while(bShouldGenerateLetter && characterOutput.size() < iMaxNewLetters)
	{
		bShouldGenerateLetter = SuccessWithPercentRatio(iAppearRatio);
		if (bShouldGenerateLetter)
		{
			if (GenerateLetterFromMainWord(sLetter))
			{
				characterOutput.push_back(sLetter);				
				iAppearRatio *= m_WordGenerateConfig.m_iRatioBetweenLettersOfMainWord / 100;				
			}
			else
				bShouldGenerateLetter = false;
		}
	}

	if (characterOutput.size() > 0) //appear letter in main word ==> update ratio for next generation
	{
		m_iMainWordGenerateRate = MAX( m_iMainWordGenerateRate-m_WordGenerateConfig.m_iDecreasePercentAfterLetterAppearedOfMainLetter, m_WordGenerateConfig.m_iMinimumRate);
	}
	

	// generate letters from sub words
	iAppearRatio = m_iSubWordGenerateRate;	
	bShouldGenerateLetter = true;
	while(bShouldGenerateLetter && characterOutput.size() < iMaxNewLetters)
	{
		bShouldGenerateLetter = SuccessWithPercentRatio(iAppearRatio);
		if (bShouldGenerateLetter)
		{
			if (GenerateLetterFromSubWords(sLetter))
			{
				characterOutput.push_back(sLetter);
				iAppearRatio *= m_WordGenerateConfig.m_iRatioBetweenLettersOfSubWord / 100;
			}
			else
				bShouldGenerateLetter = false;
		}
	}

	// generate letters from trash words
	iAppearRatio = m_iTrashWordGenerateRate;	
	bShouldGenerateLetter = true;
	while(bShouldGenerateLetter && characterOutput.size() < iMaxNewLetters)
	{
		bShouldGenerateLetter = SuccessWithPercentRatio(iAppearRatio);
		if (bShouldGenerateLetter)
		{
			if (GenerateLetterFromTrashCollection(sLetter))
			{
				characterOutput.push_back(sLetter);
				iAppearRatio *= m_WordGenerateConfig.m_iRatioBetweenLettersOfTrashWord/ 100;
			}
			else
				bShouldGenerateLetter = false;
		}
	}

	// now, place the letter to gems
	if (characterOutput.size() > 0)
	{
		int indexList[_BOARD_MAX_ROW_NUMBER_ * _BOARD_MAX_COLUMN_NUMBER_];
		int iIndexListLength = iGemCount;
		for(int i=0; i< iGemCount; i++)
			indexList[i] = i;
		
		int iRandomIndex;
		for(auto sLetter: characterOutput)
		{
			iRandomIndex = rand() % iIndexListLength;
			outputLettersForGems[indexList[iRandomIndex]] = sLetter;

			indexList[iRandomIndex] = indexList[iIndexListLength-1];
			iIndexListLength--;
		}
	}

	// update letter count
	m_iCountOfLettersOnBoard += characterOutput.size();		

	if (bIsNewMove)
	{
		m_iMainWordGenerateRate = MIN( m_iMainWordGenerateRate + m_WordGenerateConfig.m_iIncreasePercentAfterEachMoveOfMainLetter, m_WordGenerateConfig.m_iMaximumRate);			
	}
}

bool GameWordManager::GenerateNewLetter(unsigned char& sOuputLetter, const GemComboType_e& eComboType)
{
	bool bIsLetterFromMainWord = false, bIsLetterFromSubWord = false;
	bool bIsCompleted = false; 
	sOuputLetter = 255;

	if (eComboType == GemComboType_e::_GCT_COMBO5_ || eComboType == GemComboType_e::_GCT_COMBO6_)
	{
		// priopritized for main words then bonus, then trash words
		bIsCompleted = GenerateLetterFromMainWord( sOuputLetter);
		bIsLetterFromMainWord = bIsCompleted;
		
		if (!bIsCompleted)
		{
			bIsCompleted = GenerateLetterFromSubWords( sOuputLetter);			
			bIsLetterFromSubWord = bIsCompleted;
		}

		if (!bIsCompleted)
			bIsCompleted = GenerateLetterFromTrashCollection( sOuputLetter);			
		
		// note: if all letter are appeared but trash collection is empty???
	}
	else
	{
		bool bShouldGenerateLetter = false;		

		srand(time(NULL));

		// generate letters from main word		
		bShouldGenerateLetter = SuccessWithPercentRatio(m_iMainWordGenerateRate);
		if (bShouldGenerateLetter)
		{
			bIsCompleted = GenerateLetterFromMainWord(sOuputLetter);		
			bIsLetterFromMainWord = bIsCompleted;
		}				
	

		// generate letters from sub words		
		if (!bIsCompleted)		
		{
			bShouldGenerateLetter = SuccessWithPercentRatio(m_iSubWordGenerateRate);
			if (bShouldGenerateLetter || m_TrashLettersCollection.size()==0)
			{
				bIsCompleted = GenerateLetterFromSubWords(sOuputLetter);
				bIsLetterFromSubWord = bIsCompleted;

				AddLetterToTrashCollection(sOuputLetter);
			}				
		}

		// generate letters from trash words		
		if (!bIsCompleted)		
		{			
			//bShouldGenerateLetter = SuccessWithPercentRatio(m_iTrashWordGenerateRate);
			//if (bShouldGenerateLetter)
			//{
				bIsCompleted = GenerateLetterFromTrashCollection(sOuputLetter);				
			//}
		}		

	}

	if (bIsLetterFromMainWord)
		m_iMainWordGenerateRate = MAX( m_iMainWordGenerateRate-m_WordGenerateConfig.m_iDecreasePercentAfterLetterAppearedOfMainLetter, m_WordGenerateConfig.m_iMinimumRate);

	// update letter count
	if (bIsCompleted)
		m_iCountOfLettersOnBoard ++;		

	return bIsCompleted;
}

void GameWordManager::UpdateParamForNewMove()
{
	m_iMainWordGenerateRate = MIN( m_iMainWordGenerateRate + m_WordGenerateConfig.m_iIncreasePercentAfterEachMoveOfMainLetter, m_WordGenerateConfig.m_iMaximumRate);			
}

bool GameWordManager::GenerateLetterFromMainWord(unsigned char& sLetter)
{
	sLetter = 255;
	if (m_WordList[m_iMainWordIndex].m_iRemainNotAppearedCharacterCount == 0)
		return false;

	for(int i=0; i < m_WordList[m_iMainWordIndex].m_iWordLength; i++)
		if (! m_WordList[m_iMainWordIndex].m_AppearedCharacterFlags[i])
		{
			m_WordList[m_iMainWordIndex].m_AppearedCharacterFlags[i] = true;
			m_WordList[m_iMainWordIndex].m_iRemainNotAppearedCharacterCount--;

			sLetter = m_WordList[m_iMainWordIndex].m_sWord[i];
			return true;
		}
	
	return false;
}

bool GameWordManager::GenerateLetterFromSubWords(unsigned char& sLetter)
{
	sLetter = 255;
	if (m_SubWordLettersCollection.size() == 0)
		return false;

	int iRandomIndex = rand() % m_SubWordLettersCollection.size();
	sLetter = m_SubWordLettersCollection[iRandomIndex];
	
	//m_SubWordLettersCollection.erase( m_SubWordLettersCollection.begin() + iRandomIndex);	
	m_SubWordLettersCollection[iRandomIndex] = m_SubWordLettersCollection[m_SubWordLettersCollection.size()-1];
	m_SubWordLettersCollection.pop_back();

	return true;
}

bool GameWordManager::GenerateLetterFromTrashCollection(unsigned char& sLetter)
{
	sLetter = 255;
	if (m_TrashLettersCollection.size() == 0)
		return false;

	int iRandomIndex = rand() % m_TrashLettersCollection.size();
	sLetter = m_TrashLettersCollection[iRandomIndex]; // note remove letter from trash, only add to it	

	return true;
}

void GameWordManager::AddLetterToTrashCollection(const unsigned char sLetter)
{
	m_TrashLettersCollection.push_back(sLetter);
}