#include "ConfirmQuitLevelNode.h"
#include "ButtonManagerNode.h"
#include "WorldMapScene.h"
#include "HelloWorldScene.h"
#include "GetMoreLifeNode.h"
#include "LevelMapScene.h"

USING_NS_CC;

ConfirmQuitLevelNode::~ConfirmQuitLevelNode ()
{
}

ConfirmQuitLevelNode* ConfirmQuitLevelNode::createLayout(const int& iCurrentLevelId, const std::string& sChapterId)
{
	ConfirmQuitLevelNode* pConfirmQuitLevel = new ConfirmQuitLevelNode();
	pConfirmQuitLevel->m_iCurrentLevelId = iCurrentLevelId;
	pConfirmQuitLevel->m_sChapterId = sChapterId;

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

	LabelBMFont *pLabelDecription = LabelBMFont::create("You will lose a life!", "fonts/font-small-green-alert.fnt");
	pLabelDecription->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelDecription->setPosition(Point(320.0f, 505.0f));
	this->addChild(pLabelDecription);

	char sLevel[20];
	int iCalLevel = GameConfigManager::getInstance()->CountLevelOfPreviousChapters(m_sChapterId) + m_iCurrentLevelId;
	sprintf(sLevel, "Level %d", iCalLevel);
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

	/*
	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* pButtonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(ConfirmQuitLevelNode::clickNo, this));
	pButtonCloseNode->setPosition(Point(572.0f, 894.0f));
	*/

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(pButtonYesNode);
	pButtonManagerNode->addButtonNode(pButtonNoNode);
	//pButtonManagerNode->addButtonNode(pButtonCloseNode);
	this->addChild(pButtonManagerNode);

	return true;
}

bool ConfirmQuitLevelNode::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void ConfirmQuitLevelNode::clickYes(cocos2d::Object* pSender)
{
	UserTable::getInstance()->updateLife(1);

	Breadcrumb::getInstance()->getSceneModePopBack();
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	pLevelMap->getLayer()->showPopupQuitLevelFailed(m_iCurrentLevelId, m_sChapterId);
	Director::getInstance()->replaceScene(pLevelMap);
}

void ConfirmQuitLevelNode::clickNo(cocos2d::Object* pSender)
{
	this->getParent()->removeChild(this);
}