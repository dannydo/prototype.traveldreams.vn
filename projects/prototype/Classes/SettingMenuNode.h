#ifndef __SETTING_MENU_H__
#define __SETTING_MENU_H__

#include "cocos2d.h" 
#include "Database\UserTable.h"

typedef enum {
	kExitGame = 0,
    kMainMenu,
    kWorldMap,
    kLevelMap,
	kFlashCardCollection,
	kFlashCard,
	kPlayGame,
} SceneMode;

class SettingMenuNode : public cocos2d::Node
{
public:
	SettingMenuNode();
	virtual ~SettingMenuNode();
	bool init() override;
	CREATE_FUNC(SettingMenuNode);

	void show();
	void hide();

private:
	void clickMusic();
	void clickEffect();
	void clickVoice();
	void clickTutorial();
	void clickLogoutFacebook();
	void clickBack();

	cocos2d::LabelTTF* m_plabelVoice;
	cocos2d::LabelTTF* m_pLabelEffect;
	cocos2d::LabelTTF* m_pLabelMusic;
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