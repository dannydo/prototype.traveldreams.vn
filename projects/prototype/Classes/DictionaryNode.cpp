#include "DictionaryNode.h"

USING_NS_CC;

bool DictionaryNode::init()
{
	if (!Node::init())
	{
		return false;
	}

	LayerColor* pBackground = LayerColor::create(ccc4(0, 0, 0, 0));
	pBackground->setContentSize(CCSizeMake(640, 960));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { this->onTouchBackground(touch, event); return false; };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	m_pImgDictionary = Sprite::create("Dictionary/Dict_Board.png");
	m_pImgDictionary->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 50));
	this->addChild(m_pImgDictionary);

	MenuItemImage* pCloseDictionary = MenuItemImage::create(
		"Dictionary/Dict_Close_Button.png",
		"Dictionary/Dict_Close_Button.png",
		CC_CALLBACK_0(DictionaryNode::menuCloseCallBack, this));

	Menu* pMenuClose = Menu::create(pCloseDictionary, NULL);
	pMenuClose->setPosition(Point(this->getContentSize().width - 75, this->getContentSize().height - 75));
	this->addChild(pMenuClose);

	return true;
}

void DictionaryNode::menuCloseCallBack()
{	
	this->getParent()->removeChild(this);
}

void DictionaryNode::onTouchBackground(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	Point location = pTouch->getLocationInView();
	location = Director::sharedDirector()->convertToGL(location);

	if (location.x >= m_pImgDictionary->getPositionX() - m_pImgDictionary->getContentSize().width/2.0f && 
		location.x <= m_pImgDictionary->getPositionX() + m_pImgDictionary->getContentSize().width/2.0f &&
		location.y >= m_pImgDictionary->getPositionY() - m_pImgDictionary->getContentSize().height/2.0f &&
		location.y <= m_pImgDictionary->getPositionY() + m_pImgDictionary->getContentSize().height/2.0f)
	{
		pEvent->stopPropagation();
		 return;
	}

	this->getParent()->removeChild(this);
	pEvent->stopPropagation();
}