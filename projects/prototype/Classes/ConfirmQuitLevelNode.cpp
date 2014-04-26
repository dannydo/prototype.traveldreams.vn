#include "ConfirmQuitLevelNode.h"
#include "ButtonManagerNode.h"
#include "WorldMapScene.h"
#include "HelloWorldScene.h"
#include "GetMoreLifeNode.h"

USING_NS_CC;

ConfirmQuitLevelNode::~ConfirmQuitLevelNode ()
{
}

ConfirmQuitLevelNode* ConfirmQuitLevelNode::createLayout(const int& iLevel)
{
	ConfirmQuitLevelNode* pConfirmQuitLevel = new ConfirmQuitLevelNode();
	pConfirmQuitLevel->m_iLevel = iLevel;

	if(pConfirmQuitLevel->init())
	{
		pConfirmQuitLevel->autorelease();
		return pConfirmQuitLevel;
	}

	CC_SAFE_DELETE(pConfirmQuitLevel);
	return NULL;
}

bool ConfirmQuitLevelNode::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundBoard = Sprite::create("Target-End-Game/panel-level_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	Sprite* imageHeart = Sprite::create("Target-End-Game/break_heart.png");
	imageHeart->setPosition(Point(320.0f, 610.0f));
	this->addChild(imageHeart);
	
	LabelBMFont *pLabelTitle = LabelBMFont::create("QUIT LEVEL?", "fonts/font_title-popup.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(320.0f, 745.0f));
	this->addChild(pLabelTitle);

	LabelBMFont *pLabelDecription = LabelBMFont::create("You will lose a life!?", "fonts/font-small-green-alert.fnt");
	pLabelDecription->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelDecription->setPosition(Point(320.0f, 505.0f));
	this->addChild(pLabelDecription);

	char sLevel[20];
	sprintf(sLevel, "Level %d", m_iLevel);
	LabelBMFont *pLabelLevel = LabelBMFont::create(sLevel, "fonts/font-bechic.fnt");
	pLabelLevel->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelLevel->setPosition(Point(320.0f, 870.0f));
	this->addChild(pLabelLevel);

	Sprite* pButtonYesSprite = Sprite::create("Target-End-Game/btn_yes.png");
	ButtonNode* pButtonYesNode = ButtonNode::createButtonSprite(pButtonYesSprite, CC_CALLBACK_1(ConfirmQuitLevelNode::clickYes, this));
	pButtonYesNode->setPosition(Point(207.0f, 382.0f));

	Sprite* pButtonNoSprite = Sprite::create("Target-End-Game/btn_no.png");
	ButtonNode* pButtonNoNode = ButtonNode::createButtonSprite(pButtonNoSprite, CC_CALLBACK_1(ConfirmQuitLevelNode::clickNo, this));
	pButtonNoNode->setPosition(Point(440.0f, 382.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* pButtonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(ConfirmQuitLevelNode::clickNo, this));
	pButtonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(pButtonYesNode);
	pButtonManagerNode->addButtonNode(pButtonNoNode);
	pButtonManagerNode->addButtonNode(pButtonCloseNode);
	this->addChild(pButtonManagerNode);

	return true;
}

bool ConfirmQuitLevelNode::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void ConfirmQuitLevelNode::clickYes(cocos2d::Object* pSender)
{
	this->createLayoutLevelFail();
}

void ConfirmQuitLevelNode::clickNo(cocos2d::Object* pSender)
{
	this->getParent()->removeChild(this);
}

void ConfirmQuitLevelNode::clickRetry(cocos2d::Object* pSender)
{
	UserTable::getInstance()->updateLife(0);
	if(UserTable::getInstance()->getUserInfo().iLife > 0)
	{
		GameWordManager* pGameWordManager = GameWordManager::getInstance();
		pGameWordManager->RetryCurrentLevel();

		CCScene *pGameScene = HelloWorld::createScene();
		CCDirector::getInstance()->replaceScene(pGameScene);
	}
	else
	{
		GetMoreLifeNode* pGetMoreLife = GetMoreLifeNode::create();
		pGetMoreLife->setGetMoreLifeType(GetMoreLifeType::eGoToMainMenu);
		this->addChild(pGetMoreLife);
	}
}

void ConfirmQuitLevelNode::clickWordMap(cocos2d::Object* pSender)
{
	Breadcrumb::getInstance()->resetSceneNodeToMainMenu();
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kMainMenu);

	WorldMapScene* wordMap = WorldMapScene::create();
	Director::getInstance()->replaceScene(wordMap);
}

void ConfirmQuitLevelNode::createLayoutLevelFail()
{
	UserTable::getInstance()->updateLife(1);

	this->removeAllChildren();
	
	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundBoard = Sprite::create("Target-End-Game/panel-level_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	Sprite* pLevelFailImage3 = Sprite::create("Target-End-Game/text_level_fail.png");
	pLevelFailImage3->setPosition(Point(320.0f, 747.0f));
	pLevelFailImage3->setScale(0.85);
	this->addChild(pLevelFailImage3);

	LabelBMFont *pLabelDecription = LabelBMFont::create("You pressed", "fonts/font_small_alert.fnt");
	pLabelDecription->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelDecription->setPosition(Point(320.0f, 650.0f));
	pLabelDecription->setScale(1.2f);
	this->addChild(pLabelDecription);

	LabelBMFont *pLabelDecription1 = LabelBMFont::create("the quit button", "fonts/font_small_alert.fnt");
	pLabelDecription1->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelDecription1->setPosition(Point(320.0f, 600.0f));
	pLabelDecription1->setScale(1.2f);
	this->addChild(pLabelDecription1);

	char sLevel[20];
	sprintf(sLevel, "Level %d", m_iLevel);
	LabelBMFont *pLabelLevel = LabelBMFont::create(sLevel, "fonts/font-bechic.fnt");
	pLabelLevel->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelLevel->setPosition(Point(320.0f, 870.0f));
	this->addChild(pLabelLevel);

	Sprite* pButtonRetrySprite = Sprite::create("Target-End-Game/btn_retry.png");
	ButtonNode* pButtonRetryNode = ButtonNode::createButtonSprite(pButtonRetrySprite, CC_CALLBACK_1(ConfirmQuitLevelNode::clickRetry, this));
	pButtonRetryNode->setPosition(Point(320.0f, 490.0f));

	Sprite* pButtonWordMapSprite = Sprite::create("Target-End-Game/btn_world_map.png");
	ButtonNode* pButtonWordMapNode = ButtonNode::createButtonSprite(pButtonWordMapSprite, CC_CALLBACK_1(ConfirmQuitLevelNode::clickWordMap, this));
	pButtonWordMapNode->setPosition(Point(320.0f, 382.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* pButtonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(ConfirmQuitLevelNode::clickWordMap, this));
	pButtonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(pButtonRetryNode);
	pButtonManagerNode->addButtonNode(pButtonWordMapNode);
	pButtonManagerNode->addButtonNode(pButtonCloseNode);
	this->addChild(pButtonManagerNode);
}