#ifndef _LEVEL_MAP_H_
#define _LEVEL MAP_H_

#include "cocos2d.h"

#include "SimpleAudioEngine.h"

class LevelMapLayer : public cocos2d::CCLayer
{
public:
	LevelMapLayer() {};
	virtual ~LevelMapLayer();
	bool init();
	CREATE_FUNC(LevelMapLayer);

	void menuLevelSelected(Object* pSender);
	void menuBackToWorldMap();
	void menuOpenFlashCardCallBack();
	void showPopupTargetGame(const int& iLevel);
};

class LevelMapScene : public cocos2d::CCScene
{
public:
	LevelMapScene():_layer(NULL) {};
	~LevelMapScene();
	bool init();
	CREATE_FUNC(LevelMapScene);

	CC_SYNTHESIZE_READONLY(LevelMapLayer*, _layer, Layer);	
};

#endif