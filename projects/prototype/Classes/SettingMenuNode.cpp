#include "SettingMenuNode.h"
#include "Social/FacebookManager.h"
#include "MainMenuScene.h"
#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "SoundManager.h"
#include "Database\UserTable.h"
#include "FlashCardCollection.h"

USING_NS_CC;

SettingMenuNode::SettingMenuNode()
{

}

SettingMenuNode::~SettingMenuNode()
{
	
}

bool SettingMenuNode::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	m_Background = LayerColor::create(ccc4(216, 216, 216, 255), 500.0f, 960.0f);
	this->addChild(m_Background);

	m_pLabelMusic = LabelTTF::create("Music On", "Arial", 25);
	m_pLabelMusic->setColor(ccc3(0.0f, 0.0f, 0.0f));
    MenuItemLabel* m_pItemMusic = MenuItemLabel::create(m_pLabelMusic, CC_CALLBACK_0(SettingMenuNode::clickMusic, this));
    m_pItemMusic->setPosition(Point(100, 750));

	m_pLabelEffect = LabelTTF::create("Effect On", "Arial", 25);
	m_pLabelEffect->setColor(ccc3(0.0f, 0.0f, 0.0f));
    MenuItemLabel* m_pItemEffect = MenuItemLabel::create(m_pLabelEffect, CC_CALLBACK_0(SettingMenuNode::clickEffect, this));
    m_pItemEffect->setPosition(Point(250, 750));

	m_plabelVoice = LabelTTF::create("Voice On", "Arial", 25);
	m_plabelVoice->setColor(ccc3(0.0f, 0.0f, 0.0f));
    MenuItemLabel* m_pItemVoice = MenuItemLabel::create(m_plabelVoice, CC_CALLBACK_0(SettingMenuNode::clickVoice, this));
    m_pItemVoice->setPosition(Point(400, 750));

	LabelTTF* pLabelTutorial = LabelTTF::create("TUTORIAL", "Arial", 25);
	pLabelTutorial->setColor(ccc3(0.0f, 0.0f, 0.0f));
    MenuItemLabel* m_pItemTutorial = MenuItemLabel::create(pLabelTutorial, CC_CALLBACK_0(SettingMenuNode::clickTutorial, this));
    m_pItemTutorial->setPosition(Point(250, 700));

	LabelTTF* pLabelLogoutFacebook = LabelTTF::create("LOGOUT FACEBOOK", "Arial", 25);
	pLabelLogoutFacebook->setColor(ccc3(0.0f, 0.0f, 0.0f));
    MenuItemLabel* m_pItemLogoutFacebook = MenuItemLabel::create(pLabelLogoutFacebook, CC_CALLBACK_0(SettingMenuNode::clickLogoutFacebook, this));
    m_pItemLogoutFacebook->setPosition(Point(250, 600));
	m_pItemLogoutFacebook->setVisible(false);

	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnMusic", 1) != 1)
	{
		m_pLabelMusic->setString("Music Off");
	}

	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnEffect", 1) != 1)
	{
		m_pLabelEffect->setString("Effect Off");
	}

	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnVoice", 1) != 1)
	{
		m_plabelVoice->setString("Voice Off");
	}

	char *sTitleButtonBack;
	int iSize = Breadcrumb::getInstance()->getSceneModes().size();
	switch(Breadcrumb::getInstance()->getSceneMode(iSize - 2))
	{
	case SceneMode::kExitGame:
		{
			sTitleButtonBack = "EXIT GAME";
			m_pItemLogoutFacebook->setVisible(true);
			break;
		}
	case SceneMode::kMainMenu :
		sTitleButtonBack = "MAIN MENU";
		break;

	case SceneMode::kWorldMap :
		sTitleButtonBack = "BACK TO WORLD MAP";
		break;

	case SceneMode::kLevelMap :
		{
			sTitleButtonBack = "BACK TO LEVEL MAP";
			if (Breadcrumb::getInstance()->getSceneMode(iSize - 1) == SceneMode::kPlayGame)
				sTitleButtonBack = "END GAME";
			break;
		}
	case SceneMode::kFlashCardCollection :
		sTitleButtonBack = "BACK TO FLASHCARD COLLECTION";
		break;
	}

	LabelTTF* pLabelBack = LabelTTF::create(sTitleButtonBack, "Arial", 25);
	pLabelBack->setColor(ccc3(0.0f, 0.0f, 0.0f));
    MenuItemLabel* m_pItemBack = MenuItemLabel::create(pLabelBack, CC_CALLBACK_0(SettingMenuNode::clickBack, this));
	m_pItemBack->setPosition(Point(250, 650));

	CCMenu* pMenu = CCMenu::create( m_pItemMusic, m_pItemEffect, m_pItemVoice, m_pItemTutorial, m_pItemBack, m_pItemLogoutFacebook,  NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu);

	m_iShowSetting = false;
	m_isClick = false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
	if(!FacebookManager::getInstance()->isLogined())
	{
		m_pItemLogoutFacebook->setVisible(false);
	}
#endif
	
	return true;
}

void SettingMenuNode::clickMusic()
{
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnMusic", 1) != 1)
	{
		m_pLabelMusic->setString("Music On");
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnMusic", 1);
		SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kNone);
	}
	else
	{
		m_pLabelMusic->setString("Music Off");
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnMusic", 0);
		SoundManager::StopBackgroundMusic();
	}
}

void SettingMenuNode::clickEffect()
{
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnEffect", 1) != 1)
	{
		m_pLabelEffect->setString("Effect On");
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnEffect", 1);
	}
	else
	{
		m_pLabelEffect->setString("Effect Off");
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnEffect", 0);
	}
}

void SettingMenuNode::clickVoice()
{
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnVoice", 1) != 1)
	{
		m_plabelVoice->setString("Voice On");
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnVoice", 1);
	}
	else
	{
		m_plabelVoice->setString("Voice Off");
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnVoice", 0);
	}
}

void SettingMenuNode::clickTutorial()
{
}

void SettingMenuNode::clickBack()
{
	switch(Breadcrumb::getInstance()->getSceneModePopBack())
	{
	case SceneMode::kExitGame:
		{
			Director::getInstance()->end();
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				exit(0);
			#endif
			break;
		}
	case SceneMode::kMainMenu :
		{
			MainMenuScene* pMainMenu = MainMenuScene::create();
			Director::getInstance()->replaceScene(pMainMenu);
			break;
		}

	case SceneMode::kWorldMap :
		{
			WorldMapScene* pWorldMap = WorldMapScene::create();
			Director::getInstance()->replaceScene(pWorldMap);
			break;
		}

	case SceneMode::kLevelMap :
		{
			UserTable::getInstance()->updateLife(1);
			LevelMapScene* pLevelMap = LevelMapScene::create();
			Director::getInstance()->replaceScene(pLevelMap);
			break;
		}
	case SceneMode::kFlashCardCollection :
		{
			FlashCardCollectionScene* pFlashCardCollection = FlashCardCollectionScene::create();
			Director::getInstance()->replaceScene(pFlashCardCollection);
			break;
		}
	}
}

void SettingMenuNode::clickLogoutFacebook()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    FacebookManager::getInstance()->logoutByMode();
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif
}

void SettingMenuNode::show()
{
	MoveTo* actionParentMoveBy = MoveTo::create(0.25f, Point(-500.0f, 0.0f));
	this->getParent()->runAction(actionParentMoveBy);
	m_iShowSetting = true;
}

void SettingMenuNode::hide()
{
	MoveTo* actionParentMoveBy = MoveTo::create(0.25f, Point(0.0f, 0.0f));
	this->getParent()->runAction(actionParentMoveBy);
	m_iShowSetting = false;
}

bool SettingMenuNode::onTouchCustomNodeBegan(Touch* pTouch,  Event* pEvent)
{
	Point touchPosition = pTouch->getLocation();

	if (touchPosition.x < 140.0f)
	{
		m_isClick = true;
	}

	if (m_iShowSetting)
		return true;

	return false;
}

void SettingMenuNode::onTouchCustomNodeMoved(Touch* pTouch,  Event* pEvent)
{

}

void SettingMenuNode::onTouchCustomNodeEnded(Touch* pTouch,  Event* pEvent)
{
	if(m_isClick)
	{
		if (m_iShowSetting)
		{
			this->hide();
		}
		else
		{
			this->show();
		}
	}
	m_isClick = false;
}








Breadcrumb* Breadcrumb::m_Breadcrumb = NULL;

Breadcrumb::Breadcrumb()
{

}

Breadcrumb::~Breadcrumb()
{
	
}

Breadcrumb* Breadcrumb::getInstance()
{
	if (m_Breadcrumb == NULL) {
		m_Breadcrumb = new Breadcrumb();
	}

	return m_Breadcrumb;
}

SceneMode Breadcrumb::getSceneModePopBack()
{
	m_sceneModes.pop_back();
	SceneMode sceneMode = m_sceneModes.back();
	m_sceneModes.pop_back();
	return sceneMode;
}

SceneMode Breadcrumb::getSceneMode(int iIndex)
{
	SceneMode sceneMode = m_sceneModes[iIndex];
	return sceneMode;
}

void Breadcrumb::addSceneMode(SceneMode sceneMode)
{
	if (m_sceneModes.empty())
	{
		m_sceneModes.push_back(sceneMode);
	}
	else if (sceneMode != getSceneMode(m_sceneModes.size() - 1))
	{
		m_sceneModes.push_back(sceneMode);
	}
}