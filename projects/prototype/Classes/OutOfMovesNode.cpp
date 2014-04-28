#include "OutOfMovesNode.h"
#include "GameConfigManager.h"
#include "SettingMenuNode.h"
#include "LevelMapScene.h"

USING_NS_CC;

OutOfMovesNode* OutOfMovesNode::createLayout(const int& iScore, const Word& mainWord, const int& iCurrentLevel, const std::string sChapterId)
{
	OutOfMovesNode* pEndGameNode = new OutOfMovesNode();
	pEndGameNode->m_iCurrentLevel = iCurrentLevel;
	pEndGameNode->m_mainWord = mainWord;
	pEndGameNode->m_iScore = iScore;
	pEndGameNode->m_sChapterId = sChapterId;

	if(pEndGameNode->init())
	{
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

bool OutOfMovesNode::init()
{
	if (!Node::init())
	{
		return false;
	}
	
	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundBoard = Sprite::create("Target-End-Game/panel-level_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	char sLevel[20];
	int iCalLevel = GameConfigManager::getInstance()->CountLevelOfPreviousChapters(m_sChapterId);
	sprintf(sLevel, "Level %d", m_iCurrentLevel + iCalLevel);
	LabelBMFont *pLabelLevel = LabelBMFont::create(sLevel, "fonts/font-bechic.fnt");
	pLabelLevel->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelLevel->setPosition(Point(320.0f, 870.0f));
	this->addChild(pLabelLevel);

	Sprite* pLevelFailImage3 = Sprite::create("Target-End-Game/text_out_moves.png");
	pLevelFailImage3->setPosition(Point(320.0f, 752.0f));
	this->addChild(pLevelFailImage3);

	Sprite* pBorderMoveImage = Sprite::create("Target-End-Game/frame_out_moves.png");
	pBorderMoveImage->setPosition(Point(320.0f, 645.0f));
	this->addChild(pBorderMoveImage);

	Sprite* pIconMoveImage = Sprite::create("Target-End-Game/icon_moves.png");
	pIconMoveImage->setPosition(Point(198.0f, 645.0f));
	this->addChild(pIconMoveImage);

	LabelBMFont *pLabelTitle = LabelBMFont::create("5 MOVES", "fonts/font_title-popup.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(373.0f, 645.0f));
	this->addChild(pLabelTitle);

	Sprite* pButtonBuySprite = Sprite::create("Target-End-Game/btn_playon.png");
	ButtonNode* buttonBuyNode = ButtonNode::createButtonSprite(pButtonBuySprite, CC_CALLBACK_1(OutOfMovesNode::clickBuy, this));
	buttonBuyNode->setPosition(Point(320.0f, 500.0f));
	
	Sprite* pButtonGiveUpSprite = Sprite::create("Target-End-Game/btn_giveup.png");
	ButtonNode* buttonGiveUpNode = ButtonNode::createButtonSprite(pButtonGiveUpSprite, CC_CALLBACK_1(OutOfMovesNode::clickGiveUp, this));
	buttonGiveUpNode->setPosition(Point(320.0f, 390.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(OutOfMovesNode::clickGiveUp, this));
	buttonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(buttonBuyNode);
	pButtonManagerNode->addButtonNode(buttonGiveUpNode);
	pButtonManagerNode->addButtonNode(buttonCloseNode);
	this->addChild(pButtonManagerNode);

	return true;
}


void OutOfMovesNode::clickBuy(Object* sender)
{
	
}

void OutOfMovesNode::clickGiveUp(Object* sender)
{
	Breadcrumb::getInstance()->getSceneModePopBack();
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	pLevelMap->getLayer()->showPopupEndGameLose(m_iScore, m_mainWord, m_iCurrentLevel, m_sChapterId);
	Director::getInstance()->replaceScene(pLevelMap);
}