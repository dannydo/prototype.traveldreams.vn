#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_

#include "cocos2d.h"
#include "APIService\UserService.h"
#include "SettingMenuNode.h"
#include "APIService\UserService.h"	 
#include "ButtonManagerNode.h"

class MainMenuLayer : public cocos2d::Layer, InterfaceService
{
public:
	MainMenuLayer() {};
	virtual ~MainMenuLayer();
	bool init();
	CREATE_FUNC(MainMenuLayer);

	void update(float dt) override;

private:
	void playGame(cocos2d::Object* sender);
	void loginFacebook(cocos2d::Object* sender);
	void LogoutFacebook();
	void shareLinkFacebook();
	void shareDialogFacebook();
	void openSettingMenu(cocos2d::Object* sender);

	void startTimeModeDemo(cocos2d::Object* sender);

	void resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey) override;

	cocos2d::CCMenuItemImage* m_pLoginfacebook;
	cocos2d::MenuItemLabel* m_pItemLogout;
	cocos2d::MenuItemLabel* m_pItemShareLink;
	cocos2d::MenuItemLabel* m_pItemShareDialog;

	SettingMenuNode* m_pSettingNode	;

	ButtonNode* m_buttonLoginNode;
	ButtonNode* m_pButtonSettingNode;
	std::string m_sFacebookToken;
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