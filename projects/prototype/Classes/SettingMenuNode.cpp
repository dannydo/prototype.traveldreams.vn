#include "SettingMenuNode.h"
#include "MainMenuScene.h"
#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "SoundManager.h"
#include "Database\UserTable.h"
#include "FlashCardCollection.h"
#include "SystemEventHandle.h"

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

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

	m_pBackground =  Sprite::create("PanelSetting/panel-setting.png");
	m_pBackground->setAnchorPoint(Point(0,0));
	this->addChild(m_pBackground);

	m_pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(m_pButtonManagerNode);

	Sprite* pEffectSprite = Sprite::create("PanelSetting/Fx_on.PNG");
	Sprite* pEffectSpriteActive = Sprite::create("PanelSetting/Fx_off.PNG");
	ButtonNode* pButtonEffectNode = ButtonNode::createButtonSprite(pEffectSprite, pEffectSpriteActive, CC_CALLBACK_0(SettingMenuNode::clickEffect, this));
	pButtonEffectNode->setPosition(Point(410, 710));
	m_pButtonManagerNode->addButtonNode(pButtonEffectNode);

	Sprite* pMusicSprite = Sprite::create("PanelSetting/music_on.PNG");
	Sprite* pMusicSpriteActive = Sprite::create("PanelSetting/music_off.PNG");
	ButtonNode* pButtonMusicNode = ButtonNode::createButtonSprite(pMusicSprite, pMusicSpriteActive, CC_CALLBACK_0(SettingMenuNode::clickMusic, this));
	pButtonMusicNode->setPosition(Point(410, 620));
	m_pButtonManagerNode->addButtonNode(pButtonMusicNode);

	Sprite* pVoiceSprite = Sprite::create("PanelSetting/voice_on.PNG");
	Sprite* pVoiceSpriteActive = Sprite::create("PanelSetting/voice_off.PNG");
	ButtonNode* pButtonVoiceNode = ButtonNode::createButtonSprite(pVoiceSprite, pVoiceSpriteActive, CC_CALLBACK_0(SettingMenuNode::clickVoice, this));
	pButtonVoiceNode->setPosition(Point(410, 530));
	m_pButtonManagerNode->addButtonNode(pButtonVoiceNode);

	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnEffect", 1) == 0)
		pButtonEffectNode->setStateActive(true);

	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnMusic", 1) == 0)
		pButtonMusicNode->setStateActive(true);

	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnVoice", 1) == 0)
		pButtonVoiceNode->setStateActive(true);


	 m_isAddButtonFacebook = false;
	int iSize = Breadcrumb::getInstance()->getSceneModes().size();
	switch(Breadcrumb::getInstance()->getSceneMode(iSize - 2))
	{
		case SceneMode::kExitGame:
		{
			m_isAddButtonFacebook = true;
			this->addButtonFacebook();
			break;
		}
		case SceneMode::kMainMenu :
		{
			Sprite* pBackSprite = Sprite::create("PanelSetting/menu-btn.png");
			ButtonNode* pButtonBack = ButtonNode::createButtonSprite(pBackSprite, CC_CALLBACK_1(SettingMenuNode::clickBack, this));
			pButtonBack->setPosition(Point(261, 200.0f));
			m_pButtonManagerNode->addButtonNode(pButtonBack);
			break;
		}
		case SceneMode::kWorldMap :
		{
			Sprite* pBackSprite = Sprite::create("PanelSetting/worldmap-btn.png");
			ButtonNode* pButtonBack = ButtonNode::createButtonSprite(pBackSprite, CC_CALLBACK_1(SettingMenuNode::clickBack, this));
			pButtonBack->setPosition(Point(261, 200.0f));
			m_pButtonManagerNode->addButtonNode(pButtonBack);
			break;
		}
		case SceneMode::kLevelMap :
		{
			Sprite* pBackSprite;
			if (Breadcrumb::getInstance()->getSceneMode(iSize - 1) == SceneMode::kPlayGame)
				pBackSprite = Sprite::create("PanelSetting/exit-level-btn.png");
			else
				pBackSprite = Sprite::create("PanelSetting/level-map-btn.png");
			ButtonNode* pBackButtonNode = ButtonNode::createButtonSprite(pBackSprite, CC_CALLBACK_1(SettingMenuNode::clickBack, this));
			pBackButtonNode->setPosition(Point(261, 200.0f));
			m_pButtonManagerNode->addButtonNode(pBackButtonNode);
			break;
		}
		case SceneMode::kFlashCardCollection :
		{
			Sprite* pBackSprite = Sprite::create("PanelSetting/flashcard.PNG");
			ButtonNode* pButtonBack = ButtonNode::createButtonSprite(pBackSprite, CC_CALLBACK_1(SettingMenuNode::clickBack, this));
			pButtonBack->setPosition(Point(261, 200.0f));
			m_pButtonManagerNode->addButtonNode(pButtonBack);
			break;
		}
	}

	Sprite* pBackButtonSprite = Sprite::create("PanelSetting/tutorial-btn.png");
	ButtonNode* pBackButtonNode = ButtonNode::createButtonSprite(pBackButtonSprite, CC_CALLBACK_1(SettingMenuNode::clickTutorial, this));
	pBackButtonNode->setPosition(Point(261, 350.0f));
	m_pButtonManagerNode->addButtonNode(pBackButtonNode);


	m_iShowSetting = false;
	m_isClick = false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
#endif

	this->scheduleUpdate();
	m_iStatusButtonFacebook = -1;

	return true;
}

void SettingMenuNode::update(float dt)
{
	if (m_isAddButtonFacebook)
	{
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		if (m_iStatusButtonFacebook == 0 && !FacebookManager::getInstance()->isLogined() && UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", -1) == 0)
		{
			this->addButtonFacebook();
		}
		else if (m_iStatusButtonFacebook == 1 && FacebookManager::getInstance()->isLogined() && UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", 0) == 1)
		{
			this->addButtonFacebook();
		}
	#endif
	}
}

void SettingMenuNode::addButtonFacebook()
{
	m_pButtonManagerNode->removeButtonNode(m_pButtonFacebook);
	Sprite* pFacebookLoginSprite = Sprite::create("PanelSetting/loginFB.PNG");
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		if(FacebookManager::getInstance()->isLogined())
			pFacebookLoginSprite = Sprite::create("PanelSetting/logoutFB.png");
	#endif

	m_pButtonFacebook = ButtonNode::createButtonSprite(pFacebookLoginSprite, CC_CALLBACK_1(SettingMenuNode::clickFacebook, this));
	m_pButtonFacebook->setPosition(Point(261, 200.0f));
	m_pButtonManagerNode->addButtonNode(m_pButtonFacebook);
	m_iStatusButtonFacebook = -1;
}

void SettingMenuNode::setStatusButtonFacebook(const int& iStatus)
{
	m_iStatusButtonFacebook = iStatus;
}

void SettingMenuNode::clickMusic()
{
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnMusic", 1) != 1)
	{
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnMusic", 1);
		SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kNone);
	}
	else
	{
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnMusic", 0);
		SoundManager::StopBackgroundMusic();
	}
}

void SettingMenuNode::clickEffect()
{
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnEffect", 1) != 1)
	{
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnEffect", 1);
	}
	else
	{
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnEffect", 0);
	}
}

void SettingMenuNode::clickVoice()
{
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnVoice", 1) != 1)
	{
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnVoice", 1);
	}
	else
	{
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnVoice", 0);
	}
}

void SettingMenuNode::clickTutorial(Object* sender)
{
}

void SettingMenuNode::clickBack(Object* sender)
{
	this->actionBack();
	//auto actionHide = CallFunc::create(this, callfunc_selector(SettingMenuNode::hide));
	//auto actionBackTo = CallFunc::create(this, callfunc_selector(SettingMenuNode::actionBack));
	//this->runAction(Sequence::create(actionHide, DelayTime::create(0.15f)->clone(), actionBackTo, NULL));
}

void SettingMenuNode::actionBack()
{
	m_pSettingButton->setStateActive(!m_pSettingButton->getStateActive());
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

void SettingMenuNode::clickFacebook(Object* sender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if(FacebookManager::getInstance()->isLogined() && UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", 0) == 1)
	{	
		FacebookManager::getInstance()->logoutByMode();
		m_iStatusButtonFacebook = 0;
	}
	else if(!FacebookManager::getInstance()->isLogined() && UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", -1) == 0)
	{
		UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 0);
		SystemEventHandle::getInstance()->onStartConnectFacebook();
		m_iStatusButtonFacebook = 1;
	}
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif
}

void SettingMenuNode::show()
{
	// play sound effect
	SoundManager::PlaySoundEffect(_SET_GAME_MENU_);

	MoveTo* actionParentMoveTo = MoveTo::create(0.15f, Point(524.0f, 0.0f));
	this->getParent()->runAction(actionParentMoveTo);
	m_iShowSetting = true;
}

void SettingMenuNode::hide()
{
	// play sound effect
	SoundManager::PlaySoundEffect(_SET_GAME_MENU_);

	MoveTo* actionParentMoveTo = MoveTo::create(0.15f, Point(0.0f, 0.0f));
	this->getParent()->runAction(actionParentMoveTo);
	m_iShowSetting = false;
}

bool SettingMenuNode::onTouchCustomNodeBegan(Touch* pTouch,  Event* pEvent)
{
	Point touchPosition = pTouch->getLocation();

	if (touchPosition.x > 524.0f)
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
		m_pSettingButton->setStateActive(!m_pSettingButton->getStateActive());
		if (m_iShowSetting)
			this->hide();
		else
			this->show();
	}
	m_isClick = false;
}

void SettingMenuNode::addButtonSetting(ButtonNode* pSettingButton)
{
	m_pSettingButton = pSettingButton;
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
