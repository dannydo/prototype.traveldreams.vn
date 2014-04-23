#include "EndGameCustomModeNode.h"
#include "MainMenuScene.h"
#include "HelloWorldScene.h"

USING_NS_CC;

EndGameCustomModeNode* EndGameCustomModeNode::createLayout(const int& iStage, const int& iTotalWord, const int& iDurationSecond)
{
	EndGameCustomModeNode* pEndGameNode = new EndGameCustomModeNode();
	pEndGameNode->m_iStage = iStage;
	pEndGameNode->m_iTotalWord = iTotalWord;
	pEndGameNode->m_iDurationSecond = iDurationSecond;

	if(pEndGameNode->init())
	{
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

bool EndGameCustomModeNode::init()
{
	if (!Node::init())
	{
		return false;
	}
	
	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640, 960));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	//m_pSpriteBatchNode = SpriteBatchNode::create("Target-End-Game/TargetEndgame.pvr.ccz");
	//SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Target-End-Game/TargetEndgame.plist");
	//this->addChild(m_pSpriteBatchNode);

	Sprite* pBackgroundBoard = Sprite::create("Target-End-Game/panel-level_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	Sprite* pButtonReplayGameSprite = Sprite::create("Target-End-Game/btn_replay.png");
	ButtonNode* buttonReplayNode = ButtonNode::createButtonSprite(pButtonReplayGameSprite, CC_CALLBACK_1(EndGameCustomModeNode::menuRetryLevelCallBack, this));
	buttonReplayNode->setPosition(Point(225.0f, 279.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(EndGameCustomModeNode::menuCloseCallBack, this));
	buttonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(buttonCloseNode);
	pButtonManagerNode->addButtonNode(buttonReplayNode);
	this->addChild(pButtonManagerNode);

	this->generateLayoutStart();

	LabelTTF *pLabelGameOver = LabelTTF::create("GAME OVER", "fonts/UTM Cookies.ttf", 30.0f);
	pLabelGameOver->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelGameOver->setPosition(Point(320.0f, 925.0f));
	this->addChild(pLabelGameOver);

	char sStage[20];
	sprintf(sStage, "STAGE %d", m_iStage);
	LabelTTF *pLabelStage = LabelTTF::create(sStage, "fonts/UTM Cookies.ttf", 40.0f);
	pLabelStage->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelStage->setPosition(Point(320.0f, 730.0f));
	this->addChild(pLabelStage);

	char sTotalWord[20];
	sprintf(sTotalWord, "Total Word: %d", m_iTotalWord);
	LabelTTF *pLabelTotalWord = LabelTTF::create(sTotalWord, "fonts/UTM Cookies.ttf", 30.0f);
	pLabelTotalWord->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelTotalWord->setPosition(Point(120.0f, 600.0f));
	this->addChild(pLabelTotalWord);

	char sDuration[30];
	sprintf(sDuration, "Game Duration %s", this->formatSecondsToDiaplay(m_iDurationSecond).getCString());
	LabelTTF *pLabelDuration = LabelTTF::create(sDuration, "fonts/UTM Cookies.ttf", 30.0f);
	pLabelDuration->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelDuration->setPosition(Point(120.0f, 560.0f));
	this->addChild(pLabelDuration);

	return true;
}

void EndGameCustomModeNode::generateLayoutStart()
{
	for(int iIndex=0; iIndex<3; iIndex++) 
	{
		Sprite* pStarPurpleImage = Sprite::createWithSpriteFrameName("star_purple.png");
		pStarPurpleImage->setPosition(Point(200.0f + iIndex*108.0f, 823.0f + iIndex*5));
		m_pSpriteBatchNode->addChild(pStarPurpleImage);
	}
}

void EndGameCustomModeNode::addYellowStar(const int& iYellowStar)
{
	m_iYellowStar = iYellowStar;
	m_iCountYellowStar = 0;

	this->sequenceUpdateStar();
}

void EndGameCustomModeNode::sequenceUpdateStar()
{
	auto actionUpdateStar = CallFunc::create(this, callfunc_selector(EndGameCustomModeNode::updateStar));
	auto delay = DelayTime::create(0.5f);
	if (m_iCountYellowStar < m_iYellowStar) {
		this->runAction(Sequence::create(delay->clone(), actionUpdateStar, NULL));
	}
}

void EndGameCustomModeNode::updateStar()
{
	if (m_iCountYellowStar < m_iYellowStar) {
		Sprite* pStarYellowImage = Sprite::createWithSpriteFrameName("star_yellow.png");
		pStarYellowImage->setPosition(Point(200.0f + m_iCountYellowStar*108.0f, 823.0f + m_iCountYellowStar*5));
		m_pSpriteBatchNode->addChild(pStarYellowImage);
		m_iCountYellowStar++;
		this->sequenceUpdateStar();
	}
}

void EndGameCustomModeNode::menuRetryLevelCallBack(Object* sender)
{
	auto scene = HelloWorld::createScene( _GMT_TIME_MODE_);
	Director::getInstance()->replaceScene(scene);
}

void EndGameCustomModeNode::menuCloseCallBack(Object* sender)
{
	Breadcrumb::getInstance()->getSceneModePopBack();
	MainMenuScene* pMainMenu =  MainMenuScene::create();
	Director::getInstance()->replaceScene(pMainMenu);
}

String EndGameCustomModeNode::formatSecondsToDiaplay(const int& iSeconds)
{
	String str = "";
	str.appendWithFormat("%d", iSeconds/60);

	str.appendWithFormat("%s", ":");

	if(iSeconds%60 < 10)
		str.append("0");	
	str.appendWithFormat("%d", iSeconds%60);

	return str;
}