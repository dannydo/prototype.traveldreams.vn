#include "SoundManager.h"

SoundManager SoundManager::m_InternalSoundManager;

SoundManager::StateBackGroundMusic SoundManager::m_stateBackGroundMusic = SoundManager::StateBackGroundMusic::kNone;
// TODO: preload sound music and effects

void SoundManager::PreloadSoundResource()
{
	//CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("SoundEffect/MusicTheme.ogg");
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("SoundEffectPC/MusicTheme.wav");
#else
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("SoundEffect/MusicTheme.ogg");
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
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffectPC/MusicTheme.wav", true);
			#else
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffect/MusicTheme.ogg", true);
			#endif
		}
		else if(stateBackGroundMusic == StateBackGroundMusic::kIntroMusic && isPlayMusic)
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffectPC/intro.wav", true);
			#else
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("SoundEffect/intro.ogg", true);
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
			sFileName = "SoundEffectPC/Fail_sfx.wav";
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
		//_SET_START_LEVEL_,
		case _SET_WIN_:
			sFileName = "SoundEffectPC/Win_sfx_1.wav";
			break;
		case _SET_FAIL_:
			sFileName = "SoundEffectPC/Fail_sfx.wav";
			break;	
		
		case _SET_SIMPLE_COMBO_:
		case _SET_DOUBLE_COMPLE_EFFECT_:
			sFileName = "SoundEffectPC/combo_sfx.wav";
			break;	
#else	
		default:
		case _SET_DROP_:
			sFileName = "SoundEffect/Drop_sfx.ogg";
			break;
		case _SET_DRAG_FAIL_:
			sFileName = "SoundEffect/Fail_sfx.ogg";
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
		//_SET_START_LEVEL_,
		case _SET_WIN_:
			sFileName = "SoundEffect/Win_sfx_1.ogg";
			break;
		case _SET_FAIL_:
			sFileName = "SoundEffect/Fail_sfx.ogg";
			break;	

		case _SET_SIMPLE_COMBO_:
		case _SET_DOUBLE_COMPLE_EFFECT_:
			sFileName = "SoundEffect/combo_sfx.ogg";
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


float SoundManager::PlaySpellingOfWord(Node* pPlayingNode, const Word& word)
{
	char sSoundFile[64];

	// check play voice on setting
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnVoice", 1) == 1)
	{
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			sprintf(sSoundFile, "EnglishSoundPC/Words/%s.wav", word.m_sSoundEnglishFile.c_str());
		#else
			sprintf(sSoundFile, "EnglishSound/Words/%s.ogg", word.m_sSoundEnglishFile.c_str());
		#endif

		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( sSoundFile);	


		// play vietnamese sound file
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			sprintf(sSoundFile, "EnglishSoundPC/Words/%s.wav", word.m_sSoundVietnameseFile.c_str());
		#else
			sprintf(sSoundFile, "EnglishSound/Words/%s.ogg", word.m_sSoundVietnameseFile.c_str());
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

	return 0;
}


void SoundManager::PlayVietnameseSpelling(std::string sSoundVietnameseFile)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
		sSoundVietnameseFile.c_str());
}
