#include "AdvanceModePopularPackagesScene.h"
#include "FooterNode.h"
#include "ClipMaskNode.h"

USING_NS_CC;

bool AdvanceModePopularPackagesScene::init()
{
	if(CCScene::init())
	{
		this->_layer = AdvanceModePopularPackagesLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

AdvanceModePopularPackagesScene::~AdvanceModePopularPackagesScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

AdvanceModePopularPackagesLayer::~AdvanceModePopularPackagesLayer()
{
	
}

bool AdvanceModePopularPackagesLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	Sprite* pBackground = Sprite::create("AdvanceMode/background.png");
	pBackground->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackground);

	Sprite* pBackgroundPopUp = Sprite::create("AdvanceMode/panel-AM-long.png");
	pBackgroundPopUp->setPosition(Point(320.0f, 520.0f));
	this->addChild(pBackgroundPopUp);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	Sprite* m_pButtonAddMorePackageImage = Sprite::create("AdvanceMode/btn-add-small.png");
	ButtonNode* m_pButtonAddMorePackage = ButtonNode::createButtonSprite(m_pButtonAddMorePackageImage, CC_CALLBACK_1(AdvanceModePopularPackagesLayer::clickAddPackage, this));
	m_pButtonAddMorePackage->setPosition(Point(467.0f, 760.0f));
	pButtonManagerNode->addButtonNode(m_pButtonAddMorePackage);

	Sprite* m_pButtonPackageCodeImage = Sprite::create("AdvanceMode/text-box-small.png");
	ButtonNode* m_pButtonPackageCode = ButtonNode::createButtonSprite(m_pButtonPackageCodeImage, CC_CALLBACK_1(AdvanceModePopularPackagesLayer::clickAddPackageCode, this));
	m_pButtonPackageCode->setPosition(Point(213.0f, 760.0f));
	pButtonManagerNode->addButtonNode(m_pButtonPackageCode);

	LabelTTF* pLabelDescription = LabelTTF::create("Build your own vocab from vs.kiss-concept.com", "Arial", 20);
	pLabelDescription->setColor(ccc3(255.0f, 255.0f, 255.0f));
	pLabelDescription->setPosition(Point(320.0f, 705.0f));
	this->addChild(pLabelDescription);

	LabelBMFont *pLabelTitle = LabelBMFont::create("ADVANCE MODE", "fonts/font-bechic.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(320.0f, 880.0f));
	this->addChild(pLabelTitle);

	m_pBackgroundSlideShow = Sprite::create("AdvanceMode/panel-vocab-list.png");
	m_pBackgroundSlideShow->setPosition(Point(315.0f, 375.0f));
	this->addChild(m_pBackgroundSlideShow);

	m_pFooterNode = FooterNode::create();
	m_pFooterNode->changeStatusButtonFlashcard(StatusButtonFlashcard::eNoClick);
	m_pFooterNode->removeBackground();
	m_pFooterNode->removeButtonFlashcard();
	this->addChild(m_pFooterNode);

	return true;
}

void AdvanceModePopularPackagesLayer::clickDownloadPackage(Object* sender)
{
}

void AdvanceModePopularPackagesLayer::clickAddPackage(Object* sender)
{
}

void AdvanceModePopularPackagesLayer::clickAddPackageCode(Object* sender)
{
}

bool AdvanceModePopularPackagesLayer::onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void AdvanceModePopularPackagesLayer::onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
}

void AdvanceModePopularPackagesLayer::onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
}
