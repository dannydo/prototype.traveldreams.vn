#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_

#include "cocos2d.h"
#include "SettingMenuNode.h"
#include "ButtonManagerNode.h"

class MainMenuLayer : public cocos2d::Layer
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
	void openSettingMenu(cocos2d::Object* sender);
	void clickInviteFriends(cocos2d::Object* sender);

	void addButtonLoginFacebook();
	void startTimeModeDemo(cocos2d::Object* sender);

	void showPopupUpdateApp();

	cocos2d::CCMenuItemImage* m_pLoginfacebook;
	cocos2d::MenuItemLabel* m_pItemLogout;
	cocos2d::MenuItemLabel* m_pItemShareLink;
	cocos2d::MenuItemLabel* m_pItemShareDialog;

	SettingMenuNode* m_pSettingNode	;

	ButtonNode* m_buttonLoginNode;
	ButtonNode* m_pButtonSettingNode;
	std::string m_sFacebookToken;
	ButtonManagerNode* m_pButtonManagerNode;
	bool m_isAddButtonLogin;
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