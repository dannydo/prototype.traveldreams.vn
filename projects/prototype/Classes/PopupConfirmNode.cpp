#include "PopupConfirmNode.h"
#include "ButtonManagerNode.h"
#include "MainMenuScene.h"
#include "WaitingNode.h"
#include "GameConfigManager.h"
#include "Database\InitDatabase.h"
#include "SystemEventHandle.h"
#include "FunctionCommon.h"
#include "FlashCardCollection.h"

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

PopupConfirmNode::~PopupConfirmNode ()
{
}

PopupConfirmNode* PopupConfirmNode::createLayout(const std::string& sTitle, const std::string& sDescription, const PopupConfirmActionType& popupConfirmActionType, const PopupConfirmType& popupConfirmType)
{
	PopupConfirmNode* pPopupConfirm = new PopupConfirmNode();
	pPopupConfirm->m_sTitle = sTitle;
	pPopupConfirm->m_sDescription = sDescription;
	pPopupConfirm->m_popupConfirmActionType = popupConfirmActionType;
	pPopupConfirm->m_popupConfirmType = popupConfirmType;

	if(pPopupConfirm->init())
	{
		pPopupConfirm->autorelease();
		return pPopupConfirm;
	}

	CC_SAFE_DELETE(pPopupConfirm);
	return NULL;
}

bool PopupConfirmNode::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundBoard = Sprite::create("popup/panel_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackgroundBoard);
	
	LabelBMFont *pLabelTitle = LabelBMFont::create(m_sTitle.c_str(), "fonts/font_title-popup.fnt", 420.0f, TextHAlignment::CENTER);
	pLabelTitle->setAnchorPoint(Point(0.5f, 1.0f));
	pLabelTitle->setPosition(Point(320.0f, 630.0f));
	this->addChild(pLabelTitle);

	LabelBMFont *pLabelDescription= LabelBMFont::create(m_sDescription.c_str(), "fonts/font-small-green-alert.fnt", 420.0f, TextHAlignment::CENTER);
	pLabelDescription->setAnchorPoint(Point(0.5f, 1.0f));
	if (pLabelTitle->getContentSize().height <= 70)
	{
		pLabelDescription->setPosition(Point(320.0f, 560.0f));
	}
	else
	{
		pLabelDescription->setPosition(Point(320.0f, 630.0f - pLabelTitle->getContentSize().height));
	}

	pLabelDescription->setScale(1.2f);
	this->addChild(pLabelDescription);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	if (m_popupConfirmType == PopupConfirmType::eYesNo) 
	{
		Sprite* pButtonYesSprite = Sprite::create("popup/btn_yes.png");
		ButtonNode* pButtonYesNode = ButtonNode::createButtonSprite(pButtonYesSprite, CC_CALLBACK_1(PopupConfirmNode::clickYes, this));
		pButtonYesNode->setPosition(Point(207.0f, 370.0f));

		Sprite* pButtonNoSprite = Sprite::create("popup/btn_no.png");
		ButtonNode* pButtonNoNode = ButtonNode::createButtonSprite(pButtonNoSprite, CC_CALLBACK_1(PopupConfirmNode::clickNo, this));
		pButtonNoNode->setPosition(Point(443.0f, 370.0f));
		pButtonManagerNode->addButtonNode(pButtonYesNode);
		pButtonManagerNode->addButtonNode(pButtonNoNode);
	}
	else if (m_popupConfirmType == PopupConfirmType::eOK)
	{
		Sprite* pButtonOkSprite = Sprite::create("popup/ok.png");
		ButtonNode* pButtonOkNode = ButtonNode::createButtonSprite(pButtonOkSprite, CC_CALLBACK_1(PopupConfirmNode::clickOk, this));
		pButtonOkNode->setPosition(Point(320.0f, 370.0f));
		pButtonManagerNode->addButtonNode(pButtonOkNode);
	}
	else if (m_popupConfirmType == PopupConfirmType::eConnectFacebook)
	{
		Sprite* pButtonCloseSprite = Sprite::create("popup/btn_close.png");
		ButtonNode* pButtonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(PopupConfirmNode::clickNo, this));
		pButtonCloseNode->setPosition(Point(570.0f, 647.0f));
		pButtonManagerNode->addButtonNode(pButtonCloseNode);

		Sprite* pButtonOkSprite = Sprite::create("PanelSetting/btn_login.png");
		ButtonNode* pButtonOkNode = ButtonNode::createButtonSprite(pButtonOkSprite, CC_CALLBACK_1(PopupConfirmNode::clickLoginFacebook, this));
		pButtonOkNode->setPosition(Point(320.0f, 370.0f));
		pButtonManagerNode->addButtonNode(pButtonOkNode);
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
#endif

	return true;
}

bool PopupConfirmNode::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void PopupConfirmNode::clickYes(cocos2d::Object* pSender)
{
	switch (m_popupConfirmActionType)
	{
		case PopupConfirmActionType::eResetGame :
		{
			this->removeAllChildren();

			WaitingNode* pWaitingNode = WaitingNode::createLayout("Loading...");
			pWaitingNode->setTag(1000);
			this->getParent()->addChild(pWaitingNode, 1000);

			auto actionRunProcess = CallFunc::create(this, callfunc_selector(PopupConfirmNode::runResetGame));
			auto actionFinishWord = CallFunc::create(this, callfunc_selector(PopupConfirmNode::finishResetGame));
			this->runAction(Sequence::create(DelayTime::create(0.2f), actionRunProcess, actionFinishWord, NULL));

			break;
		}
		case PopupConfirmActionType::eQuitGameCustomMode :
		{
			Breadcrumb::getInstance()->resetSceneNodeToMainMenu();
			MainMenuScene* pMainMenuScene = MainMenuScene::create();
			Director::getInstance()->replaceScene(pMainMenuScene);
			break;
		}
		case PopupConfirmActionType::eExitGame :
		{
			Director::getInstance()->end();
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				exit(0);
			#endif
		}
		default:
			break;
	}
}

void PopupConfirmNode::clickNo(cocos2d::Object* pSender)
{
	this->getParent()->removeChild(this);
}

void PopupConfirmNode::clickOk(cocos2d::Object* pSender)
{
	switch (m_popupConfirmActionType)
	{
		case PopupConfirmActionType::eFlashcardCollection :
		{
			Breadcrumb::getInstance()->getSceneModePopBack();
			FlashCardCollectionScene* scene = FlashCardCollectionScene::create();
			Director::getInstance()->replaceScene(scene);

			break;
		}
		default:
			this->getParent()->removeChild(this);
	}
}

void PopupConfirmNode::clickLoginFacebook(cocos2d::Object* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (!FacebookManager::getInstance()->isNetworkOnline())
	{
		PopupConfirmNode* pPopupConfirmNode = PopupConfirmNode::createLayout("CONNECTION FAIL", "No internet connection was found. Please try later", PopupConfirmActionType::eNone, PopupConfirmType::eOK);
		this->getParent()->addChild(pPopupConfirmNode);	
	}
	else
	{
		UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 0);
		SystemEventHandle::getInstance()->onStartConnectFacebook();
	}
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif

	this->getParent()->removeChild(this);
}

void PopupConfirmNode::runResetGame()
{
	UserDefault::getInstance()->setIntegerForKey("InitDatabase", 0);
	UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 0);

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		FacebookManager::getInstance()->logoutByMode();
	#endif

	if (InitDatabase::getInstance()->resetDatabase())
	{
		WordlMapConfig worldMapConfig = GameConfigManager::getInstance()->GetWordlMapConfig();
		WordlMapConfig::WordMapChapterConfig worldMapChapterConfig = worldMapConfig.m_WorlMapChapterConfigs[0];

		UserInfo userInfoNew = UserTable::getInstance()->getUserInfo();
		userInfoNew.sCurrentChapterId = worldMapChapterConfig.m_sChapterId;
		userInfoNew.iCurrentLevel = 1;
		UserTable::getInstance()->updateUser(userInfoNew);

		// Create data for one chapter
		std::vector<std::string> wordList;
		std::vector<int> mapLevels;
		GameConfigManager::getInstance()->GenerateWordsForNewChapter(worldMapChapterConfig.m_sChapterId, wordList, mapLevels);

		if(InitDatabase::getInstance()->createDataChapterAndLevel(worldMapChapterConfig.m_sChapterId, wordList, mapLevels))
		{
			UserDefault::getInstance()->setIntegerForKey("InitDatabase", 1);
			SystemEventHandle::getInstance()->onStartSyncGame(true);
		}
	}
}

void PopupConfirmNode::finishResetGame()
{
	this->getParent()->removeChildByTag(1000);
	this->getParent()->removeChild(this);
}

void PopupConfirmNode::update(float dt)
{
	m_iTimeCountDown -= dt;
	if (m_iTimeCountDown < 0)
	{
		m_iTimeCountDown = 0;
		m_pLabelClockMiniGame->setString(formatHMSToDisplay((int)m_iTimeCountDown).getCString());
		m_pLabelClockMiniGame->setString("MINI GAME");
	}
	else
	{
		m_pLabelClockMiniGame->setString(formatHMSToDisplay((int)m_iTimeCountDown).getCString());
	}
	
}

void PopupConfirmNode::addClockMiniGame(const float& iTimeCountDown)
{
	m_iTimeCountDown = iTimeCountDown;
	m_pLabelClockMiniGame = LabelBMFont::create(formatHMSToDisplay(m_iTimeCountDown).getCString(), "fonts/font_score.fnt");
	m_pLabelClockMiniGame->setPosition(Point(320.0f, 443.0f));
	m_pLabelClockMiniGame->setScale(1.2f);
	this->addChild(m_pLabelClockMiniGame);
	this->scheduleUpdate();
}
