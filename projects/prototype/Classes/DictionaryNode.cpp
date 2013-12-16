#include "DictionaryNode.h"
#include "GameTargetNode.h"
#include "EndGameNode.h"

USING_NS_CC;

bool DictionaryNode::init()
{
	if (!Node::init())
	{
		return false;
	}

	Sprite* imgDictionary = Sprite::create("Dictionary/Dict_Board.png");
	this->addChild(imgDictionary);

	MenuItemImage* pCloseDictionary = MenuItemImage::create(
		"Dictionary/Dict_Close_Button.png",
		"Dictionary/Dict_Close_Button.png",
		CC_CALLBACK_0(DictionaryNode::menuCloseCallBack, this));

	Menu* pMenuClose = Menu::create(pCloseDictionary, NULL);
	pMenuClose->setPosition(Point(imgDictionary->getContentSize().width/2.0f, imgDictionary->getContentSize().height/2.0f));
	this->addChild(pMenuClose);

	return true;
}

void DictionaryNode::menuCloseCallBack()
{
	if (typeid(*this->getParent()) == typeid(GameTargetNode))
	{
		GameTargetNode* parent = (GameTargetNode*)this->getParent();
		parent->setEnableAction(true);
		parent->removeChild(this);
	}
	else if(typeid(*this->getParent()) == typeid(EndGameNode))
	{
		EndGameNode* parent = (EndGameNode*)this->getParent();
		parent->setEnableAction(true);
		parent->removeChild(this);
	}
	else
		getParent()->removeChild(this);
}