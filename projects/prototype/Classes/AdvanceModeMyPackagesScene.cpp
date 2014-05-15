#include "AdvanceModeMyPackagesScene.h"
#include "ClipMaskNode.h"
#include "AdvanceModePopularPackagesScene.h"
#include "AdvanceModeTargetNode.h"

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
	m_pSlideShow->AllowSwipingBackground(true);
	m_pSlideShow->setPosition(Point(0.0f, 150.0f));

	ClipMaskNode* pClipMaskNode = ClipMaskNode::create();
	pClipMaskNode->setPosition(Point(28.0f, 295.0f));
	pClipMaskNode->setContentSize(Size(544.0f, 448.0f));
	pClipMaskNode->addChild(m_pSlideShow);
	m_pBackgroundSlideShow->addChild(pClipMaskNode);

	CSPackageTable::getAllCSPackages(m_csPackageInfos);
	m_maxHeight = m_csPackageInfos.size()*120.0f;
	for (int iIndex = 0; iIndex < m_csPackageInfos.size(); iIndex++)
	{
		LabelBMFont *pLabelPackageName = LabelBMFont::create(m_csPackageInfos[iIndex].sPackageName.c_str(), "fonts/font_small_alert.fnt", 310.0f);
		pLabelPackageName->setAnchorPoint(Point(0.0f, 0.5f));
		pLabelPackageName->setPosition(Point(0.0f, -iIndex*120 - 10));
		m_pSlideShow->addChild(pLabelPackageName);

		LabelTTF* pLabelTCreatedBy = LabelTTF::create(m_csPackageInfos[iIndex].sCreatedBy.c_str(), "Arial", 28);
		pLabelTCreatedBy->setColor(ccc3(255.0f, 255.0f, 255.0f));
		pLabelTCreatedBy->setAnchorPoint(Point(0.0f, 0.5f));
		pLabelTCreatedBy->setPosition(Point(0.0f, -iIndex*120 - 55));
		m_pSlideShow->addChild(pLabelTCreatedBy);

		LabelBMFont *pLabelPackageCode = LabelBMFont::create(m_csPackageInfos[iIndex].sPackageCode.c_str(), "fonts/font_score.fnt");
		pLabelPackageCode->setAnchorPoint(Point(1.0f, 0.5f));
		pLabelPackageCode->setPosition(Point(440.0f, -iIndex*120 - 10));
		m_pSlideShow->addChild(pLabelPackageCode);

		char sTotalWord[20];
		sprintf(sTotalWord, "%d/%d", m_csPackageInfos[iIndex].iTotalWordUnlock,  m_csPackageInfos[iIndex].iTotalWord);
		LabelTTF* pLabelTotalWord = LabelTTF::create(sTotalWord, "Arial", 28);
		pLabelTotalWord->setColor(ccc3(255.0f, 255.0f, 255.0f));
		pLabelTotalWord->setAnchorPoint(Point(1.0f, 0.5f));
		pLabelTotalWord->setPosition(Point(420.0f, -iIndex*120 - 55));
		m_pSlideShow->addChild(pLabelTotalWord);

		Sprite* pButtonPlayPackageImage = Sprite::create("AdvanceMode/btn-play-advance-mode.png");
		ButtonNode* pButtonPlayPackage = ButtonNode::createButtonSprite(pButtonPlayPackageImage, CC_CALLBACK_1(AdvanceModeMyPackagesLayer::clickPlayAdvanceMode, this));
		pButtonPlayPackage->setTag(iIndex);
		pButtonPlayPackage->setPosition(Point(460.0f, -iIndex*120 - 35));
		m_pSlideShow->addButtonNode(pButtonPlayPackage);

		Sprite* pLineImage = Sprite::create("AdvanceMode/line.png"); 
		pLineImage->setPosition(Point(245.0f, -iIndex*120 -90.0f));
		m_pSlideShow->addChild(pLineImage, 5);
	}

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

	Breadcrumb::getInstance()->addSceneMode(SceneMode::kMyPackage);

	m_pScrollManager = new ScrollManager();

	return true;
}

void AdvanceModeMyPackagesLayer::clickPlayAdvanceMode(Object* sender)
{
	int iPackageIndex = ((ButtonNode*)sender)->getTag();	 
	auto* pTargetNode = AdvanceModeTargetNode::createLayout( m_csPackageInfos[iPackageIndex].sPackageId, true);
	this->addChild(pTargetNode);
}

void AdvanceModeMyPackagesLayer::clickAddMorePackage(Object* sender)
{
	auto scene = AdvanceModePopularPackagesScene::create();
	Director::getInstance()->replaceScene(scene);
}

bool AdvanceModeMyPackagesLayer::onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	if(m_pFooterNode->getSettingNode() != NULL && m_pFooterNode->getSettingNode()->getShowSetting() || m_maxHeight < 448.0f)
	{
		return false;
	}

	Point touchPosition = pTouch->getLocation();
	m_fBeginY = touchPosition.y;

	CCRect *pRectButton = new CCRect(m_pBackgroundSlideShow->getOffsetPosition().x, 
			m_pBackgroundSlideShow->getOffsetPosition().y, 
			m_pBackgroundSlideShow->getTextureRect().size.width, 
			m_pBackgroundSlideShow->getTextureRect().size.height);

	Point touchButton = m_pBackgroundSlideShow->convertToNodeSpace(touchPosition);

	if(pRectButton->containsPoint(touchButton))
	{
		DataTouch dataTouch;
		dataTouch.point = touchPosition;
		dataTouch.lTime = 0;
		dataTouch.fDeltaTime = 0;
		m_pScrollManager->addDataToQueue(dataTouch);

		return true;
	}
	
	return false;
}

void AdvanceModeMyPackagesLayer::onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	if (m_pSlideShow->IsInTapMode())
		return;

	Point touchPosition = pTouch->getLocation();
	m_fYMoved = touchPosition.y - m_fBeginY;

	if (m_fYMoved + m_pSlideShow->getPosition().y <= 150.0f) {
		m_fYMoved = 150.0f - m_pSlideShow->getPosition().y;
	}
	else if(m_fYMoved + m_pSlideShow->getPosition().y >= m_maxHeight + 150.0f - 448.0f)
	{
		m_fYMoved = m_maxHeight + 150.0f - 448.0f - m_pSlideShow->getPosition().y;
	}

	Point point = m_pSlideShow->getPosition();
	m_pSlideShow->setPositionY(point.y + m_fYMoved);
	m_fBeginY = touchPosition.y;

	DataTouch dataTouch;
	dataTouch.point = touchPosition;
	dataTouch.lTime = 0;
	dataTouch.fDeltaTime = 0;
	m_pScrollManager->addDataToQueue(dataTouch);
}

void AdvanceModeMyPackagesLayer::onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	if (m_pSlideShow->IsInTapMode())
		return;

	DataTouch dataTouch = m_pScrollManager->getDistanceScrollY();
	float distanceY = dataTouch.point.y;
	float deltaTime = dataTouch.fDeltaTime;
	
	if(distanceY!=0 && deltaTime!=0)
	{
		float fTime = 0.2f;
		distanceY = distanceY * fTime / deltaTime / 10; 

		
		if (distanceY + m_pSlideShow->getPosition().y <= 150.0f) {
			distanceY = 150.0f - m_pSlideShow->getPosition().y;
		}
		else if(distanceY + m_pSlideShow->getPosition().y >= m_maxHeight + 150.0f - 448.0f)
		{
			distanceY = m_maxHeight + 150.0f - 448.0f - m_pSlideShow->getPosition().y;
		}

		auto actionMove = MoveBy::create(fTime, Point(0.0f, distanceY));
		auto actionEaseOut = EaseOut::create(actionMove, 2.5f);
		m_pSlideShow->stopAllActions();
		m_pSlideShow->runAction(Sequence::create(actionEaseOut, NULL));
	}
}