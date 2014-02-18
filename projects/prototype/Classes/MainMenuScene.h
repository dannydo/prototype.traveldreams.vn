#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_

#include "cocos2d.h"

class MainMenuLayer : public cocos2d::Layer
{
public:
	MainMenuLayer() {};
	virtual ~MainMenuLayer();
	bool init();
	CREATE_FUNC(MainMenuLayer);

private:
	void playGame();
	void loginFacebook();
	void LogoutFacebook();
	void shareLinkFacebook();
	void shareDialogFacebook();

	cocos2d::CCMenuItemImage* m_pLoginfacebook;
	cocos2d::MenuItemLabel* m_pItemLogout;
	cocos2d::MenuItemLabel* m_pItemShareLink;
	cocos2d::MenuItemLabel* m_pItemShareDialog;
};

class MainMenuScene : public cocos2d::Scene
{
public:
	MainMenuScene():_layer(NULL) {};
	~MainMenuScene();
	bool init();
	CREATE_FUNC(MainMenuScene);

	CC_SYNTHESIZE_READONLY(MainMenuLayer*, _layer, Layer);	
};

#endif