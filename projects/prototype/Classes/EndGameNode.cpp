#include "EndGameNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "DictionaryNode.h"

USING_NS_CC;

EndGameNode* EndGameNode::createLayoutLose(const int& iScore, char* pMainWord, const int& iCurrentLevel)
{
	EndGameNode* pEndGameNode = new EndGameNode();
	if(pEndGameNode->initLose(iScore, pMainWord))
	{
		pEndGameNode->m_iCurrentLevel = iCurrentLevel;
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

EndGameNode* EndGameNode::createLayoutWin(const int& iScore, char* pMainWord, std::vector<char*> pSubWord, const int& iCurrentLevel)
{
	EndGameNode* pEndGameNode = new EndGameNode();
	if(pEndGameNode->initWin(iScore, pMainWord, pSubWord))
	{			
		pEndGameNode->m_iCurrentLevel = iCurrentLevel;
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

bool EndGameNode::initLose(const int& iScore, char* pMainWord)
{
	if (!this->init())
	{
		return false;
	}

	Sprite* pBackgroundWin = Sprite::create("Target-End-Game/Fail_Board.png");
	pBackgroundWin->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 80));
	this->addChild(pBackgroundWin);

	std::vector<char*> subWord;
	Node* pWordNode = this->generateLayoutWord(pMainWord, subWord);
	pWordNode->setPosition(Point(this->getContentSize().width/2.0f - pWordNode->getContentSize().width/2.0f, 555));
	this->addChild(pWordNode);

	Node* pScoreNode = this->generateLayoutScore(iScore);
	pScoreNode->setPosition(Point(382 - pScoreNode->getContentSize().width/2.0f, 455));
	this->addChild(pScoreNode);

	MenuItemImage* pRetryLevel = MenuItemImage::create(
		"Target-End-Game/Retry_Button.png",
		"Target-End-Game/Retry_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuRetryLevelCallBack, this));

	Menu* pMenuLose = Menu::create(pRetryLevel, NULL);
	pMenuLose->setPosition(Point(this->getContentSize().width/2.0f, 385));
	this->addChild(pMenuLose);

	return true;
}

bool EndGameNode::initWin(const int& iScore, char* pMainWord, std::vector<char*> pSubWord)
{
	if (!this->init())
	{
		return false;
	}

	Sprite* pBackgroundShine = Sprite::create("Target-End-Game/Shine.png");
	pBackgroundShine->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height - 160));
	this->addChild(pBackgroundShine);

	Sprite* pBackgroundWin = Sprite::create("Target-End-Game/Win_Board.png");
	pBackgroundWin->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 80));
	this->addChild(pBackgroundWin);

	m_pStarNode = this->generateLayoutStar();
	m_pStarNode->setPosition(Point(228, 728));
	this->addChild(m_pStarNode);

	Node* pWordNode = this->generateLayoutWord(pMainWord, pSubWord);
	pWordNode->setPosition(Point(this->getContentSize().width/2.0f - pWordNode->getContentSize().width/2.0f, 555));
	this->addChild(pWordNode);

	Node* pScoreNode = this->generateLayoutScore(iScore);
	pScoreNode->setPosition(Point(382 - pScoreNode->getContentSize().width/2.0f, 455));
	this->addChild(pScoreNode);

	MenuItemImage* pNextLevel = MenuItemImage::create(
		"Target-End-Game/Next_Button.png",
		"Target-End-Game/Next_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuNextLevelCallBack, this));

	Menu* pMenuWin = Menu::create(pNextLevel, NULL);
	pMenuWin->setPosition(Point(this->getContentSize().width/2.0f, 385));
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

	Menu* pMenuClose = Menu::create(pCloseItem, NULL);
	pMenuClose->setPosition(Point(561, 790));
	this->addChild(pMenuClose, 10);

	MenuItemImage* pDictItem = MenuItemImage::create(
		"Target-End-Game/Dict_Button.png",
		"Target-End-Game/Dict_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuOpenDictCallBack, this));

	Menu* pMenuDict = Menu::create(pDictItem, NULL);
	pMenuDict->setPosition(Point(150, 380));
	this->addChild(pMenuDict, 10);

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
	pNode->setContentSize(CCSizeMake(width, height));

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

Node* EndGameNode::generateLayoutWord(char* mainWord, std::vector<char*> subWord)
{
	Node* pNode = Node::create();

	pNode->setContentSize(CCSizeMake(345, 64));

	Label* pMainWord = Label::createWithTTF(mainWord, "fonts/ARLRDBD.ttf", 34);
	pMainWord->setColor(ccc3(0, 255, 255));
	pMainWord->setPosition(Point(pNode->getContentSize().width/2.0f-pMainWord->getContentSize().width/2.0f, 30));
	pNode->addChild(pMainWord);
	
	int iIndex=0;
	while(!subWord.empty())
	{
		Label* pSubWord = Label::createWithTTF(subWord.back(), "fonts/ARLRDBD.ttf", 24);
		pSubWord->setColor(ccc3(0, 0, 0));

		Point point;
		if (iIndex == 0)
		{
			point = Point(-pSubWord->getContentSize().width/2.0f, 0);
		}
		else if (iIndex == 1)
		{
			point = Point(pNode->getContentSize().width/2.0f-pSubWord->getContentSize().width/2.0f, 0);
		}
		else if (iIndex == 2)
		{
			point = Point(pNode->getContentSize().width - pSubWord->getContentSize().width/2.0f - 20, 0);
		}

		subWord.pop_back();
		iIndex++;
		pSubWord->setPosition(point);
		pNode->addChild(pSubWord);
	}

	return pNode;
}

void EndGameNode::menuNextLevelCallBack()
{
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->GenerateWordForNewLevel();

	CCScene *pGameScene = HelloWorld::createScene(m_iCurrentLevel+1);
	CCDirector::getInstance()->replaceScene(pGameScene);
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