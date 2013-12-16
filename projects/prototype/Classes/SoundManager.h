#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;

enum SoundEffectType
{
	_SET_WRONG_MOVE_,
	_SET_SIMPLE_MATCH_,
	_SET_SIMPLE_COMBO_,
	_SET_GET_CHARACTER_,
	_SET_GET_START_,
	_SET_COMPLETE_WORD_,
	_SET_DOUBLE_COMPLE_EFFECT_,
	_SET_COMBINE_DOUBLE_COMBO_,
	_SET_DROP_GEM_,
	_SET_WIN_,
	_SET_LOSE
};

class SoundManager : Object
{
public:
	static void PreloadSoundResource();

	static void PlayBackgroundMusic();
	static void StopBackgroundMusic();

	static void PlayIntroMusic();
	static void StopIntroMusic();

	static void PlaySoundEffect(const SoundEffectType& eSoundEffectType, float fDelayTime = 0.f);	
private:
	void PlaySoundEffectUtil(Node* pNode, void* data);
	static SoundManager m_InternalSoundManager;
};

#endif _SOUND_MANAGER_H_