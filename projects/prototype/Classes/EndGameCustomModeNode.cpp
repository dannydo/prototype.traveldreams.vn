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

	Sprite* pBackgroundBoard = Sprite::create("Target-End-Game/panel-level_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	Sprite* pButtonReplayGameSprite = Sprite::create("Target-End-Game/btn_replay.png");
	ButtonNode* buttonReplayNode = ButtonNode::createButtonSprite(pButtonReplayGameSprite, CC_CALLBACK_1(EndGameCustomModeNode::menuRetryLevelCallBack, this));
	buttonReplayNode->setPosition(Point(320.0f, 279.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(EndGameCustomModeNode::menuCloseCallBack, this));
	buttonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(buttonCloseNode);
	pButtonManagerNode->addButtonNode(buttonReplayNode);
	this->addChild(pButtonManagerNode);

	this->generateLayoutStart();

	LabelBMFont *pLabelGameOver = LabelBMFont::create("GAME OVER", "fonts/font-bechic.fnt");
	pLabelGameOver->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelGameOver->setPosition(Point(320.0f, 870.0f));
	this->addChild(pLabelGameOver);

	char sStage[20];
	sprintf(sStage, "STAGE %d", m_iStage);
	LabelBMFont *pLabelStage = LabelBMFont::create(sStage, "fonts/font_title-popup.fnt");
	pLabelStage->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelStage->setPosition(Point(320.0f, 670.0f));
	this->addChild(pLabelStage);

	char sTotalWord[20];
	sprintf(sTotalWord, "Total Word: %d", m_iTotalWord);
	LabelBMFont *pLabelTotalWord = LabelBMFont::create(sTotalWord, "fonts/font_score.fnt");
	pLabelTotalWord->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelTotalWord->setPosition(Point(150.0f, 550.0f));
	this->addChild(pLabelTotalWord);

	char sDuration[30];
	sprintf(sDuration, "Game Duration %s", this->formatSecondsToDiaplay(m_iDurationSecond).getCString());
	LabelBMFont *pLabelDuration = LabelBMFont::create(sDuration, "fonts/font_score.fnt");
	pLabelDuration->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelDuration->setPosition(Point(150.0f, 500.0f));
	this->addChild(pLabelDuration);

	return true;
}

void EndGameCustomModeNode::generateLayoutStart()
{
	for(int iIndex=0; iIndex<3; iIndex++) 
	{
		Sprite* pStarPurpleImage = Sprite::create("Target-End-Game/star_target_small.png");
		pStarPurpleImage->setPosition(Point(220.0f + iIndex*100.0f, 760));
		if (iIndex == 1)
			pStarPurpleImage->setScale(1.4f);
		this->addChild(pStarPurpleImage);
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
		Sprite* pStarYellowImage = Sprite::create("Target-End-Game/star_win_small.png");
		pStarYellowImage->setPosition(Point(210.0f + m_iCountYellowStar*108.0f, 760));

		if (m_iCountYellowStar == 1)
			pStarYellowImage->setScale(1.4f);

		this->addChild(pStarYellowImage);
		m_iCountYellowStar++;
		this->sequenceUpdateStar();
	}
}

void EndGameCustomModeNode::menuRetryLevelCallBack(Object* sender)
{
	auto timeModeConfig = &GameConfigManager::getInstance()->GetTimeModeDemoConfig();
		//m_GameBoardManager.GetLevelConfig();
	//GameConfigManager::getInstance()->GetTimeModeDemoConfig();	
	GameWordManager::getInstance()->GenerateWordForNewLevelOfTimeMode(timeModeConfig);

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