#include "PopupConfirmNode.h"
#include "ButtonManagerNode.h"
#include "MainMenuScene.h"
#include "WaitingNode.h"
#include "GameConfigManager.h"
#include "Database\InitDatabase.h"
#include "SystemEventHandle.h"

USING_NS_CC;

PopupConfirmNode::~PopupConfirmNode ()
{
}

PopupConfirmNode* PopupConfirmNode::createLayout(const std::string& sTitle, const PopupConfirmType& popupConfirmType)
{
	PopupConfirmNode* pPopupConfirm = new PopupConfirmNode();
	pPopupConfirm->m_sTitle = sTitle;
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
	
	LabelBMFont *pLabelTitle = LabelBMFont::create(m_sTitle.c_str(), "fonts/font_title-popup.fnt", 410.0f, TextHAlignment::CENTER);
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(320.0f, 527.0f));
	this->addChild(pLabelTitle);

	Sprite* pButtonYesSprite = Sprite::create("popup/btn_yes.png");
	ButtonNode* pButtonYesNode = ButtonNode::createButtonSprite(pButtonYesSprite, CC_CALLBACK_1(PopupConfirmNode::clickYes, this));
	pButtonYesNode->setPosition(Point(207.0f, 370.0f));

	Sprite* pButtonNoSprite = Sprite::create("popup/btn_no.png");
	ButtonNode* pButtonNoNode = ButtonNode::createButtonSprite(pButtonNoSprite, CC_CALLBACK_1(PopupConfirmNode::clickNo, this));
	pButtonNoNode->setPosition(Point(443.0f, 370.0f));

	Sprite* pButtonCloseSprite = Sprite::create("popup/btn_close.png");
	ButtonNode* pButtonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(PopupConfirmNode::clickNo, this));
	pButtonCloseNode->setPosition(Point(570.0f, 647.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(pButtonYesNode);
	pButtonManagerNode->addButtonNode(pButtonNoNode);
	pButtonManagerNode->addButtonNode(pButtonCloseNode);
	this->addChild(pButtonManagerNode);

	return true;
}

bool PopupConfirmNode::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void PopupConfirmNode::clickYes(cocos2d::Object* pSender)
{
	switch (m_popupConfirmType)
	{
		case PopupConfirmType::eResetGame :
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
		case PopupConfirmType::eQuitGameCustomMode :
		{
			Breadcrumb::getInstance()->resetSceneNodeToMainMenu();
			MainMenuScene* pMainMenuScene = MainMenuScene::create();
			Director::getInstance()->replaceScene(pMainMenuScene);
			break;
		}
		default:
			break;
	}
}

void PopupConfirmNode::clickNo(cocos2d::Object* pSender)
{
	this->getParent()->removeChild(this);
}

void PopupConfirmNode::runResetGame()
{
	UserDefault::getInstance()->setIntegerForKey("InitDatabase", 0);

	if (InitDatabase::getInstance()->resetDatabase())
	{
		WordlMapConfig worldMapConfig = GameConfigManager::getInstance()->GetWordlMapConfig();
		WordlMapConfig::WordMapChapterConfig worldMapChapterConfig = worldMapConfig.m_WorlMapChapterConfigs[0];

		UserInfo userInfoNew = UserTable::getInstance()->getUserInfo();
		userInfoNew.sCurrentChapterId = worldMapChapterConfig.m_sChapterId;
		userInfoNew.iCurrentLevel = 1;
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		userInfoNew.sFacebookToken  = FacebookManager::getInstance()->getAccessToken();
		#endif
		UserTable::getInstance()->updateUser(userInfoNew);

		// Create data for one chapter
		std::vector<std::string> wordList;
		std::vector<int> mapLevels;
		GameConfigManager::getInstance()->GenerateWordsForLevels(worldMapChapterConfig.m_sChapterId, wordList, mapLevels);

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