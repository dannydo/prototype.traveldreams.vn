#include "SoundManager.h"
#include "GameWordManager.h"

SoundManager SoundManager::m_InternalSoundManager;

SoundManager::StateBackGroundMusic SoundManager::m_stateBackGroundMusic = SoundManager::StateBackGroundMusic::kNone;
// TODO: preload sound music and effects

void SoundManager::PreloadSoundResource()
{
	//CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("SoundEffect/MusicTheme.ogg");
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("SoundEffectPC/OMW_BG_ingame.wav");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("SoundEffectPC/OMW_BG_Bonus_time.wav");
#else
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("SoundEffect/OMW_BG_ingame.ogg");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("SoundEffect/OMW_BG_Bonus_time.ogg");
#endif	
}

void SoundManager::PlayBackgroundMusic(StateBackGroundMusic stateBackGroundMusic)
{	
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnMusic", 1) == 1)
	{
		bool isPlayMusic = false;
		if (stateBackGroundMusic != m_stateBackGroundMusic)
		{
			isPlayMusic = true;
		}

		if(stateBackGroundMusic == StateBackGroundMusic::kNone)
		{
			stateBackGroundMusic = m_stateBackGroundMusic;
		}

		if (stateBackGroundMusic == StateBackGroundMusic::kGameMusic && isPlayMusic)
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffectPC/OMW_BG_ingame.wav", true);
			#else
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffect/OMW_BG_ingame.ogg", true);
			#endif
		}
		else if(stateBackGroundMusic == StateBackGroundMusic::kIntroMusic && isPlayMusic)
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffectPC/OMW_Main_menu._Chapter_Level_screen.wav", true);
			#else
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffect/OMW_Main_menu._Chapter_Level_screen.ogg", true);
			#endif
		}
		else if (stateBackGroundMusic == StateBackGroundMusic::kEndGameBonus && isPlayMusic)
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffectPC/OMW_BG_Bonus_time.wav", true);
			#else
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffect/OMW_BG_Bonus_time.ogg", true);
			#endif
		}
	}

	if(stateBackGroundMusic != StateBackGroundMusic::kNone)
	{
		m_stateBackGroundMusic = stateBackGroundMusic;
	}
}

void SoundManager::StopBackgroundMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}
	

void SoundManager::PlaySoundEffect(const SoundEffectType& eSoundEffectType, float fDelayTime)
{	
	char* sFileName;
	switch(eSoundEffectType)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		
		default:
		case _SET_DROP_:
			sFileName = "SoundEffectPC/Drop_sfx.wav";
			break;
		case _SET_DRAG_FAIL_:
			sFileName = "SoundEffectPC/Drag_fail_sfx.wav";
			break;
		case _SET_DRAG_SUCCESS_:
			sFileName = "SoundEffectPC/Drag_success_sfx.wav";
			break;	
		case _SET_BASIC_MATCH_3_:
			sFileName = "SoundEffectPC/Basic_match_3_sfx.wav";
			break;	
		case _SET_DESTROY_LETTER_:
			sFileName = "SoundEffectPC/One_letter_destroy_sfx.wav";
			break;	
		case _SET_UNLOCK_LETTER_:
			sFileName = "SoundEffectPC/Question_to_Letter_sfx.wav";
			break;	
		case _SET_FILL_LETTER_:
			sFileName = "SoundEffectPC/Letter_filled_sfx.wav";
			break;	
		case _SET_COMPLETE_WORD_:
			sFileName = "SoundEffectPC/Completed_word_sfx.wav";
			break;	
		case _SET_BUTTON_PRESS_:
			sFileName = "SoundEffectPC/Button_press_sfx.wav";
			break;	
		case _SET_CHAPTER_SELECT_:
			sFileName = "SoundEffectPC/Chapter_select_sfx.wav";
			break;	
		case _SET_LEVEL_SELECT_:
			sFileName = "SoundEffectPC/Level_select_sfx.wav";
			break;						

		case _SET_START_LEVEL_:
			sFileName = "SoundEffectPC/Start_level_sfx.wav";
			break;

		case _SET_WIN_:
			sFileName = "SoundEffectPC/Win_sfx.wav";
			break;
		case _SET_FAIL_:
			sFileName = "SoundEffectPC/Fail_sfx.wav";
			break;	
		
		case _SET_CREATE_COMBO_4_:
			sFileName = "SoundEffectPC/Create_combo_4_sfx.wav";
			break;	
		case _SET_GAME_MENU_:
			sFileName = "SoundEffectPC/IGM_sfx.wav";
			break;	
			
		case _SET_ACTIVATE_COMBO_4_:
			sFileName = "SoundEffectPC/Combo4_effect_sfx.wav";
			break;	
		case _SET_CREATE_COMBO_5_:
			sFileName = "SoundEffectPC/Create_combo_5_sfx.wav";
			break;	
		case _SET_ACTIVATE_COMBO_5_:
			sFileName = "SoundEffectPC/Combo5_effect_sfx.wav";
			break;	
		case _SET_CREATE_COMBO_6_:
			sFileName = "SoundEffectPC/Create_combo_6_sfx.wav";
			break;	
		case _SET_ACTIVATE_COMBO_6_:
			sFileName = "SoundEffectPC/Combo6_effect_sfx.wav";
			break;	
		case _SET_ACTIVATE_DOUBLE_COMBO_:
			sFileName = "SoundEffectPC/Active_double_combo_sfx .wav";
			break;	
		case _SET_ACTIVATE_TRIPLE_COMBO_:
			sFileName = "SoundEffectPC/Active_triple_combo_sfx.wav";
			break;	

		case _SET_POPUP_:
			sFileName = "SoundEffectPC/Popup_sfx.wav";
			break;	

		case _SET_LETS_FIND_:
			sFileName = "SoundEffectPC/Encouragement/LetsFind.wav";
			break;		

		case _SET_FLASHCARD_CHOOSE_RIGHT_:
			sFileName = "SoundEffectPC/Sfx_flashcard_right.wav";
			break;		
		case _SET_FLASHCARD_CHOOSE_WRONG_:
			sFileName = "SoundEffectPC/Sfx_flashcard_wrong.wav";
			break;		

		case _SET_ENCOURAGE_NICE_:
			sFileName = "SoundEffectPC/Encouragement/Nice.wav";
			break;		
		case _SET_ENCOURAGE_LOVELY_:
			sFileName = "SoundEffectPC/Encouragement/Lovely.wav";
			break;	
		case _SET_ENCOURAGE_BEAUTIFUL_:
			sFileName = "SoundEffectPC/Encouragement/Beautiful.wav";
			break;			
		case _SET_ENCOURAGE_BRILLIANT_:
			sFileName = "SoundEffectPC/Encouragement/Brilliant.wav";
			break;	
		case _SET_ENCOURAGE_ARTISTIC_:
			sFileName = "SoundEffectPC/Encouragement/Artistic.wav";
			break;
	
#else	
		default:
		case _SET_DROP_:
			sFileName = "SoundEffect/Drop_sfx.ogg";
			break;
		case _SET_DRAG_FAIL_:
			sFileName = "SoundEffect/Drag_fail_sfx.ogg";
			break;
		case _SET_DRAG_SUCCESS_:
			sFileName = "SoundEffect/Drag_success_sfx.ogg";
			break;	
		case _SET_BASIC_MATCH_3_:
			sFileName = "SoundEffect/Basic_match_3_sfx.ogg";
			break;	
		case _SET_DESTROY_LETTER_:
			sFileName = "SoundEffect/One_letter_destroy_sfx.ogg";
			break;	
		case _SET_UNLOCK_LETTER_:
			sFileName = "SoundEffect/Question_to_Letter_sfx.ogg";
			break;	
		case _SET_FILL_LETTER_:
			sFileName = "SoundEffect/Letter_filled_sfx.ogg";
			break;	
		case _SET_COMPLETE_WORD_:
			sFileName = "SoundEffect/Completed_word_sfx.ogg";
			break;	
		case _SET_BUTTON_PRESS_:
			sFileName = "SoundEffect/Button_press_sfx.ogg";
			break;	
		case _SET_CHAPTER_SELECT_:
			sFileName = "SoundEffect/Chapter_select_sfx.ogg";
			break;	
		case _SET_LEVEL_SELECT_:
			sFileName = "SoundEffect/Level_select_sfx.ogg";
			break;						

		case _SET_START_LEVEL_:
			sFileName = "SoundEffect/Start_level_sfx.ogg";
			break;

		case _SET_WIN_:
			sFileName = "SoundEffect/Win_sfx.ogg";
			break;
		case _SET_FAIL_:
			sFileName = "SoundEffect/Fail_sfx.ogg";
			break;	

		case _SET_CREATE_COMBO_4_:
			sFileName = "SoundEffect/Create_combo_4_sfx.ogg";
			break;
		case _SET_GAME_MENU_:
			sFileName = "SoundEffect/IGM_sfx.ogg";
			break;
		case _SET_ACTIVATE_COMBO_4_:
			sFileName = "SoundEffect/Combo4_effect_sfx.ogg";
			break;	
		case _SET_CREATE_COMBO_5_:
			sFileName = "SoundEffect/Create_combo_5_sfx.ogg";
			break;	
		case _SET_ACTIVATE_COMBO_5_:
			sFileName = "SoundEffect/Combo5_effect_sfx.ogg";
			break;	
		case _SET_CREATE_COMBO_6_:
			sFileName = "SoundEffect/Create_combo_6_sfx.ogg";
			break;	
		case _SET_ACTIVATE_COMBO_6_:
			sFileName = "SoundEffect/Combo6_effect_sfx.ogg";
			break;	
		case _SET_ACTIVATE_DOUBLE_COMBO_:
			sFileName = "SoundEffect/Active_double_combo_sfx .ogg";
			break;	
		case _SET_ACTIVATE_TRIPLE_COMBO_:
			sFileName = "SoundEffectPActive_triple_combo_sfx.ogg";
			break;	

		case _SET_POPUP_:
			sFileName = "SoundEffect/Popup_sfx.ogg";
			break;	

		case _SET_LETS_FIND_:
			sFileName = "SoundEffect/Encouragement/LetsFind.ogg";
			break;		

		case _SET_FLASHCARD_CHOOSE_RIGHT_:
			sFileName = "SoundEffectPC/Sfx_flashcard_right.ogg";
			break;		
		case _SET_FLASHCARD_CHOOSE_WRONG_:
			sFileName = "SoundEffectPC/Sfx_flashcard_wrong.ogg";
			break;		


		case _SET_ENCOURAGE_NICE_:
			sFileName = "SoundEffect/Encouragement/Nice.ogg";
			break;		
		case _SET_ENCOURAGE_LOVELY_:
			sFileName = "SoundEffect/Encouragement/Lovely.ogg";
			break;	
		case _SET_ENCOURAGE_BEAUTIFUL_:
			sFileName = "SoundEffect/Encouragement/Beautiful.ogg";
			break;			
		case _SET_ENCOURAGE_BRILLIANT_:
			sFileName = "SoundEffect/Encouragement/Brilliant.ogg";
			break;	
		case _SET_ENCOURAGE_ARTISTIC_:
			sFileName = "SoundEffect/Encouragement/Artistic.ogg";
			break;
#endif
	};

	if ( fDelayTime == 0)
	{
		m_InternalSoundManager.PlaySoundEffectUtil(NULL, sFileName);
	}
	else
		Director::getInstance()->getRunningScene()->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				CCCallFuncND::create( &m_InternalSoundManager, callfuncND_selector(SoundManager::PlaySoundEffectUtil), sFileName),
				NULL));

	//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(sFileName);
}

void SoundManager::PlaySoundEffectUtil(Node* pNode, void* data)
{
	// check play voice on setting
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnEffect", 1) == 1)
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((const char*)data);
	}
}


float SoundManager::PlaySpellingOfWord(Node* pPlayingNode, const Word& word, float fDelayTime, bool bPlayMeaningFile)
{
	char sSoundFile[150];
	

	// check play voice on setting
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnVoice", 1) == 1)
	{
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		sprintf(sSoundFile, "%s/EN_SoundPC/Words/%s.wav", GameWordManager::getInstance()->GetPackagePathFromWord(word).c_str(), word.m_sSoundEnglishFile.c_str());
		#else
			sprintf(sSoundFile, "%s/EN_Sound/Words/%s.ogg", GameWordManager::getInstance()->GetPackagePathFromWord(word).c_str(), word.m_sSoundEnglishFile.c_str());
		#endif

		if (fDelayTime == 0)
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( sSoundFile);	
		else
		{
			auto playEnglishSound = std::bind(&SoundManager::PlayVietnameseSpelling, m_InternalSoundManager, std::string(sSoundFile));

			pPlayingNode->runAction(
			Sequence::create( 
				DelayTime::create(fDelayTime),
				CallFunc::create(playEnglishSound),
				NULL));
		}

		if (bPlayMeaningFile)
		{
			// play vietnamese sound file
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				sprintf(sSoundFile, "%s/EN_SoundPC/Words/%s.wav", GameWordManager::getInstance()->GetPackagePathFromWord(word).c_str(), word.m_sSoundVietnameseFile.c_str());
			#else
				sprintf(sSoundFile, "%s/EN_Sound/Words/%s.ogg", GameWordManager::getInstance()->GetPackagePathFromWord(word).c_str(), word.m_sSoundVietnameseFile.c_str());
			#endif

			std::string sSoundVietnameseFile = sSoundFile;

			auto playVietnameseSound = std::bind(&SoundManager::PlayVietnameseSpelling, m_InternalSoundManager, sSoundVietnameseFile);

			pPlayingNode->runAction(
				Sequence::create( 
					DelayTime::create(word.m_fSoundEnglishLength),
					CallFunc::create(playVietnameseSound),
					NULL));

			return word.m_fSoundEnglishLength + word.m_fSoundVietnameseLength;
		}
	}

	return 0;
}

float SoundManager::PlaySentenceOfWord(Node* pPlayingNode, const Word& word)
{
	char sSoundFile[150];
	

	// check play voice on setting
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnVoice", 1) == 1)
	{
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		sprintf(sSoundFile, "%s/EN_SoundPC/Sentences/%s.wav", GameWordManager::getInstance()->GetPackagePathFromWord(word).c_str(), word.m_sSentenceSoundFile.c_str());
		#else
			sprintf(sSoundFile, "%s/EN_Sound/Sentences/%s.ogg", GameWordManager::getInstance()->GetPackagePathFromWord(word).c_str(), word.m_sSentenceSoundFile.c_str());
		#endif

		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( sSoundFile);	

		pPlayingNode->runAction(
			Sequence::create( 
				DelayTime::create(word.m_fSentenceSoundLength),
				NULL));

		return word.m_fSentenceSoundLength;
	}

	return 0;
}


void SoundManager::PlayVietnameseSpelling(std::string sSoundVietnameseFile)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
		sSoundVietnameseFile.c_str());
}

