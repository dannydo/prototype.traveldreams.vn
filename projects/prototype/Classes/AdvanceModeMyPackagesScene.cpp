#include "AdvanceModeMyPackagesScene.h"
#include "FooterNode.h"
#include "ClipMaskNode.h"
#include "AdvanceModePopularPackagesScene.h"

USING_NS_CC;

bool AdvanceModeMyPackagesScene::init()
{
	if(CCScene::init())
	{
		this->_layer = AdvanceModeMyPackagesLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

AdvanceModeMyPackagesScene::~AdvanceModeMyPackagesScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

AdvanceModeMyPackagesLayer::~AdvanceModeMyPackagesLayer()
{
	
}

bool AdvanceModeMyPackagesLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	Sprite* pBackground = Sprite::create("AdvanceMode/background.png");
	pBackground->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackground);

	Sprite* pBackgroundPopUp = Sprite::create("AdvanceMode/panel-AM-short.png");
	pBackgroundPopUp->setPosition(Point(320.0f, 590.0f));
	this->addChild(pBackgroundPopUp);

	LabelBMFont *pLabelTitle = LabelBMFont::create("ADVANCE MODE", "fonts/font-bechic.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(320.0f, 870.0f));
	this->addChild(pLabelTitle);

	LabelTTF* pLabelMyPackage = LabelTTF::create("My Packages", "Arial", 35);
	pLabelMyPackage->setColor(ccc3(255.0f, 255.0f, 255.0f));
	pLabelMyPackage->setPosition(Point(170.0f, 775.0f));
	this->addChild(pLabelMyPackage);

	m_pBackgroundSlideShow = Sprite::create("AdvanceMode/panel-vocab-list.png");
	m_pBackgroundSlideShow->setPosition(Point(315.0f, 520.0f));
	this->addChild(m_pBackgroundSlideShow);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	Sprite* m_pButtonAddMorePackageImage = Sprite::create("AdvanceMode/add-package.png");
	ButtonNode* pButtonAddMorePackage = ButtonNode::createButtonSprite(m_pButtonAddMorePackageImage, CC_CALLBACK_1(AdvanceModeMyPackagesLayer::clickAddMorePackage, this));
	pButtonAddMorePackage->setPosition(Point(320.0f, 175.0f));
	pButtonManagerNode->addButtonNode(pButtonAddMorePackage);

	LabelTTF* pLabelDescription = LabelTTF::create("Build your own vocab from vs.kiss-concept.com", "Arial", 20);
	pLabelDescription->setColor(ccc3(179.0f, 190.0f, 196.0f));
	pLabelDescription->setPosition(Point(320.0f, 120.0f));
	this->addChild(pLabelDescription);

	m_pFooterNode = FooterNode::create();
	m_pFooterNode->changeStatusButtonFlashcard(StatusButtonFlashcard::eNoClick);
	m_pFooterNode->removeBackground();
	m_pFooterNode->removeButtonFlashcard();
	this->addChild(m_pFooterNode);

	m_pSlideShow = ButtonManagerNode::create();
	m_pSlideShow->setPosition(Point(0.0f, 150.0f));

	ClipMaskNode* pClipMaskNode = ClipMaskNode::create();
	pClipMaskNode->setPosition(Point(28.0f, 295.0f));
	pClipMaskNode->setContentSize(Size(544.0f, 478.0f));
	pClipMaskNode->addChild(m_pSlideShow);
	m_pBackgroundSlideShow->addChild(pClipMaskNode);

	CSPackageTable::getAllCSPackages(m_csPackageInfos);
	for (int iIndex = 0; iIndex < 5; iIndex++)
	{
		LabelBMFont *pLabelPackageName = LabelBMFont::create(m_csPackageInfos[0].sPackageName.c_str(), "fonts/font_small_alert.fnt", 310.0f);
		pLabelPackageName->setAnchorPoint(Point(0.0f, 0.5f));
		pLabelPackageName->setPosition(Point(0.0f, -iIndex*120 - 10));
		m_pSlideShow->addChild(pLabelPackageName);

		LabelTTF* pLabelTCreatedBy = LabelTTF::create(m_csPackageInfos[0].sCreatedBy.c_str(), "Arial", 28);
		pLabelTCreatedBy->setColor(ccc3(255.0f, 255.0f, 255.0f));
		pLabelTCreatedBy->setAnchorPoint(Point(0.0f, 0.5f));
		pLabelTCreatedBy->setPosition(Point(0.0f, -iIndex*120 - 55));
		m_pSlideShow->addChild(pLabelTCreatedBy);

		LabelBMFont *pLabelPackageCode = LabelBMFont::create(m_csPackageInfos[0].sPackageCode.c_str(), "fonts/font_score.fnt");
		pLabelPackageCode->setAnchorPoint(Point(1.0f, 0.5f));
		pLabelPackageCode->setPosition(Point(440.0f, -iIndex*120 - 10));
		m_pSlideShow->addChild(pLabelPackageCode);

		char sTotalWord[20];
		sprintf(sTotalWord, "%d/%d", m_csPackageInfos[0].iTotalWordUnlock,  m_csPackageInfos[0].iTotalWord);
		LabelTTF* pLabelTotalWord = LabelTTF::create(sTotalWord, "Arial", 28);
		pLabelTotalWord->setColor(ccc3(255.0f, 255.0f, 255.0f));
		pLabelTotalWord->setAnchorPoint(Point(1.0f, 0.5f));
		pLabelTotalWord->setPosition(Point(420.0f, -iIndex*120 - 55));
		m_pSlideShow->addChild(pLabelTotalWord);

		Sprite* pButtonPlayPackageImage = Sprite::create("AdvanceMode/btn-play-advance-mode.png");
		ButtonNode* pButtonPlayPackage = ButtonNode::createButtonSprite(pButtonPlayPackageImage, CC_CALLBACK_1(AdvanceModeMyPackagesLayer::clickPlayAdvanceMode, this));
		pButtonPlayPackage->setTag(0);
		pButtonPlayPackage->setPosition(Point(460.0f, -iIndex*120 - 35));
		m_pSlideShow->addButtonNode(pButtonPlayPackage);

		Sprite* pLineImage = Sprite::create("AdvanceMode/line.png"); 
		pLineImage->setPosition(Point(245.0f, -iIndex*120 -90.0f));
		m_pSlideShow->addChild(pLineImage, 5);
	}

	return true;
}

void AdvanceModeMyPackagesLayer::clickPlayAdvanceMode(Object* sender)
{
}

void AdvanceModeMyPackagesLayer::clickAddMorePackage(Object* sender)
{
	auto scene = AdvanceModePopularPackagesScene::create();
	Director::getInstance()->replaceScene(scene);
}

bool AdvanceModeMyPackagesLayer::onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void AdvanceModeMyPackagesLayer::onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
}

void AdvanceModeMyPackagesLayer::onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
}