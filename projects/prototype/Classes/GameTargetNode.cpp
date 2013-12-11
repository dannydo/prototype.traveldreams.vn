#include "GameTargetNode.h"
#include "MainMenuScene.h"
#include "HelloWorldScene.h"

USING_NS_CC;

GameTargetNode* GameTargetNode::createLayout(char* pMainWord, const int& iCurrentLevel)
{
	GameTargetNode* pGameTargetNode = new GameTargetNode();
	if(pGameTargetNode->initLayout(pMainWord))
	{
		pGameTargetNode->m_iCurrentLevel = iCurrentLevel;
		pGameTargetNode->autorelease();
		return pGameTargetNode;
	}

	CC_SAFE_DELETE(pGameTargetNode);
	return NULL;
}

bool GameTargetNode::initLayout(char* pMainWord)
{
	if (!Node::init())
	{
		return false;
	}

	LayerColor* pBackground = LayerColor::create(ccc4(191, 103, 241, 229));
	pBackground->setContentSize(CCSizeMake(640, 960));
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundWin = Sprite::create("Target-End-Game/Target_Board.png");
	pBackgroundWin->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 80));
	this->addChild(pBackgroundWin);

	Label* pLabelMainWord = Label::createWithTTF(pMainWord, "fonts/ARLRDBD.ttf", 32);
	pLabelMainWord->setColor(ccc3(0, 0, 0));
	pLabelMainWord->setPosition(Point(225, 580));
	this->addChild(pLabelMainWord);

	MenuItemImage* pPlayLevel = MenuItemImage::create(
		"Target-End-Game/Play_Button.png",
		"Target-End-Game/Play_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuPlayLevelCallBack, this));

	Menu* pMenuPlay = Menu::create(pPlayLevel, NULL);
	pMenuPlay->setPosition(Point(this->getContentSize().width/2.0f, 385));
	this->addChild(pMenuPlay);

	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Target-End-Game/Close_Button.png",
		"Target-End-Game/Close_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuCloseCallBack, this));

	Menu* pMenuClose = Menu::create(pCloseItem, NULL);
	pMenuClose->setPosition(Point(561, 790));
	this->addChild(pMenuClose, 10);

	MenuItemImage* pDictItem = MenuItemImage::create(
		"Target-End-Game/Dict_Button.png",
		"Target-End-Game/Dict_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuOpenDictCallBack, this));

	Menu* pMenuDict = Menu::create(pDictItem, NULL);
	pMenuDict->setPosition(Point(150, 380));
	this->addChild(pMenuDict, 10);

	return true;
}

void GameTargetNode::menuPlayLevelCallBack()
{
	CCScene *pGameScene = HelloWorld::createScene(m_iCurrentLevel);
	CCDirector::getInstance()->replaceScene(pGameScene);
}

void GameTargetNode::menuCloseCallBack()
{
	MainMenuScene* mainMenu =  MainMenuScene::create();
	Director::getInstance()->replaceScene(mainMenu);
}

void GameTargetNode::menuOpenDictCallBack()
{

}