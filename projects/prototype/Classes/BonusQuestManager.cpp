#include "BonusQuestManager.h"
#include "NewGameBoardManager.h"
#include "cocos2d.h"

BonusQuestManager::BonusQuestManager()
{
}

void BonusQuestManager::InitLevel()
{
	memset( m_IsBonusEnabledQuestFlags, 0, sizeof(m_IsBonusEnabledQuestFlags));
	memset( m_IsBonusQuestActivatedFlags, 0, sizeof(m_IsBonusQuestActivatedFlags));
	memset( m_IsBonusQuestDirtyFlags, 0, sizeof(m_IsBonusQuestDirtyFlags));	
	memset( m_IsBonusQuestCompletedFlags, 0, sizeof(m_IsBonusQuestCompletedFlags));	
	

	m_pLevelConfig = &m_pGameBoardManager->GetLevelConfig();
	for(int i=0; i < _BQT_TYPE_COUNT_; i++)
	{
		m_IsBonusEnabledQuestFlags[i] = m_pLevelConfig->m_BonusQuestConfig.m_IsBonusEnabledQuestFlags[i];
		if (m_IsBonusEnabledQuestFlags[i])
		{
			switch (i)
			{
				case _BQT_COLLECT_GEM_:
				{
					m_CollectGemObjective = m_pLevelConfig->m_BonusQuestConfig.m_CollectGemQuest;
					memset(m_CollectGemParam.m_CountPerGemType, 0, sizeof( m_CollectGemObjective.m_CountPerGemType));
					break;
				}
				case _BQT_COLLECT_COMBO_:
				{
					m_CollectComboObjective = m_pLevelConfig->m_BonusQuestConfig.m_CollectComboQuest;
					memset(m_CollectComboParam.m_CountPerComboGem, 0, sizeof( m_CollectComboObjective.m_CountPerComboGem));
					break;
				}
				case _BQT_COLLECT_BONUS_WORD_:
				{
					m_CollectBonusWordObjective.Set(m_pLevelConfig->m_BonusQuestConfig.m_CollectBonusWordQuest);
					m_CollectBonusWordParam.m_iRemainLettersCount = m_CollectBonusWordObjective.m_iRequiredLetterCount;
					
					//random bonus word
					m_CollectBonusWordObjective.m_iSelectedBonusWordID = m_pLevelConfig->m_BonusQuestConfig.m_CollectBonusWordQuest.m_BonusWordIDList[rand() % m_CollectBonusWordObjective.m_iBonusWordCount];
					break;
				}
			}
		}
	}
}

void BonusQuestManager::ActivateBonusQuest()
{
	for(int i=0; i < _BQT_TYPE_COUNT_; i++)
		if (m_IsBonusEnabledQuestFlags[i] && !m_IsBonusQuestActivatedFlags[i] && !m_IsBonusQuestDirtyFlags[i])
		{
			m_IsBonusQuestDirtyFlags[i] = true;
			//m_IsBonusQuestActivatedFlags[i].
			break;
		}
}

void BonusQuestManager::IncreaseComboCellCountForBonusQuest(const GemComboType_e& eGemComboType)
{
	if (m_IsBonusQuestActivatedFlags[_BQT_COLLECT_COMBO_] && !m_IsBonusQuestCompletedFlags[_BQT_COLLECT_COMBO_])
	{
		m_CollectComboParam.m_CountPerComboGem[(int)eGemComboType-1]++;

		bool bIsQuestCompleted = true;
		for(int i=0; i< _GCT_SINGLE_COMBO_COUNT_; i++)
		{
			if (m_CollectComboObjective.m_CountPerComboGem[i] > m_CollectComboParam.m_CountPerComboGem[i])
			{
				bIsQuestCompleted = false;
				break;
			}
		}

		if (bIsQuestCompleted)
		{
			m_IsBonusQuestCompletedFlags[_BQT_COLLECT_COMBO_] = true;
			m_IsBonusQuestDirtyFlags[_BQT_COLLECT_COMBO_] = true;
		}
	}
}

void BonusQuestManager::IncreaseBasicCellCountForBonusQuest(const int& iGemID)
{
	if (m_IsBonusQuestActivatedFlags[_BQT_COLLECT_GEM_ ] && !m_IsBonusQuestCompletedFlags[_BQT_COLLECT_GEM_])
	{
		m_CollectGemParam.m_CountPerGemType[iGemID]++;		

		bool bIsQuestCompleted = true;
		for(int i=0; i< _MAX_GEM_ID_; i++)
		{
			if (m_CollectGemObjective.m_CountPerGemType[i] > m_CollectGemParam.m_CountPerGemType[i])
			{
				bIsQuestCompleted = false;
				break;
			}
		}

		if (bIsQuestCompleted)
		{
			m_IsBonusQuestCompletedFlags[_BQT_COLLECT_GEM_] = true;
			m_IsBonusQuestDirtyFlags[_BQT_COLLECT_GEM_] = true;
		}
	}
}

void BonusQuestManager::IncreaseCollectedLetterOfBonusWord()
{
	if (m_IsBonusQuestActivatedFlags[_BQT_COLLECT_BONUS_WORD_ ] && !m_IsBonusQuestCompletedFlags[_BQT_COLLECT_BONUS_WORD_])
	{
		m_CollectBonusWordParam.m_iRemainLettersCount--;
		if (m_CollectBonusWordParam.m_iRemainLettersCount == 0)
		{
			m_IsBonusQuestCompletedFlags[_BQT_COLLECT_BONUS_WORD_]=  true;
			m_IsBonusQuestDirtyFlags[_BQT_COLLECT_BONUS_WORD_] = true;
		}
	}
}

void BonusQuestManager::GetLettersOfCollectBonusWordQuest(std::vector<char>& letterList, const int& iMaxLetterCount)
{
	auto&  bonusWord = GameWordManager::getInstance()->GetWord(m_CollectBonusWordObjective.m_iSelectedBonusWordID);

	char sBuffer[_GDS_WORD_MAX_LENGTH_];
	int iBufferLenght = 0;
	int i, iLetterIndex;

	for(i=0; i < bonusWord.m_iWordLength; i++)
		if (bonusWord.m_sWord[i] != ' ')
		{
			sBuffer[iBufferLenght] = bonusWord.m_sWord[i];
			iBufferLenght++;
		}
	
	if (m_CollectBonusWordParam.m_iRemainLettersCount > iBufferLenght)
		m_CollectBonusWordParam.m_iRemainLettersCount = iBufferLenght;
	if (m_CollectBonusWordParam.m_iRemainLettersCount > iMaxLetterCount)
		m_CollectBonusWordParam.m_iRemainLettersCount = iMaxLetterCount;

	for(i=0; i < m_CollectBonusWordParam.m_iRemainLettersCount; i++)
	{
		iLetterIndex = rand() % iBufferLenght;
		letterList.push_back( sBuffer[iLetterIndex]);
		
		sBuffer[iLetterIndex] = sBuffer[iBufferLenght-1];
		iBufferLenght--;
	}
}