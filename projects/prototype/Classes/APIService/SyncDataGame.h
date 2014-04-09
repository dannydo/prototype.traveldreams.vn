#ifndef __SYNC_DATA_GAME_H__
#define __SYNC_DATA_GAME_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class SyncDataGame : public cocos2d::Object
{
public:
	SyncDataGame();
	~SyncDataGame();

	bool syncRun();

private:
};
#endif