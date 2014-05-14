#ifndef __SETTING_MENU_H__
#define __SETTING_MENU_H__

#include "cocos2d.h" 
#include "Database\UserTable.h"
#include "CustomNode.h"	
#include "ButtonManagerNode.h"

typedef enum {
	kExitGame = 0,
    kMainMenu,
    kWorldMap,
    kLevelMap,
	kFlashCardCollection,
	kFlashCard,
	kPlayGame,
	kMyPackage,
	kPopularPackage,
} SceneMode;

class SettingMenuNode : public CustomNode
{
public:
	SettingMenuNode();
	virtual ~SettingMenuNode();
	bool init() override;
	CREATE_FUNC(SettingMenuNode);

	void show();
	void hide();
	inline bool getShowSetting() { return m_iShowSetting; };
	void addButtonSetting(ButtonNode* pSettingButton);
	void update(float dt) override;
	void setStatusButtonFacebook(const int& iStatus);

private:
	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchCustomNodeMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override; 
	void onTouchCustomNodeEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

	void clickMusic();
	void clickEffect();
	void clickVoice();
	void clickTutorial(cocos2d::Object* sender);
	void clickFacebook(cocos2d::Object* sender);
	void clickBack(cocos2d::Object* sender);
	void clickReset(cocos2d::Object* sender);
	void clickMainMenu(cocos2d::Object* sender);
	void clickQuitLevel(cocos2d::Object* sender);
	void clickResume(cocos2d::Object* sender);
	void clickUnlockAllLevel(cocos2d::Object* sender);

	void actionBack();
	void addButtonFacebook();

	cocos2d::MenuItemToggle* m_pMenuVoice;
	cocos2d::MenuItemToggle* m_pMenuEffect;
	cocos2d::MenuItemToggle* m_pMenuMusic;
	
	ButtonNode* m_pSettingButton;
	ButtonManagerNode* m_pButtonManagerNode;
	bool m_isAddButtonFacebook;
	ButtonNode* m_pButtonFacebook;

	bool m_iShowSetting;
	bool m_isClick;
	int m_iStatusButtonFacebook;

	cocos2d::LabelBMFont *m_pUnlockLabel;
};

class Breadcrumb
{
public:
	static Breadcrumb* getInstance();
	bool init();

	SceneMode getSceneModePopBack();
	SceneMode getSceneMode(int iIndex);
	inline std::vector<SceneMode> getSceneModes() { return m_sceneModes; };
	void addSceneMode(SceneMode sceneMode);

	void resetSceneNodeToMainMenu();

private:
	Breadcrumb();
	virtual ~Breadcrumb();

	static Breadcrumb* m_Breadcrumb;
	std::vector<SceneMode> m_sceneModes;
};

#endif;