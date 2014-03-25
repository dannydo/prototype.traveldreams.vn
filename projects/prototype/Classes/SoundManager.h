#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameDataStructure.h"

using namespace cocos2d;

enum SoundEffectType
{
	_SET_DROP_,
	_SET_DRAG_FAIL_,
	_SET_DRAG_SUCCESS_,
	_SET_BASIC_MATCH_3_,
	_SET_DESTROY_LETTER_,
	_SET_UNLOCK_LETTER_,
	_SET_FILL_LETTER_, // fill letter on main word header
	_SET_COMPLETE_WORD_,

	_SET_CREATE_COMBO_4_,
	_SET_ACTIVATE_COMBO_4_,
	_SET_CREATE_COMBO_5_,
	_SET_ACTIVATE_COMBO_5_,
	_SET_CREATE_COMBO_6_,
	_SET_ACTIVATE_COMBO_6_,
	_SET_ACTIVATE_DOUBLE_COMBO_,
	_SET_ACTIVATE_TRIPLE_COMBO_,


	_SET_BUTTON_PRESS_,
	_SET_POPUP_,
	_SET_CHAPTER_SELECT_,
	_SET_LEVEL_SELECT_,
	_SET_START_LEVEL_,
	_SET_WIN_,
	_SET_FAIL_

	/*
	_SET_WRONG_MOVE_,
	_SET_SIMPLE_MATCH_,
	_SET_SIMPLE_COMBO_,
	_SET_GET_CHARACTER_,
	_SET_GET_START_,
	_SET_BONUS_WORD_APPEAR_,
	_SET_COMPLETE_WORD_,
	_SET_DOUBLE_COMPLE_EFFECT_,
	_SET_COMBINE_DOUBLE_COMBO_,
	_SET_DROP_GEM_,
	_SET_WIN_,
	_SET_LOSE*/
};

class SoundManager : Object
{
public:
	 typedef enum {
		kNone = 0,
    	kGameMusic,
    	kIntroMusic,
		kEndGameBonus
    } StateBackGroundMusic;

	static void PreloadSoundResource();

	static void PlayBackgroundMusic(StateBackGroundMusic stateBackGroundMusic);
	static void StopBackgroundMusic();

	static void PlaySoundEffect(const SoundEffectType& eSoundEffectType, float fDelayTime = 0.f);	

	static float PlaySpellingOfWord(Node* pPlayingNode, const Word& word);
private:
	void PlayVietnameseSpelling(std::string sSoundVietnameseFile);	

	void PlaySoundEffectUtil(Node* pNode, void* data);
	static SoundManager m_InternalSoundManager;
	static StateBackGroundMusic m_stateBackGroundMusic;
	
};

#endif _SOUND_MANAGER_H_