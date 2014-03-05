#include "DictionaryNode.h"
#include "Database\DictionaryDatabase.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

DictionaryNode::~DictionaryNode()
{
	
}

DictionaryNode* DictionaryNode::createLayout(const char* sWordDict)
{
	DictionaryNode* pDictionaryNode = new DictionaryNode();
	pDictionaryNode->m_sWordDict = sWordDict;
	if(pDictionaryNode->init())
	{
		pDictionaryNode->autorelease();
		return pDictionaryNode;
	}

	CC_SAFE_DELETE(pDictionaryNode);
	return NULL;
}

bool DictionaryNode::init()
{
	if (!Node::init())
	{
		return false;
	}

	m_pBackground = Sprite::create("Dictionary/background.png");
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { this->onTouchBackground(touch, event); return false; };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, m_pBackground);
	this->addChild(m_pBackground);
	this->setContentSize(m_pBackground->getContentSize());

	float fWidth = m_pBackground->getContentSize().width;
	float fHeight = m_pBackground->getContentSize().height;

	WordDictionaryInfo wordDictInfo = DictionaryDatabase::getInstance()->fectMeanWord(m_sWordDict);
	std::string sTitle = m_sWordDict;
	sTitle.append(" ");
	sTitle.append(wordDictInfo.sPhonetic);
	LabelTTF* pLabelTitle = LabelTTF::create(sTitle.c_str(), "Arial", 30);
	pLabelTitle->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelTitle->setPosition(Point(-fWidth/2.0f + 30 + pLabelTitle->getContentSize().width/2.0f, fHeight/2.0f-30));
	this->addChild(pLabelTitle);

	float fPoisitionY = pLabelTitle->getPositionY();

	cs::JsonDictionary* pJsonDict = new cs::JsonDictionary();
	pJsonDict->initWithDescription(wordDictInfo.sMean.c_str());

	int iTotalItem = pJsonDict->getArrayItemCount("data");
	for(int iIndex=0; iIndex<iTotalItem; iIndex++)
	{
		cs::JsonDictionary* pJsonItem = pJsonDict->getSubItemFromArray("data", iIndex);
		LabelTTF* pLabelType = LabelTTF::create(pJsonItem->getItemStringValue("Type"), "Arial", 25);
		pLabelType->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelType->setPosition(Point(-fWidth/2.0f + 30 + pLabelType->getContentSize().width/2.0f, fPoisitionY - pLabelTitle->getContentSize().height - 20));
		this->addChild(pLabelType);

		LabelTTF* pLabelMean = LabelTTF::create(pJsonItem->getItemStringValue("Mean"), "Arial", 20.0f, Size(fWidth-60, 230.0f), TextHAlignment::LEFT, TextVAlignment::TOP);
		pLabelMean->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelMean->setAnchorPoint(Point(0.5f, 1.0f));
		pLabelMean->setPosition(Point(-fWidth/2.0f + 30 + pLabelMean->getContentSize().width/2.0f, pLabelType->getPositionY() - pLabelType->getContentSize().height));
		this->addChild(pLabelMean);

		fPoisitionY = pLabelMean->getPositionY();
		break;
	}

	MenuItemImage* pCloseDictionary = MenuItemImage::create(
		"Dictionary/Dict_Close_Button.png",
		"Dictionary/Dict_Close_Button.png",
		CC_CALLBACK_0(DictionaryNode::menuCloseCallBack, this));
	pCloseDictionary->setPosition(Point(m_pBackground->getContentSize().width/2.0f-10, m_pBackground->getContentSize().height/2.0f-10));

	Menu* pMenuClose = Menu::create(pCloseDictionary, NULL);
	pMenuClose->setPosition(Point::ZERO);
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

	if (location.x >= m_pBackground->getPositionX() - m_pBackground->getContentSize().width/2.0f && 
		location.x <= m_pBackground->getPositionX() + m_pBackground->getContentSize().width/2.0f &&
		location.y >= m_pBackground->getPositionY() - m_pBackground->getContentSize().height/2.0f &&
		location.y <= m_pBackground->getPositionY() + m_pBackground->getContentSize().height/2.0f)
	{
		pEvent->stopPropagation();
		 return;
	}

	this->getParent()->removeChild(this);
	pEvent->stopPropagation();
}