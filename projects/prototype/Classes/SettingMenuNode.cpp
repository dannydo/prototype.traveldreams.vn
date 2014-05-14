#include "SettingMenuNode.h"
#include "MainMenuScene.h"
#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "SoundManager.h"
#include "Database\UserTable.h"
#include "FlashCardCollection.h"
#include "SystemEventHandle.h"
#include "Database\InitDatabase.h"
#include "SystemEventHandle.h"
#include "WaitingNode.h"
#include "ConfirmQuitLevelNode.h"
#include "PopupConfirmNode.h"

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

	m_OnHideSettingMenuCallback = nullptr;

	Sprite* pBackground =  Sprite::create("PanelSetting/panel-setting.png");
	pBackground->setAnchorPoint(Point(0,0));
	this->addChild(pBackground);

	Sprite* pLogoImage = Sprite::create("PanelSetting/logo.png");
	pLogoImage->setPosition(Point(252.5f, 795.0f));
	this->addChild(pLogoImage);

	m_pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(m_pButtonManagerNode);

	Sprite* pEffectSprite = Sprite::create("PanelSetting/btn_sfx.png");
	Sprite* pEffectSpriteActive = Sprite::create("PanelSetting/btn_sound_off.png");
	ButtonNode* pButtonEffectNode = ButtonNode::createButtonSprite(pEffectSprite, pEffectSpriteActive, CC_CALLBACK_0(SettingMenuNode::clickEffect, this));
	pButtonEffectNode->setPosition(Point(390.0f, 625.0f));
	m_pButtonManagerNode->addButtonNode(pButtonEffectNode);

	Sprite* pMusicSprite = Sprite::create("PanelSetting/btn_music.png");
	Sprite* pMusicSpriteActive = Sprite::create("PanelSetting/btn_music_off.png");
	ButtonNode* pButtonMusicNode = ButtonNode::createButtonSprite(pMusicSprite, pMusicSpriteActive, CC_CALLBACK_0(SettingMenuNode::clickMusic, this));
	pButtonMusicNode->setPosition(Point(250.0f, 625.0f));
	m_pButtonManagerNode->addButtonNode(pButtonMusicNode);

	Sprite* pVoiceSprite = Sprite::create("PanelSetting/btn_voice.png");
	Sprite* pVoiceSpriteActive = Sprite::create("PanelSetting/btn_voice_off.png");
	ButtonNode* pButtonVoiceNode = ButtonNode::createButtonSprite(pVoiceSprite, pVoiceSpriteActive, CC_CALLBACK_0(SettingMenuNode::clickVoice, this));
	pButtonVoiceNode->setPosition(Point(113.0f, 625.0f));
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

			/*
			Sprite* pResetSprite = Sprite::create("PanelSetting/btn_reset.png");
			ButtonNode* pButtonReset = ButtonNode::createButtonSprite(pResetSprite, CC_CALLBACK_1(SettingMenuNode::clickReset, this));
			pButtonReset->setPosition(Point(252.5f, 278.0f));
			m_pButtonManagerNode->addButtonNode(pButtonReset);
			*/

		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			// Van Dao
			m_pUnlockLabel = LabelBMFont::create("Unlock Levels", "fonts/font_score.fnt");
			m_pUnlockLabel->setAnchorPoint(Point(0.5f, 0.5f));
			m_pUnlockLabel->setPosition(Point(230.0f, 52.0f));

			/*
			if(UserDefault::getInstance()->getIntegerForKey("IsUnlockALlLevel", 0) == 1)
			{
				m_pUnlockLabel->setString("Not Unlock Levels");
			}
			else
			{
				m_pUnlockLabel->setString("Unlock Levels");
			}
			*/

			Sprite* pUnlockSprite = Sprite::create("PanelSetting/btn_unlock_all_level.png");
			pUnlockSprite->addChild(m_pUnlockLabel);

			ButtonNode* pButtonUnlock = ButtonNode::createButtonSprite(pUnlockSprite, CC_CALLBACK_1(SettingMenuNode::clickUnlockAllLevel, this));
			pButtonUnlock->setPosition(Point(252.5f, 170.0f));
			m_pButtonManagerNode->addButtonNode(pButtonUnlock);
		#endif

			break;
		}
		case SceneMode::kMainMenu : 
		case SceneMode::kWorldMap :
		case SceneMode::kFlashCardCollection :
		{
			Sprite* pMainMenuSprite = Sprite::create("PanelSetting/btn_mainmenu.png");
			ButtonNode* pButtonManiMenu = ButtonNode::createButtonSprite(pMainMenuSprite, CC_CALLBACK_1(SettingMenuNode::clickMainMenu, this));
			pButtonManiMenu->setPosition(Point(252.5f, 495.0f));
			m_pButtonManagerNode->addButtonNode(pButtonManiMenu);

			Sprite* pBackSprite = Sprite::create("PanelSetting/btn_back.png");
			ButtonNode* pButtonBack = ButtonNode::createButtonSprite(pBackSprite, CC_CALLBACK_1(SettingMenuNode::clickBack, this));
			pButtonBack->setPosition(Point(252.5f, 278.0f));
			m_pButtonManagerNode->addButtonNode(pButtonBack);

			break;
		}
		case SceneMode::kLevelMap :
		{
			Sprite* pBackSprite;
			if (Breadcrumb::getInstance()->getSceneMode(iSize - 1) == SceneMode::kPlayGame)
			{
				Sprite* pBackToGameSprite = Sprite::create("PanelSetting/btn_resume.png");
				ButtonNode* pButtonBackToGame = ButtonNode::createButtonSprite(pBackToGameSprite, CC_CALLBACK_1(SettingMenuNode::clickResume, this));
				pButtonBackToGame->setPosition(Point(252.5f, 495.0f));
				m_pButtonManagerNode->addButtonNode(pButtonBackToGame);

				Sprite* pQuitLevelSprite = Sprite::create("PanelSetting/btn_quitlevel.png");
				ButtonNode* pButtonQuitLevel = ButtonNode::createButtonSprite(pQuitLevelSprite, CC_CALLBACK_1(SettingMenuNode::clickQuitLevel, this));
				pButtonQuitLevel->setPosition(Point(252.5f, 278.0f));
				m_pButtonManagerNode->addButtonNode(pButtonQuitLevel);
			}
			else
			{
				Sprite* pMainMenuSprite = Sprite::create("PanelSetting/btn_mainmenu.png");
				ButtonNode* pButtonManiMenu = ButtonNode::createButtonSprite(pMainMenuSprite, CC_CALLBACK_1(SettingMenuNode::clickMainMenu, this));
				pButtonManiMenu->setPosition(Point(252.5f, 495.0f));
				m_pButtonManagerNode->addButtonNode(pButtonManiMenu);

				Sprite* pBackSprite = Sprite::create("PanelSetting/btn_back.png");
				ButtonNode* pButtonBack = ButtonNode::createButtonSprite(pBackSprite, CC_CALLBACK_1(SettingMenuNode::clickBack, this));
				pButtonBack->setPosition(Point(252.5f, 278.0f));
				m_pButtonManagerNode->addButtonNode(pButtonBack);
			}
			break;
		}
	}
	
	Sprite* pTutorialSprite = Sprite::create("PanelSetting/tutorial.png");
	ButtonNode* pButtonTutorial = ButtonNode::createButtonSprite(pTutorialSprite, CC_CALLBACK_1(SettingMenuNode::clickTutorial, this));
	pButtonTutorial->setPosition(Point(252.5f, 386.0f));
	m_pButtonManagerNode->addButtonNode(pButtonTutorial);

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
	Sprite* pFacebookLoginSprite = Sprite::create("PanelSetting/btn_login.png");
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		if(FacebookManager::getInstance()->isLogined())
			pFacebookLoginSprite = Sprite::create("PanelSetting/btn_logout.png");
	#endif

	m_pButtonFacebook = ButtonNode::createButtonSprite(pFacebookLoginSprite, CC_CALLBACK_1(SettingMenuNode::clickFacebook, this));
	m_pButtonFacebook->setPosition(Point(261, 495.0f));
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

void SettingMenuNode::clickReset(cocos2d::Object* sender)
{
	this->hide();
	PopupConfirmNode* popupConfirm = PopupConfirmNode::createLayout("DO YOU WANT TO RESET GAME", PopupConfirmType::eResetGame);
	this->getParent()->addChild(popupConfirm, 10000);
}

void SettingMenuNode::clickMainMenu(cocos2d::Object* sender)
{
	Breadcrumb::getInstance()->resetSceneNodeToMainMenu();
	MainMenuScene* pMainMenuScene = MainMenuScene::create();
	Director::getInstance()->replaceScene(pMainMenuScene);
}

void SettingMenuNode::clickQuitLevel(cocos2d::Object* sender)
{
	this->hide();
	 
	// check play time mode van dao
	bool bPlayCustomMode = false;
	if (bPlayCustomMode)
	{
		PopupConfirmNode* popupConfirm = PopupConfirmNode::createLayout("DO YOU WANT TO QUIT GAME", PopupConfirmType::eQuitGameCustomMode);
		this->getParent()->addChild(popupConfirm, 10000);
	}
	else
	{
		int iCurrentLevel = GameConfigManager::getInstance()->GetCurrentLevelId();
		std::string sChapterId = GameConfigManager::getInstance()->GetCurrentChapterID();

		ConfirmQuitLevelNode* confirmQuitLevel = ConfirmQuitLevelNode::createLayout(iCurrentLevel, sChapterId);
		this->getParent()->addChild(confirmQuitLevel, 10000);
	}
}

void SettingMenuNode::clickResume(cocos2d::Object* sender)
{
	this->hide();
}

void SettingMenuNode::show()
{
	// play sound effect
	SoundManager::PlaySoundEffect(_SET_GAME_MENU_);

	MoveTo* actionParentMoveTo = MoveTo::create(0.15f, Point(505.0f, 0.0f));
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
	
	if ((touchPosition.x > 505.0f && touchPosition.y > 104) || touchPosition.x > 610)
	{
		m_isClick = true;
	}

	if (m_iShowSetting && m_isClick)
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
		if (m_OnHideSettingMenuCallback == nullptr)
		{
			m_pSettingButton->setStateActive(!m_pSettingButton->getStateActive());
			if (m_iShowSetting)
				this->hide();
			else
				this->show();
		}
		else
			m_OnHideSettingMenuCallback();
	}
	m_isClick = false;
}

void SettingMenuNode::addButtonSetting(ButtonNode* pSettingButton)
{
	m_pSettingButton = pSettingButton;
}

void SettingMenuNode::clickUnlockAllLevel(cocos2d::Object* sender)
{
	// Van Dao
	UserDefault::getInstance()->setIntegerForKey("IsUnlockALlLevel", 1);
	/*
	if(UserDefault::getInstance()->getIntegerForKey("IsUnlockALlLevel", 0) == 1)
	{
		m_pUnlockLabel->setString("Unlock Levels");
		UserDefault::getInstance()->setIntegerForKey("IsUnlockALlLevel", 0);
	}
	else
	{
		m_pUnlockLabel->setString("Not Unlock Levels");
		UserDefault::getInstance()->setIntegerForKey("IsUnlockALlLevel", 1);
	}
	*/
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

void Breadcrumb::resetSceneNodeToMainMenu()
{
	while(!m_sceneModes.empty())
	{
		m_sceneModes.pop_back();
	}

	m_sceneModes.push_back(SceneMode::kExitGame);
}