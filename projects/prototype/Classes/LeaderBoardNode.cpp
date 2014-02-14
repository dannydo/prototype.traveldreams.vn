#include "LeaderBoardNode.h"

USING_NS_CC;

LeaderBoardtNode::~LeaderBoardtNode()
{
	
}

bool LeaderBoardtNode::init()
{
	if (!Node::init())
	{
		return false;
	}

	Sprite* pSpriteBackground = Sprite::create("Target-End-Game/high-score.png");
	pSpriteBackground->setAnchorPoint(Point(0.5f, 0.5f));
	this->addChild(pSpriteBackground);
	this->setContentSize(pSpriteBackground->getContentSize());

	Label* pLabelTitle = Label::createWithTTF("HIGHSCORE", "fonts/ARLRDBD.ttf", 28);
	pLabelTitle->setColor(ccc3(0, 0, 0));
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPositionY(60.0f);
	this->addChild(pLabelTitle);

	Node* pNodeItem1 = createItem(7894563, "Tracy", "Target-End-Game/avatar.png", 1); 
	pNodeItem1->setPositionX(-200.0f);
	this->addChild(pNodeItem1);

	Node* pNodeItem2 = createItem(6541239, "Tracy", "Target-End-Game/avatar.png", 2);
	this->addChild(pNodeItem2);

	Node* pNodeItem3 = createItem(4123569, "Tracy", "Target-End-Game/avatar.png", 3);
	pNodeItem3->setPositionX(200.0f);
	this->addChild(pNodeItem3);

	return true;
}

Node* LeaderBoardtNode::createItem(const int& iScore, const char* sName, const char* sPathAvatar, const int& iINdex)
{
	Node* pNodeItem = Node::create();

	Sprite* pSpriteBackgroundAvatar = Sprite::create("Target-End-Game/avatar border.png");
	pSpriteBackgroundAvatar->setAnchorPoint(Point(0.5f, 0.0f));
	pSpriteBackgroundAvatar->setPositionY(110.0f);
	pNodeItem->addChild(pSpriteBackgroundAvatar);

	Sprite* pSpriteAvatar = Sprite::create(sPathAvatar);
	pSpriteAvatar->setAnchorPoint(Point(0.5f, 0.0f));
	pSpriteAvatar->setPositionY(115.0f);
	pNodeItem->addChild(pSpriteAvatar);

	Label* pLabelName = Label::createWithTTF(sName, "fonts/ARLRDBD.ttf", 24);
	pLabelName->setColor(ccc3(0, 0, 0));
	pLabelName->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelName->setPosition(Point(-45.0f, 60.0f));
	pNodeItem->addChild(pLabelName);

	char sScore[10];
	sprintf( sScore, "%d", iScore);
	Label* pLabelScore = Label::createWithTTF(sScore, "fonts/ARLRDBD.ttf", 24);
	pLabelScore->setColor(ccc3(0, 0, 0));
	pLabelScore->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelScore->setPosition(Point(-50.0f, 30.0f));
	pNodeItem->addChild(pLabelScore);

	char sIndex[10];
	sprintf( sIndex, "%d", iINdex);
	Label* pLabelIndex = Label::createWithTTF(sIndex, "fonts/ARLRDBD.ttf", 50);
	pLabelIndex->setColor(ccc3(96, 137, 241));
	pLabelIndex->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelIndex->setPosition(Point(-90.0f, 15.0f));
	pNodeItem->addChild(pLabelIndex);

	pNodeItem->setContentSize(CCSizeMake(200, 160));
	pNodeItem->setAnchorPoint(Point(0.0f, 0.0f));
	pNodeItem->setPositionY(-160.0f);

	return pNodeItem;
}

void LeaderBoardtNode::addButtonShowMe()
{
	MenuItemImage* pShowMeItem = MenuItemImage::create(
		"Target-End-Game/Me-button.png",
		"Target-End-Game/Me-button.png",
		CC_CALLBACK_0(LeaderBoardtNode::menuShowMeCallback, this));
	pShowMeItem->setPosition(Point(230.0f, 85.0f));

	Menu* pMenu = Menu::create(pShowMeItem, NULL);
	pMenu->setPosition(Point::ZERO);
	this->addChild(pMenu);
}

void LeaderBoardtNode::menuShowMeCallback()
{
}