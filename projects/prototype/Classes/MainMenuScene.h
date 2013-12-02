#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_

#include "cocos2d.h"

#include "SimpleAudioEngine.h"

class MainMenuLayer : public cocos2d::CCLayerColor
{
public:
	MainMenuLayer() {};
	virtual ~MainMenuLayer();
	bool init();
	CREATE_FUNC(MainMenuLayer);

	void menuItemSelected(Object* pSender);

	
};

class MainMenuScene : public cocos2d::CCScene
{
public:
	MainMenuScene():_layer(NULL) {};
	~MainMenuScene();
	bool init();
	CREATE_FUNC(MainMenuScene);

	CC_SYNTHESIZE_READONLY(MainMenuLayer*, _layer, Layer);	
};

#endif