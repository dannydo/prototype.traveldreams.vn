#include "EndGameNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "DictionaryNode.h"

USING_NS_CC;

EndGameNode* EndGameNode::createLayoutLose(const int& iScore, const Word& pMainWord, const int& iCurrentLevel)
{
	EndGameNode* pEndGameNode = new EndGameNode();	  
	pEndGameNode->m_iCurrentLevel = iCurrentLevel;
	if(pEndGameNode->initLose(iScore, pMainWord))
	{
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

EndGameNode* EndGameNode::createLayoutWin(const int& iScore, const Word& pMainWord, const std::vector<Word>& pSubWord, const int& iCurrentLevel)
{
	EndGameNode* pEndGameNode = new EndGameNode();	  
	pEndGameNode->m_iCurrentLevel = iCurrentLevel;
	if(pEndGameNode->initWin(iScore, pMainWord, pSubWord))
	{			
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

bool EndGameNode::initLose(const int& iScore, const Word& pMainWord)
{
	if (!this->init())
	{
		return false;
	}

	Sprite* pBackgroundFail = Sprite::create("Target-End-Game/Fail_Board.png");
	pBackgroundFail->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 50));
	this->addChild(pBackgroundFail);

	Node* pLevelNode = this->generateLayoutLevel(m_iCurrentLevel);
	pLevelNode->setPosition(Point(365, 840));
	this->addChild(pLevelNode);

	Sprite* pBackgroundTargetCard = Sprite::create("Target-End-Game/Fail_Card.png");
	pBackgroundTargetCard->setPosition(Point(320, 535));
	this->addChild(pBackgroundTargetCard);

	Label* pLabelMainWord = Label::createWithTTF(pMainWord.m_sWord, "fonts/ARLRDBD.ttf", 32);
	pLabelMainWord->setColor(ccc3(0, 0, 0));
	pLabelMainWord->setAnchorPoint(Point(0.5f, 0.0f));
	pLabelMainWord->setPosition(Point(315, 580));
	this->addChild(pLabelMainWord);

	MenuItemImage* pRetryLevel = MenuItemImage::create(
		"Target-End-Game/Retry_Button.png",
		"Target-End-Game/Retry_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuRetryLevelCallBack, this));

	Menu* pMenuLose = Menu::create(pRetryLevel, NULL);
	pMenuLose->setPosition(Point(this->getContentSize().width/2.0f, 305));
	this->addChild(pMenuLose);

	return true;
}

bool EndGameNode::initWin(const int& iScore, const Word& pMainWord, const std::vector<Word>& pSubWord)
{
	if (!this->init())
	{
		return false;
	}

	Sprite* pBackgroundShine = Sprite::create("Target-End-Game/Shine.png");
	pBackgroundShine->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height - 160));
	this->addChild(pBackgroundShine);

	Sprite* pBackgroundWin = Sprite::create("Target-End-Game/Win_Board.png");
	pBackgroundWin->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 50));
	this->addChild(pBackgroundWin);

	Node* pLevelNode = this->generateLayoutLevel(m_iCurrentLevel);
	pLevelNode->setPosition(Point(365, 840));
	this->addChild(pLevelNode);

	m_pStarNode = this->generateLayoutStar();
	m_pStarNode->setPosition(Point(228, 735));
	this->addChild(m_pStarNode);

	Node* pScoreNode = this->generateLayoutScore(iScore);
	pScoreNode->setPosition(Point(320, 680));

	this->addChild(pScoreNode);

	Sprite* pBackgroundTargetCard = Sprite::create("Target-End-Game/Win_Card.png");
	pBackgroundTargetCard->setPosition(Point(320, 535));
	this->addChild(pBackgroundTargetCard);

	std::string sPath = "FlashCard/";
	sPath.append(pMainWord.m_sFlashCardImage);
	Sprite* pFlashCard = Sprite::create(sPath.c_str());
	pFlashCard->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 31));
	this->addChild(pFlashCard, 10);

	Label* pLabelMainWord = Label::createWithTTF(pMainWord.m_sWord, "fonts/ARLRDBD.ttf", 32);
	pLabelMainWord->setColor(ccc3(0, 0, 0));
	pLabelMainWord->setAnchorPoint(Point(0.5f, 0.0f));
	pLabelMainWord->setPosition(Point(315, 580));
	this->addChild(pLabelMainWord);

	Node* pSubWordNode = this->generateLayoutSubWord(pSubWord);
	pSubWordNode->setPosition(Point(320, 375));
	this->addChild(pSubWordNode);

	MenuItemImage* pNextLevel = MenuItemImage::create(
		"Target-End-Game/Next_Button.png",
		"Target-End-Game/Next_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuNextLevelCallBack, this));

	Menu* pMenuWin = Menu::create(pNextLevel, NULL);
	pMenuWin->setPosition(Point(this->getContentSize().width/2.0f, 305));
	this->addChild(pMenuWin);
	
	return true;
}

bool EndGameNode::init()
{
	if (!Node::init())
	{
		return false;
	}
	
	LayerColor* pBackground = LayerColor::create(ccc4(191, 103, 241, 229));
	pBackground->setContentSize(CCSizeMake(640, 960));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Target-End-Game/Close_Button.png",
		"Target-End-Game/Close_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuCloseCallBack, this));
	pCloseItem->setPosition(Point(561, 790));

	/*
	MenuItemImage* pDictItem = MenuItemImage::create(
		"Target-End-Game/Dict_Button.png",
		"Target-End-Game/Dict_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuOpenDictCallBack, this));
	pDictItem->setPosition(Point(150, 300));
	*/

	Menu* pMenu = Menu::create(pCloseItem, NULL);
	pMenu->setPosition(Point::ZERO);
	this->addChild(pMenu, 10);

	return true;
}

std::vector<int> EndGameNode::generateArrayNumber(int iNumber)
{
	int iTemp = iNumber;
	std::vector<int> arrNumber;
	while(iTemp >= 1)
	{
		arrNumber.push_back(iTemp%10);
		iTemp = iTemp/10.0f;
	}

	return arrNumber;
}

Node* EndGameNode::generateLayoutScore(int iScore)
{
	std::vector<int> arrNumber = this->generateArrayNumber(iScore);
	int width = 0;
	int height;
	Node* pNode = Node::create();
	int iTotal = arrNumber.size();
	
	while(!arrNumber.empty())
	{
		char sFileName[50];
		sprintf(sFileName, "Target-End-Game/NumScore/NumScore_%d.png", arrNumber.back());
		Sprite* sprite = Sprite::create(sFileName);

		height = sprite->getContentSize().height;
		sprite->setPosition(Point(width, height/2.0f));
		width = width + sprite->getContentSize().width - 15;
		pNode->addChild(sprite);
		arrNumber.pop_back();
	}
	pNode->setContentSize(CCSizeMake(width - 18, height));
	pNode->setAnchorPoint(Point(0.5f, 0.5f));
	pNode->setScale(1.2f);

	return pNode;
}

Node* EndGameNode::generateLayoutStar()
{
	Node* pNode = Node::create();

	Sprite* pBlackStar1 = Sprite::create("Target-End-Game/Star_Disable.png");
	pBlackStar1->setPosition(Point::ZERO);
	pNode->addChild(pBlackStar1);

	Sprite* pBlackStar2 = Sprite::create("Target-End-Game/Star_Disable.png");
	pBlackStar2->setPosition(Point(91, 17));
	pNode->addChild(pBlackStar2);

	Sprite* pBlackStar3 = Sprite::create("Target-End-Game/Star_Disable.png");
	pBlackStar3->setPosition(Point(182, 0));
	pNode->addChild(pBlackStar3);
	
	return pNode;
}

void EndGameNode::addYellowStar(int iYellowStar)
{
	m_iYellowStar = iYellowStar;
	m_iCountYellowStar = 0;
	this->sequenceUpdateStar();
}

void EndGameNode::sequenceUpdateStar()
{
	auto actionUpdateStar = CallFunc::create(this, callfunc_selector(EndGameNode::updateStar));
	auto delay = DelayTime::create(0.7f);
	auto actionLoopUpdateStar = CallFunc::create(this, callfunc_selector(EndGameNode::loopUpdateStar));
	this->runAction(Sequence::create(delay->clone(), actionLoopUpdateStar, actionUpdateStar, NULL));
}

void EndGameNode::loopUpdateStar()
{
	if (m_iCountYellowStar < m_iYellowStar)
	{
		this->sequenceUpdateStar();
	}
}

void EndGameNode::updateStar()
{
	if (m_iCountYellowStar < m_iYellowStar)
	{
		Sprite* pYellowStar = Sprite::create("Target-End-Game/Star_Enable.png");
		if(m_iCountYellowStar == 0)
		{
			pYellowStar->setPosition(Point::ZERO);
		}
		else if(m_iCountYellowStar == 1)
		{
			pYellowStar->setPosition(Point(91, 17));
		}
		else if(m_iCountYellowStar == 2)
		{
			pYellowStar->setPosition(Point(182, 0));
		}

		m_pStarNode->addChild(pYellowStar);
		m_iCountYellowStar++;
	}
}

Node* EndGameNode::generateLayoutSubWord(const std::vector<Word>& subWords)
{
	Node* pNode = Node::create();
	int iWidth = 0;
	//while(!subWord.empty())
	for(auto subword : subWords)
	{
		Label* pSubWord = Label::createWithTTF(subword.m_sWord, "fonts/ARLRDBD.ttf", 24);
		pSubWord->setPosition(Point(iWidth, 0));
		pSubWord->setColor(ccc3(0, 0, 0));
		pNode->addChild(pSubWord);

		iWidth += pSubWord->getContentSize().width + 60;
		//subWord.pop_back();
	}

	pNode->setContentSize(CCSizeMake(iWidth-60, 20));
	pNode->setAnchorPoint(Point(0.5f, 0.5f));

	return pNode;
}

Node* EndGameNode::generateLayoutLevel(int iLevel)
{
	std::vector<int> arrNumber = this->generateArrayNumber(iLevel);
	int width = 0;
	int height;
	Node* pNode = Node::create();
	int iTotal = arrNumber.size();
	
	while(!arrNumber.empty())
	{
		char sFileName[50];
		sprintf(sFileName, "Target-End-Game/NumScore/NumScore_%d.png", arrNumber.back());
		Sprite* sprite = Sprite::create(sFileName);

		height = sprite->getContentSize().height;
		sprite->setPosition(Point(width, height/2.0f));
		width = width + sprite->getContentSize().width - 15;
		pNode->addChild(sprite);
		arrNumber.pop_back();
	}
	pNode->setContentSize(CCSizeMake(width - 18, height));
	pNode->setAnchorPoint(Point(0.0f, 0.5f));
	pNode->setScale(1.3f);

	return pNode;
}

void EndGameNode::menuNextLevelCallBack()
{
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->GenerateWordForNewLevel(m_iCurrentLevel+1);

	LevelMapScene* pLevelMap = LevelMapScene::create();
	pLevelMap->getLayer()->showPopupTargetGame(m_iCurrentLevel+1);
	CCDirector::getInstance()->replaceScene(pLevelMap);
}

void EndGameNode::menuRetryLevelCallBack()
{
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->RetryCurrentLevel();

	CCScene *pGameScene = HelloWorld::createScene(m_iCurrentLevel);
	CCDirector::getInstance()->replaceScene(pGameScene);
}

void EndGameNode::menuCloseCallBack()
{
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	Director::getInstance()->replaceScene(pLevelMap);
}

void EndGameNode::menuOpenDictCallBack()
{
	DictionaryNode* pDictionary = DictionaryNode::create();
	this->addChild(pDictionary, 10);
}