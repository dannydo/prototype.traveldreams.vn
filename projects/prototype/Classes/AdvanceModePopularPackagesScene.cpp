#include "AdvanceModePopularPackagesScene.h"
#include "ClipMaskNode.h"
#include "APIService\PackageService.h"
#include "AdvanceModeTargetNode.h"

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

	LabelTTF* pLabelMyPackage = LabelTTF::create("Popular Packages", "Arial", 35);
	pLabelMyPackage->setColor(ccc3(255.0f, 255.0f, 255.0f));
	pLabelMyPackage->setPosition(Point(200.0f, 630.0f));
	this->addChild(pLabelMyPackage);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	Sprite* m_pButtonAddMorePackageImage = Sprite::create("AdvanceMode/btn-add-small.png");
	ButtonNode* m_pButtonAddMorePackage = ButtonNode::createButtonSprite(m_pButtonAddMorePackageImage, CC_CALLBACK_1(AdvanceModePopularPackagesLayer::clickAddPackage, this));
	m_pButtonAddMorePackage->setPosition(Point(467.0f, 760.0f));
	pButtonManagerNode->addButtonNode(m_pButtonAddMorePackage);

	Sprite* m_pButtonPackageCodeImage = Sprite::create("AdvanceMode/text-box-small.png");
	//ButtonNode* m_pButtonPackageCode = ButtonNode::createButtonSprite(m_pButtonPackageCodeImage, CC_CALLBACK_1(AdvanceModePopularPackagesLayer::clickAddPackageCode, this));
	//m_pButtonPackageCode->setPosition(Point(213.0f, 760.0f));
	//pButtonManagerNode->addButtonNode(m_pButtonPackageCode);
	m_pButtonPackageCodeImage->setPosition(Point(213.0f, 760.0f));
	this->addChild(m_pButtonPackageCodeImage);


	m_pCodeEditBox = EditBox::create(Size(250.f, 50.f), Scale9Sprite::create("AdvanceMode/blank_edit.png"));
	m_pCodeEditBox->setFont("Arial", 30);
	m_pCodeEditBox->setFontColor(Color3B( 50, 50, 50));
    //m_pCodeEditBox->setPosition( Point(60.f, 260.f)); //Point(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y+visibleSize.height/4));
	m_pCodeEditBox->setPosition(Point(215.0f, 760.0f));
    //m_pCodeEditBox->setAnchorPoint(Point(0, 0.5f));
    m_pCodeEditBox->setPlaceHolder("Input code");
	m_pCodeEditBox->setInputFlag(EditBox::InputFlag::INTIAL_CAPS_ALL_CHARACTERS);
	//m_pCodeEditBox->setInputMode(EditBox::InputMode::ANY);
    //editCode->setDelegate(this);
    addChild(m_pCodeEditBox);



	LabelTTF* pLabelDescription = LabelTTF::create("Build your own vocab from vs.kiss-concept.com", "Arial", 20);
	pLabelDescription->setColor(ccc3(255.0f, 255.0f, 255.0f));
	pLabelDescription->setPosition(Point(320.0f, 700.0f));
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

	m_pSlideShow = ButtonManagerNode::create();
	m_pSlideShow->AllowSwipingBackground(true);
	m_pSlideShow->setPosition(Point(0.0f, 295.0f));

	ClipMaskNode* pClipMaskNode = ClipMaskNode::create();
	pClipMaskNode->setPosition(Point(28.0f, 150.0f));
	pClipMaskNode->setContentSize(Size(544.0f, 448.0f));
	pClipMaskNode->addChild(m_pSlideShow);
	m_pBackgroundSlideShow->addChild(pClipMaskNode);

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

	Breadcrumb::getInstance()->addSceneMode(SceneMode::kPopularPackage);

	m_pScrollManager = new ScrollManager();

	m_iConnectServer = UserDefault::getInstance()->getIntegerForKey("NumberConnectServer", 0);
	m_iConnectServer++;
	UserDefault::getInstance()->setIntegerForKey("NumberConnectServer", m_iConnectServer);

	PackageService::getInstance()->setInterfaceServiceCallBack(this);
	PackageService::getInstance()->getPackagePopular(0, 20, m_iConnectServer);

	m_iTotalPackage = 0;
	m_maxHeight = 0;

	m_CustomPackageDownloadManager.SetDownloadPackageCompleteCallback(std::bind( &AdvanceModePopularPackagesLayer::OnDownloadPackageComplete, this, std::placeholders::_1));

	return true;
}

void AdvanceModePopularPackagesLayer::resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey)
{
	String sTag = "GetPackagePopular";
	sTag.appendWithFormat("_%d", m_iConnectServer);
	if (sKey == sTag.getCString())
	{
		if (pJsonDict != NULL)
		{
			cs::JsonDictionary* jsonData = pJsonDict->getSubDictionary("data");
			if (jsonData->getItemBoolvalue("result", false))
			{
				m_iTotalPackage = jsonData->getArrayItemCount("list");
				m_maxHeight = m_iTotalPackage*120.0f;
				for(int iIndex=0; iIndex < m_iTotalPackage; iIndex++)
				{
					cs::JsonDictionary* pJsonItem = jsonData->getSubItemFromArray("list", iIndex);
					CSPackageInfo csPackageInfo;
					csPackageInfo.sPackageName = pJsonItem->getItemStringValue("WordListName");
					csPackageInfo.sPackageCode = pJsonItem->getItemStringValue("PackageCode");
					csPackageInfo.sCreatedBy = pJsonItem->getItemStringValue("Author");
					csPackageInfo.iTotalWord = pJsonItem->getItemIntValue("TotalWords", 0);
					csPackageInfo.iTotalWordUnlock = 0;

					m_csPackageInfos.push_back(csPackageInfo);

					LabelBMFont *pLabelPackageName = LabelBMFont::create(csPackageInfo.sPackageName.c_str(), "fonts/font_small_alert.fnt", 310.0f);
					pLabelPackageName->setAnchorPoint(Point(0.0f, 0.5f));
					pLabelPackageName->setPosition(Point(0.0f, -iIndex*120 - 10));
					m_pSlideShow->addChild(pLabelPackageName);

					LabelTTF* pLabelTCreatedBy = LabelTTF::create(csPackageInfo.sCreatedBy.c_str(), "Arial", 28, Size(310.0f, 30.0f), TextHAlignment::LEFT, TextVAlignment::TOP);
					pLabelTCreatedBy->setColor(ccc3(255.0f, 255.0f, 255.0f));
					pLabelTCreatedBy->setAnchorPoint(Point(0.0f, 0.5f));
					pLabelTCreatedBy->setPosition(Point(0.0f, -iIndex*120 - 55));
					m_pSlideShow->addChild(pLabelTCreatedBy);

					LabelBMFont *pLabelPackageCode = LabelBMFont::create(csPackageInfo.sPackageCode.c_str(), "fonts/font_score.fnt");
					pLabelPackageCode->setAnchorPoint(Point(1.0f, 0.5f));
					pLabelPackageCode->setPosition(Point(440.0f, -iIndex*120 - 10));
					m_pSlideShow->addChild(pLabelPackageCode);

					char sTotalWord[20];
					sprintf(sTotalWord, "%d Words", csPackageInfo.iTotalWord);
					LabelTTF* pLabelTotalWord = LabelTTF::create(sTotalWord, "Arial", 28);
					pLabelTotalWord->setColor(ccc3(255.0f, 255.0f, 255.0f));
					pLabelTotalWord->setAnchorPoint(Point(1.0f, 0.5f));
					pLabelTotalWord->setPosition(Point(420.0f, -iIndex*120 - 55));
					m_pSlideShow->addChild(pLabelTotalWord);

					Sprite* pButtonPlayPackageImage = Sprite::create("AdvanceMode/download-icon.png");
					ButtonNode* pButtonPlayPackage = ButtonNode::createButtonSprite(pButtonPlayPackageImage, CC_CALLBACK_1(AdvanceModePopularPackagesLayer::clickDownloadPackage, this));
					pButtonPlayPackage->setTag(iIndex);
					pButtonPlayPackage->setPosition(Point(460.0f, -iIndex*120 - 35));
					m_pSlideShow->addButtonNode(pButtonPlayPackage);

					Sprite* pLineImage = Sprite::create("AdvanceMode/line.png"); 
					pLineImage->setPosition(Point(245.0f, -iIndex*120 -90.0f));
					m_pSlideShow->addChild(pLineImage, 5);
				}
			}
		}
	}
}

void AdvanceModePopularPackagesLayer::clickDownloadPackage(Object* sender)
{
	int iPopularPackageIndex = ((ButtonNode*)sender)->getTag();
	m_pCodeEditBox->setText( m_csPackageInfos[iPopularPackageIndex].sPackageCode.c_str());
	clickAddPackage(NULL);
}

void AdvanceModePopularPackagesLayer::OnDownloadPackageComplete(const CSPackageInfo& packageInfo)
<<<<<<< a2e5f4877b9168d777fb103129c3a22bdfe4556b
{	
	auto* pTargetNode = AdvanceModeTargetNode::createLayout( packageInfo.sPackageId, false);
	pTargetNode->setIsBackToMyPackage(true);
	this->addChild(pTargetNode);
}

void AdvanceModePopularPackagesLayer::clickAddPackage(Object* sender)
{
	if(m_pCodeEditBox->getText()[0] == 0)
		return;

	m_CustomPackageDownloadManager.StartDownloadPackage( this, m_pCodeEditBox->getText());
}

void AdvanceModePopularPackagesLayer::clickAddPackageCode(Object* sender)
{
}

bool AdvanceModePopularPackagesLayer::onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
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

void AdvanceModePopularPackagesLayer::onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	if (m_pSlideShow->IsInTapMode())
		return;

	Point touchPosition = pTouch->getLocation();
	m_fYMoved = touchPosition.y - m_fBeginY;

	if (m_fYMoved + m_pSlideShow->getPosition().y <= 295.0f) {
		m_fYMoved = 295.0f - m_pSlideShow->getPosition().y;
	}
	else if(m_fYMoved + m_pSlideShow->getPosition().y >= m_maxHeight + 295.0f - 448.0f)
	{
		m_fYMoved = m_maxHeight + 295.0f - 448.0f - m_pSlideShow->getPosition().y;
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

void AdvanceModePopularPackagesLayer::onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
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

		
		if (distanceY + m_pSlideShow->getPosition().y <= 295.0f) {
			distanceY = 295.0f - m_pSlideShow->getPosition().y;
		}
		else if(distanceY + m_pSlideShow->getPosition().y >= m_maxHeight + 295.0f - 448.0f)
		{
			distanceY = m_maxHeight + 295.0f - 448.0f - m_pSlideShow->getPosition().y;
		}

		auto actionMove = MoveBy::create(fTime, Point(0.0f, distanceY));
		auto actionEaseOut = EaseOut::create(actionMove, 2.5f);
		m_pSlideShow->stopAllActions();
		m_pSlideShow->runAction(Sequence::create(actionEaseOut, NULL));
	}
}