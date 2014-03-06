#ifndef __GAME_TRACKING_H__
#define __GAME_TRACKING_H__

#include "cocos2d.h"

class GameTracking
{
public:
	static void saveFileTrackingLevel(const int& ilevel, const int& iMove, const int& iLetterUnlock, const char* sStatusGame);
};
#endif