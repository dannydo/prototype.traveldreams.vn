#ifndef __SETTING_MENU_H__
#define __SETTING_MENU_H__

#include "cocos2d.h" 
#include "Database\UserTable.h"
#include "CustomNode.h"

typedef enum {
	kExitGame = 0,
    kMainMenu,
    kWorldMap,
    kLevelMap,
	kFlashCardCollection,
	kFlashCard,
	kPlayGame,
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

private:
	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchCustomNodeMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override; 
	void onTouchCustomNodeEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

	void clickMusic();
	void clickEffect();
	void clickVoice();
	void clickTutorial();
	void clickLogoutFacebook();
	void clickBack();

	//cocos2d::LabelTTF* m_plabelVoice;
	cocos2d::MenuItemToggle* m_pMenuVoice;
	//cocos2d::LabelTTF* m_pLabelEffect;
	cocos2d::MenuItemToggle* m_pMenuEffect;
	//cocos2d::LabelTTF* m_pLabelMusic;
	cocos2d::MenuItemToggle* m_pMenuMusic;
	//cocos2d::LayerColor*m_Background;
	cocos2d::Sprite* m_pBackground;

	bool m_iShowSetting;
	bool m_isClick;
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

private:
	Breadcrumb();
	virtual ~Breadcrumb();

	static Breadcrumb* m_Breadcrumb;
	std::vector<SceneMode> m_sceneModes;
};

#endif;