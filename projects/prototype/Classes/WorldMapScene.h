#ifndef _WORLD_MAP_H_
#define _WORLD_MAP_H_

#include "cocos2d.h"

class WorldMapLayer : public cocos2d::CCLayer
{
public:
	WorldMapLayer() {};
	virtual ~WorldMapLayer();
	bool init();
	CREATE_FUNC(WorldMapLayer);

private:
	void menuPlayChapterCallBack();
	void menuOpenIntroductionCallBack();
};

class WorldMapScene : public cocos2d::CCScene
{
public:
	WorldMapScene():_layer(NULL) {};
	~WorldMapScene();
	bool init();
	CREATE_FUNC(WorldMapScene);

	CC_SYNTHESIZE_READONLY(WorldMapLayer*, _layer, Layer);	
};

#endif