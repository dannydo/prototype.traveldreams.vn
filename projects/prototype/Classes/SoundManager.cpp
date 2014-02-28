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
		case _SET_WRONG_MOVE_:
			sFileName = "SoundEffectPC/WrongMove.wav";
			break;
		case _SET_SIMPLE_MATCH_:
			sFileName = "SoundEffectPC/SingleMatch.wav";
			break;
		case _SET_SIMPLE_COMBO_:
			sFileName = "SoundEffectPC/SingleComboEffect.wav";
			break;
		case _SET_GET_CHARACTER_:
			sFileName = "SoundEffectPC/GetCharacter.wav";
			break;
		case _SET_GET_START_:
			sFileName = "SoundEffectPC/GetStar.wav";
			break;
		case _SET_BONUS_WORD_APPEAR_:
			sFileName = "SoundEffectPC/openS.wav";
			break;
		case _SET_COMPLETE_WORD_:
			sFileName = "SoundEffectPC/CompleteWord.wav";
			break;
		case _SET_DOUBLE_COMPLE_EFFECT_:
			sFileName = "SoundEffectPC/DoubleComboEffect.wav";
			break;
		case _SET_COMBINE_DOUBLE_COMBO_:
			sFileName = "SoundEffectPC/CombineDoubleCombo.wav";
			break;
		case _SET_DROP_GEM_:
			sFileName = "SoundEffectPC/DropGem.wav";
			break;
		case _SET_WIN_:
			sFileName = "SoundEffectPC/Win.wav";
			break;
		default:
		case _SET_LOSE:
			sFileName = "SoundEffectPC/Lose.wav";
			break;
#else	
		case _SET_WRONG_MOVE_:
			sFileName = "SoundEffect/WrongMove.ogg";
			break;
		case _SET_SIMPLE_MATCH_:
			sFileName = "SoundEffect/SingleMatch.ogg";
			break;
		case _SET_SIMPLE_COMBO_:
			sFileName = "SoundEffect/SingleComboEffect.ogg";
			break;
		case _SET_GET_CHARACTER_:
			sFileName = "SoundEffect/GetCharacter.ogg";
			break;
		case _SET_GET_START_:
			sFileName = "SoundEffect/GetStar.ogg";
			break;
		case _SET_BONUS_WORD_APPEAR_:
			sFileName = "SoundEffect/openS.ogg";
			break;
		case _SET_COMPLETE_WORD_:
			sFileName = "SoundEffect/CompleteWord.ogg";
			break;
		case _SET_DOUBLE_COMPLE_EFFECT_:
			sFileName = "SoundEffect/DoubleComboEffect.ogg";
			break;
		case _SET_COMBINE_DOUBLE_COMBO_:
			sFileName = "SoundEffect/CombineDoubleCombo.ogg";
			break;
		case _SET_DROP_GEM_:
			sFileName = "SoundEffect/DropGem.ogg";
			break;
		case _SET_WIN_:
			sFileName = "SoundEffect/Win.ogg";
			break;
		default:
		case _SET_LOSE:
			sFileName = "SoundEffect/Lose.ogg";
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
