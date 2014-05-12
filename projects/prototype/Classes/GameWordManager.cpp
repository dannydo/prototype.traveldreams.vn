#include "GameWordManager.h"
#include "cocos2d.h"
#include "GameBoardManager.h"
#include "GameConfigManager.h"
#include "Database\LevelTable.h"
#include "Database\CSWordTable.h"

using namespace cocos2d;

GameWordManager* GameWordManager::m_pInstance = NULL;

GameWordManager::GameWordManager()
{
	m_iCountOfLettersOnBoard = 0;
	m_iTotalCollectibleLettersOfMainWord = 0;	
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
	/*
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
	std::getline(inputStream, sTemp);	*/

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


	delete[] data;
	delete[] orginalData;
}

void GameWordManager::LoadWorldListConfig()
{
	unsigned long iDataSize;
	std::string fullPath = CCFileUtils::getInstance()->fullPathForFilename("GameData/Words/WordListConfig.data");
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData(fullPath.c_str(), "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	int iPackageCount;
	inputStream >> iPackageCount;
	
	for(int i=0; i< iPackageCount; i++)
	{		
		WorldListPackgeDescription package;
		inputStream >> package.m_sPackgageID;
		inputStream >> package.m_sPackagePath;
		
		m_MapPackgeNameToIndex[package.m_sPackgageID] = i;
		m_WorldPackageList.push_back(package);
	}
}

void GameWordManager::ClearCache()
{
	m_WordList.clear();
	m_MapWordIDToLoadedIndex.clear();
}

void GameWordManager::PreLoadPackageForWord(std::string sWordID)
{	
	// parse packageID from wordID
	auto iIndexOfSpecalLetter = sWordID.find_first_of('-');
	auto sPackageID = sWordID.substr(0, iIndexOfSpecalLetter);
	int iPackageIndex = m_MapPackgeNameToIndex[sPackageID];		
	LoadWords( iPackageIndex);
}

const std::string& GameWordManager::GetPackagePathFromWordID(const std::string& sWordID)
{
	int iWordIndex = GetLoadedWordIndexFromID(sWordID);
	return m_WorldPackageList[m_WordList[iWordIndex].m_iPackageIndex ].m_sPackagePath;
}

const std::string& GameWordManager::GetPackagePathFromWord(const Word& word)
{
	return m_WorldPackageList[word.m_iPackageIndex].m_sPackagePath;
}

const std::string GameWordManager::GetWordIdFromWord(const Word& word)
{
	return word.m_sWordID;
	/*
	std::string sWordId = m_WorldPackageList[word.m_iPackageIndex].m_sPackgageID;
	sWordId.append("-");
	sWordId.append(word.m_sWord);
	return sWordId;*/
}

int GameWordManager::AddAndLoadCustomPackageToList(const std::string& sPackageID, const std::string& sPackagePath)
{
	ClearCache();
	

	if (m_MapPackgeNameToIndex.find(sPackageID) == m_MapPackgeNameToIndex.end()) //this package is not added to list yet
	{
		WorldListPackgeDescription package;
		package.m_sPackgageID = sPackageID;
		package.m_sPackagePath = sPackagePath;
		
		m_MapPackgeNameToIndex[package.m_sPackgageID] = m_WorldPackageList.size();		

		m_WorldPackageList.push_back(package);
	}

	int iPackageIndex = m_MapPackgeNameToIndex[sPackageID];
	LoadWords(iPackageIndex);

	return iPackageIndex;
}

void GameWordManager::LoadWords(const int& iPackageIndex)
{

	std::string sPackagePath = m_WorldPackageList[iPackageIndex].m_sPackagePath;
	sPackagePath.append("/WordList.txt");	

	unsigned long iDataSize;	
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData(sPackagePath.c_str(), "r", &iDataSize);
	

	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	char sWordID[60];	

	int iWordCount;
	inputStream >> iWordCount;
	std::string sTemp;
	for(int iWordIndex = 0; iWordIndex < iWordCount; iWordIndex++)
	{
		Word newWord;
		newWord.m_iPackageIndex = iPackageIndex;

		std::getline(inputStream, sTemp);
		std::getline(inputStream, sTemp);		

		// get word id first		
		sprintf( sWordID, "%s-%s", m_WorldPackageList[iPackageIndex].m_sPackgageID.c_str(), sTemp.substr( 0, sTemp.size()-1).c_str()) ;
		newWord.m_sWordID = sWordID;
		m_MapWordIDToLoadedIndex[sWordID] = iWordIndex;
				

		// get word content
		std::getline(inputStream, sTemp);
		strcpy( newWord.m_sWord, sTemp.data());		
		newWord.m_iWordLength = sTemp.size()-1;
		newWord.m_sWord[newWord.m_iWordLength] = 0;
				

		//inputStream >> m_WordList[iWordIndex].m_sWord;		
		//m_WordList[iWordIndex].m_iWordLength = strlen( m_WordList[iWordIndex].m_sWord);

		// read \r to temp
		//std::getline(inputStream, sTemp);		
		std::getline(inputStream, newWord.m_sMeaning);
		//strcpy( m_WordList[iWordIndex].m_sMeaning, sTemp.c_str());

		inputStream >> newWord.m_sSoundEnglishFile;		
		inputStream >> newWord.m_fSoundEnglishLength;

		inputStream >> newWord.m_sSoundVietnameseFile;
		inputStream >> newWord.m_fSoundVietnameseLength;

		//inputStream >> m_WordList[iWordIndex].m_sSoundVietnameseFile;		
		//std::getline(inputStream, m_WordList[iWordIndex].m_sSoundFile);
		//std::getline(inputStream, m_WordList[iWordIndex].m_sSoundVietnameseFile);
		
		inputStream >> newWord.m_sFlashCardImage;
		

		std::getline(inputStream, sTemp);
		std::getline(inputStream, newWord.m_sPhonetic);
		std::getline(inputStream, newWord.m_sSentence);
		inputStream >> newWord.m_sSentenceSoundFile;
		inputStream >> newWord.m_fSentenceSoundLength;

		inputStream >> newWord.m_iLimitLetterCountOfFirstLine;
		inputStream >> newWord.m_iMinimumLevelLetterRequired;
		inputStream >> newWord.m_iMaximumLevelLetterRequired;		

		m_WordList.push_back(newWord);
	}

	delete[] data;
	delete[] orginalData;
}

int GameWordManager::GetLoadedWordIndexFromID(const std::string& sWordID)
{
	if ( m_MapWordIDToLoadedIndex.find(sWordID) == m_MapWordIDToLoadedIndex.end()) //this word not loaded yet
	{
		PreLoadPackageForWord(sWordID);

		if ( m_MapWordIDToLoadedIndex.find(sWordID) == m_MapWordIDToLoadedIndex.end())
			return -1;
		else
			return m_MapWordIDToLoadedIndex[sWordID];
	}
	else{
		return m_MapWordIDToLoadedIndex[sWordID];
	}

	/*int iWordCount = m_WordList.size();
	for(int iWordIndex=0; iWordIndex < iWordCount; iWordIndex++)
	{
		if (m_WordList[iWordIndex].m_sWord == sWord)
			return iWordIndex;
	}
	// error, word not found
	assert(0);

	return 0;*/
}

void GameWordManager::GenerateWordForNewLevel(std::string sChapterID, int iLevel)
{
//	std::string sCurrentChapterID = GameConfigManager::getInstance()->GetCurrentChapterID();
//	int iCurrentLevel = GameConfigManager::getInstance()->GetCurrentLevelId();	 

	LevelInfo levelInfo = LevelTable::getInstance()->getLevel( sChapterID, iLevel);	

	m_pLevelConfig = &GameConfigManager::getInstance()->GetLevelConfig(sChapterID, iLevel);
	m_iMainWordIndex = GetLoadedWordIndexFromID(levelInfo.sWordId); //m_pLevelConfig->m_sMainWordID);
	

	if (!m_pLevelConfig->m_bIsMainWordExistedOnBoard)		
		m_bAddDirectLetterOfMainWordToTrash = true;	

	// begin hard code level 1
	/*m_bAddDirectLetterOfMainWordToTrash = false;
	m_iSubWordCount = _GDS_SUB_WORD_MAX_COUNT_;

	int iPreviousWordIndex = UserDefault::getInstance()->getIntegerForKey("wordIndex", -1);
	m_iMainWordIndex = (iPreviousWordIndex + 1) % m_iWordCount;

	if(iLevel == 1)
	{
		m_bAddDirectLetterOfMainWordToTrash = true;
		m_iSubWordCount = 0;
		m_iMainWordIndex = 5;		
	}
	else if(iLevel == 2)
	{		
		m_iMainWordIndex = 7;		
	}
	// end hard code level 1

	UserDefault::getInstance()->setIntegerForKey("wordIndex", m_iMainWordIndex);

	// generate sub words from remain words
	int wordIndexList[_GDS_WORD_MAX_COUNT_];	
	for(int i=0; i < m_iWordCount; i++)
		wordIndexList[i] = i;

	int iWordRemainCount = m_iWordCount-1;
	wordIndexList[m_iMainWordIndex] = iWordRemainCount;

	srand(time(NULL ));
	int iRandomIndex;
	for(int i=0; i< m_iSubWordCount; i++)
	{
		iRandomIndex = rand() % iWordRemainCount;
		m_SubWordList[i] = wordIndexList[iRandomIndex];
		wordIndexList[iRandomIndex] = wordIndexList[iWordRemainCount-1];
		iWordRemainCount--;

		// log for test
		CCLOG( "Bonus word %d: %s", i, m_WordList[m_SubWordList[i]].m_sWord);
	}	
	*/
	// reset data
	ResetDataForNewPlay();
}

long GameWordManager::GetTotalPlayTimeOfTimeModeSession()
{
	timeval now;
	gettimeofday(&now, NULL);
	unsigned long iCurrentTime = now.tv_sec * 1000 + now.tv_usec/1000 ; //miliseconds
	return (iCurrentTime - m_iStartTimeOfTimeModeGameSession)/1000;
}

void GameWordManager::GenerateWordForNewLevelOfTimeMode(TimeModeLevelConfig* pTimeModeConfig, bool bStartNewTimeModeSession)
{
	m_pLevelConfig = NULL; //not have normal level config


	if (bStartNewTimeModeSession)
	{	
		m_TimeModeSessionTracking.clear();

		timeval now;
		gettimeofday(&now, NULL);
		m_iStartTimeOfTimeModeGameSession = now.tv_sec * 1000 + now.tv_usec/1000 ; //miliseconds

		auto wordDBList = CSWordTable::getInstance()->getAllCSWordsForPackage(pTimeModeConfig->m_sCustomPackageID);
		auto currentWordIndexList = pTimeModeConfig->m_WordIndexList;
	
		int iLoadedWordIndex, i, iCurrentWordListSize = currentWordIndexList.size();
		for(auto wordDB: wordDBList)
		{
			iLoadedWordIndex = GetLoadedWordIndexFromID( wordDB.sCSWordId);
			if (iLoadedWordIndex >= 0)
			{
				for(i=0; i< iCurrentWordListSize; i++)
				{
					if ( iLoadedWordIndex == currentWordIndexList[i])
						pTimeModeConfig->m_WordCollectedCountList[i] = wordDB.iCollectedCount;
				}
			}		
		}
	}

	
	int iSmallestColllectedCountOfWord = 1000, iSmallestCollectedWordCount = 0;

	for(auto& iColllectedCount : pTimeModeConfig->m_WordCollectedCountList)
		if (iSmallestColllectedCountOfWord > iColllectedCount)
		{
			iSmallestColllectedCountOfWord = iColllectedCount;
			iSmallestCollectedWordCount = 1;
		}
		else if (iSmallestColllectedCountOfWord == iColllectedCount)
		{
			iSmallestCollectedWordCount++;
		}


	//CCLOG("Generate 1");

	srand((unsigned int)time(NULL));
	int iRandomIndex;
	if (iSmallestCollectedWordCount > 0)
		iRandomIndex = rand() % iSmallestCollectedWordCount + 1;
	else
		iRandomIndex = rand() % pTimeModeConfig->m_WordCollectedCountList.size();

	for(int i=0; i< pTimeModeConfig->m_WordIndexList.size(); i++)
		if (iSmallestColllectedCountOfWord == pTimeModeConfig->m_WordCollectedCountList[i])
		{
			iRandomIndex--;
			if (iRandomIndex <= 0)
			{
				m_iMainWordIndex = pTimeModeConfig->m_WordIndexList[i];
				break;
			}
		}
	//m_iMainWordIndex = pTimeModeConfig->m_WordIndexList[iRandomIndex];

	//CCLOG("Generate 2");

	m_bAddDirectLetterOfMainWordToTrash = false;

	// reset data
	ResetDataForNewPlay();
}

void GameWordManager::UpdateTimeModeTracking(const int& iCompletedWordIndex)
{	
	int iIndexInTrackingList=  -1;
	for(int i=0; i< m_TimeModeSessionTracking.size(); i++)
	{
		if (m_TimeModeSessionTracking[i].m_iWordIndex == iCompletedWordIndex)
		{
			iIndexInTrackingList = i;
			break;
		}		
	}

	if (iIndexInTrackingList >=0)
	{
		m_TimeModeSessionTracking[iIndexInTrackingList].m_iCollectedCount++;
	}
	else
	{
		TimeModeWordTrackInfo wordTrackInfo;
		wordTrackInfo.m_iCollectedCount = 1;
		wordTrackInfo.m_iWordIndex = iCompletedWordIndex;

		m_TimeModeSessionTracking.push_back(wordTrackInfo);
	}
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

	int tempRemovedIndexList[_GDS_WORD_MAX_LENGTH_];
	int iListCount = 0;

	for(int i=0; i< m_WordList[m_iMainWordIndex].m_iWordLength; i++)
	{
		if (m_WordList[m_iMainWordIndex].m_sWord[i] == ' ')
		{
			m_WordList[m_iMainWordIndex].m_ActivatedCharacterFlags[i] = true;
			m_WordList[m_iMainWordIndex].m_AppearedCharacterFlags[i] = true;
			m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount--;
			m_WordList[m_iMainWordIndex].m_iRemainNotAppearedCharacterCount--;			
		}
		else
		{
			tempRemovedIndexList[iListCount] = i;		
			iListCount++;
		}
	}

	if (m_pLevelConfig != NULL) //normal mode
	{
		// random choose some letters of main word to match with requirement of level
		int iRandomRemovedLetter;
		while(iListCount > m_pLevelConfig->m_iNumberLetterOfMainWord)
		{
			iRandomRemovedLetter = rand() % iListCount;

			m_WordList[m_iMainWordIndex].m_ActivatedCharacterFlags[tempRemovedIndexList[iRandomRemovedLetter]] = true;
			m_WordList[m_iMainWordIndex].m_AppearedCharacterFlags[tempRemovedIndexList[iRandomRemovedLetter]] = true;
			m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount--;
			m_WordList[m_iMainWordIndex].m_iRemainNotAppearedCharacterCount--;

			tempRemovedIndexList[iRandomRemovedLetter] = tempRemovedIndexList[iListCount-1];
			iListCount--;
		}
	}

	m_iTotalCollectibleLettersOfMainWord = m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount;


	// reset counter
	m_iCountOfLettersOnBoard = 0;

	// reset ratio
	if (m_pLevelConfig == NULL || m_pLevelConfig->m_bIsMainWordExistedOnBoard)
		m_iMainWordGenerateRate = 0;
	else
		m_iMainWordGenerateRate = m_pLevelConfig->m_iInitRateOfMainLetter;	
	m_iTrashWordGenerateRate = m_WordGenerateConfig.m_iFixRatioOfTrashWords;

	// reset trash letters collection
	m_TrashLettersCollection.clear();	
}

void GameWordManager::EnableVersusBossMode()
{
	m_iMainWordGenerateRate = 0;
	m_bAddDirectLetterOfMainWordToTrash = false;
}

bool GameWordManager::UnlockLetter(const unsigned char& iLetter, int& iUnlockedLetterIndexOfMainWord,   bool& bIsMainWordJustUnlocked)
{
	bool bNewCharacterIsUnlocked = false;
	bIsMainWordJustUnlocked = false;	

	// check main word
	iUnlockedLetterIndexOfMainWord = -1;
	if ( m_WordList[m_iMainWordIndex].m_iRemainInactivatedCharacterCount > 0)
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
	if (m_pLevelConfig != NULL && !m_pLevelConfig->m_bIsMainWordExistedOnBoard)
		if (iUnlockedLetterIndexOfMainWord >=0)
			m_iMainWordGenerateRate = MAX( m_iMainWordGenerateRate- m_pLevelConfig->m_iDecreasePercentAfterLetterDestroyedOfMainLetter, m_WordGenerateConfig.m_iMinimumRate);	

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

void GameWordManager::GenerateNewLetters(const std::vector<bool>& gemCanContainLetterFlagList,const int& iGemCount, std::vector<GemLetterData>& outputLettersForGems, bool bIsNewMove)
{
	// init outputLettersForGems as empty list
	outputLettersForGems.clear();
	for(int i=0; i< gemCanContainLetterFlagList.size(); i++)
		outputLettersForGems.push_back(GemLetterData());	

	// check maximum possible new letters
	int iMaxNewLetters = MAX( MIN( iGemCount, m_WordGenerateConfig.m_iMaximumLettersAtTheSame - m_iCountOfLettersOnBoard), 0);	
	//if (iMaxNewLetters == 0)
		//return;	

	// generate new letters
	std::vector<GemLetterData> characterOutput;

	bool bShouldGenerateLetter = true;
	unsigned char sLetter;

	srand(time(NULL));

	// generate letters from main word
	int iAppearRatio = m_iMainWordGenerateRate;	
	if (m_pLevelConfig != NULL && !m_pLevelConfig->m_bIsMainWordExistedOnBoard && !m_pLevelConfig->m_bEnableBoss)
	{
		while(bShouldGenerateLetter && characterOutput.size() < iMaxNewLetters)
		{
			bShouldGenerateLetter = SuccessWithPercentRatio(iAppearRatio);
			if (bShouldGenerateLetter)
			{
				if (GenerateLetterFromMainWord(sLetter))
				{
					characterOutput.push_back(GemLetterData( sLetter, true));						
					iAppearRatio *=  m_pLevelConfig->m_iRatioBetweenLettersOfMainWord / 100;				
				}
				else
					bShouldGenerateLetter = false;
			}
		}

		if (characterOutput.size() > 0) //appear letter in main word ==> update ratio for next generation
		{
			m_iMainWordGenerateRate = MAX( m_iMainWordGenerateRate-m_pLevelConfig->m_iDecreasePercentAfterLetterAppearedOfMainLetter, m_WordGenerateConfig.m_iMinimumRate);
		}
	}
	
	// generate letters from trash words
	/*
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
	}*/

	// now, place the letter to gems
	if (characterOutput.size() > 0)
	{
		int indexList[_BOARD_MAX_ROW_NUMBER_ * _BOARD_MAX_COLUMN_NUMBER_];
		int iIndexListLength = iGemCount;
		int iIndex = 0;
		for(int i=0; i< gemCanContainLetterFlagList.size(); i++)
			if (gemCanContainLetterFlagList[i])
			{				
				indexList[iIndex] = i;
				iIndex++;
			}
		
		int iRandomIndex;
		for(auto gemData: characterOutput)
		{
			iRandomIndex = rand() % iIndexListLength;
			outputLettersForGems[indexList[iRandomIndex]] = gemData;

			indexList[iRandomIndex] = indexList[iIndexListLength-1];
			iIndexListLength--;
		}
	}

	// update letter count
	m_iCountOfLettersOnBoard += characterOutput.size();		

	if (bIsNewMove)
		if (m_pLevelConfig != NULL && !m_pLevelConfig->m_bIsMainWordExistedOnBoard)
		{
			m_iMainWordGenerateRate = MIN( m_iMainWordGenerateRate + m_pLevelConfig->m_iIncreasePercentAfterEachMoveOfMainLetter, m_WordGenerateConfig.m_iMaximumRate);			
		}
}

/*bool GameWordManager::GenerateNewLetter(unsigned char& sOuputLetter, const GemComboType_e& eComboType)
{
	bool bIsLetterFromMainWord = false, bIsLetterFromSubWord = false;
	bool bIsCompleted = false; 
	sOuputLetter = 255;

	if (eComboType == GemComboType_e::_GCT_COMBO5_ || eComboType == GemComboType_e::_GCT_COMBO6_)
	{
		// priopritized for main words then bonus, then trash words
		if (!m_pLevelConfig->m_bIsMainWordExistedOnBoard)
		{
			bIsCompleted = GenerateLetterFromMainWord( sOuputLetter);
			bIsLetterFromMainWord = bIsCompleted;
			if (bIsLetterFromMainWord && m_bAddDirectLetterOfMainWordToTrash)
			{
				AddLetterToTrashCollection(sOuputLetter);
			}
		}
		
		if (!bIsCompleted)
		{
			bIsCompleted = GenerateLetterFromSubWords( sOuputLetter);			
			bIsLetterFromSubWord = bIsCompleted;

			if (bIsCompleted)
				AddLetterToTrashCollection(sOuputLetter);
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
		if (!m_pLevelConfig->m_bIsMainWordExistedOnBoard)
		{
			bShouldGenerateLetter = SuccessWithPercentRatio(m_iMainWordGenerateRate);
			if (bShouldGenerateLetter || (m_bAddDirectLetterOfMainWordToTrash && m_TrashLettersCollection.size()==0))
			{
				bIsCompleted = GenerateLetterFromMainWord(sOuputLetter);		
				bIsLetterFromMainWord = bIsCompleted;

				if (bIsLetterFromMainWord && m_bAddDirectLetterOfMainWordToTrash)
				{
					AddLetterToTrashCollection(sOuputLetter);
				}
			}				
		}	

		// generate letters from sub words		
		if (!bIsCompleted)		
		{
			bShouldGenerateLetter = SuccessWithPercentRatio(m_iSubWordGenerateRate);
			if (bShouldGenerateLetter || m_TrashLettersCollection.size()==0)
			{
				bIsCompleted = GenerateLetterFromSubWords(sOuputLetter);
				bIsLetterFromSubWord = bIsCompleted;

				if (bIsCompleted)
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

	// NOTE: hot fix
	if (sOuputLetter == 255 && bIsCompleted)
		bIsCompleted = false;

	if (bIsLetterFromMainWord)
		m_iMainWordGenerateRate = MAX( m_iMainWordGenerateRate-m_WordGenerateConfig.m_iDecreasePercentAfterLetterAppearedOfMainLetter, m_WordGenerateConfig.m_iMinimumRate);

	// update letter count
	if (bIsCompleted)
		m_iCountOfLettersOnBoard ++;		

	return bIsCompleted;
}*/

void GameWordManager::UpdateParamForNewMove()
{
	if (m_pLevelConfig != NULL && !m_pLevelConfig->m_bIsMainWordExistedOnBoard)
		m_iMainWordGenerateRate = MIN( m_iMainWordGenerateRate + m_pLevelConfig->m_iIncreasePercentAfterEachMoveOfMainLetter, m_WordGenerateConfig.m_iMaximumRate);			
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