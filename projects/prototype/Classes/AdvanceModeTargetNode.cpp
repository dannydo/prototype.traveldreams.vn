#include "AdvanceModeTargetNode.h"
#include "LeaderBoardAdvanceModeNode.h"

USING_NS_CC;

AdvanceModeTargetNode* AdvanceModeTargetNode::createLayout(const std::string sPackageId)
{
	AdvanceModeTargetNode* pGameTargetNode = new AdvanceModeTargetNode();
	pGameTargetNode->m_sPackageId = sPackageId;

	if(pGameTargetNode->init())
	{
		pGameTargetNode->autorelease();
		return pGameTargetNode;
	}

	CC_SAFE_DELETE(pGameTargetNode);
	return NULL;
}

bool AdvanceModeTargetNode::init()
{
	if (!Node::init())
	{
		return false;
	}

	Sprite* pBackground = Sprite::create("AdvanceMode/background.png");
	pBackground->setPosition(Point(320.0f, 485.0f));
	this->addChild(pBackground);

	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);

	Sprite* pBackgroundPopUp = Sprite::create("AdvanceMode/panel-AM-short.png");
	pBackgroundPopUp->setPosition(Point(320.0f, 595.0f));
	this->addChild(pBackgroundPopUp);

	LabelBMFont *pLabelTitle = LabelBMFont::create("ADVANCE MODE", "fonts/font-bechic.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(320.0f, 880.0f));
	this->addChild(pLabelTitle);

	Sprite* pPanel = Sprite::create("AdvanceMode/panel-target-end-game.png");
	pPanel->setPosition(Point(320.0f, 550.0f));
	this->addChild(pPanel);

	Sprite* pIconEye1 = Sprite::create("AdvanceMode/eye-icon.png");
	pIconEye1->setPosition(Point(500.0f, 620.0f));
	this->addChild(pIconEye1);

	Sprite* pIconEye2 = Sprite::create("AdvanceMode/eye-icon.png");
	pIconEye2->setPosition(Point(500.0f, 550.0f));
	this->addChild(pIconEye2);

	m_csPackageInfo = CSPackageTable::getCSPackageInfo(m_sPackageId);

	LabelBMFont *pLabelpackageName = LabelBMFont::create(m_csPackageInfo.sPackageName.c_str(), "fonts/font_title-popup.fnt");
	pLabelpackageName->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelpackageName->setPosition(Point(320.0f, 780.0f));
	this->addChild(pLabelpackageName);

	LabelTTF* pLabelTCreatedBy = LabelTTF::create(m_csPackageInfo.sCreatedBy.c_str(), "Arial", 28);
	pLabelTCreatedBy->setColor(ccc3(255.0f, 255.0f, 255.0f));
	pLabelTCreatedBy->setPosition(Point(320.0f, 720.0f));
	this->addChild(pLabelTCreatedBy);

	LabelBMFont *pLabelTotalWord = LabelBMFont::create("Total Words", "fonts/font_small_alert.fnt", 310.0f);
	pLabelTotalWord->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelTotalWord->setPosition(Point(122.0f, 620));
	this->addChild(pLabelTotalWord);

	char sTotalWord[10];
	sprintf(sTotalWord, "%d", m_csPackageInfo.iTotalWord);
	LabelBMFont *pLabelNumberTotalWord = LabelBMFont::create(sTotalWord, "fonts/font_score.fnt");
	pLabelNumberTotalWord->setAnchorPoint(Point(1.0f, 0.5f));
	pLabelNumberTotalWord->setPosition(Point(475.0f, 620));
	pLabelNumberTotalWord->setScale(1.2);
	this->addChild(pLabelNumberTotalWord);

	LabelBMFont *pLabelWordCollected = LabelBMFont::create("Words Collected", "fonts/font_small_alert.fnt", 310.0f);
	pLabelWordCollected->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelWordCollected->setPosition(Point(122.0f, 550));
	this->addChild(pLabelWordCollected);

	char sTotalWordUnlock[10];
	sprintf(sTotalWordUnlock, "%d", m_csPackageInfo.iTotalWordUnlock);
	LabelBMFont *pLabelNumberWordCollected = LabelBMFont::create(sTotalWordUnlock, "fonts/font_score.fnt");
	pLabelNumberWordCollected->setAnchorPoint(Point(1.0f, 0.5f));
	pLabelNumberWordCollected->setPosition(Point(475.0f, 550));
	pLabelNumberWordCollected->setScale(1.2);
	this->addChild(pLabelNumberWordCollected);

	LabelBMFont *pLabelStage = LabelBMFont::create("Best Stage", "fonts/font_small_alert.fnt", 310.0f);
	pLabelStage->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelStage->setPosition(Point(122.0f, 480.0f));
	this->addChild(pLabelStage);

	char sStage[10];
	sprintf(sStage, "%d", m_csPackageInfo.iStage);
	LabelBMFont *pLabelNumberStage = LabelBMFont::create(sStage, "fonts/font_score.fnt");
	pLabelNumberStage->setAnchorPoint(Point(1.0f, 0.5f));
	pLabelNumberStage->setPosition(Point(475.0f, 480.0f));
	pLabelNumberStage->setScale(1.2);
	this->addChild(pLabelNumberStage);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	Sprite* m_pButtonPlayImage = Sprite::create("AdvanceMode/btn_big_play.png");
	ButtonNode* pButtonPlay = ButtonNode::createButtonSprite(m_pButtonPlayImage, CC_CALLBACK_1(AdvanceModeTargetNode::clickPlayAdvanceMode, this));
	pButtonPlay->setPosition(Point(320.0f, 320.0f));
	pButtonManagerNode->addButtonNode(pButtonPlay);

	Sprite* m_pButtonCloseImage = Sprite::create("AdvanceMode/btn_close.png");
	ButtonNode* pButtonClose = ButtonNode::createButtonSprite(m_pButtonCloseImage, CC_CALLBACK_1(AdvanceModeTargetNode::clickClose, this));
	pButtonClose->setPosition(Point(580.0f, 898.0f));
	pButtonManagerNode->addButtonNode(pButtonClose);

	LeaderBoardAdvanceModeNode* pLeaderBoard = LeaderBoardAdvanceModeNode::createLayout(m_sPackageId);
	pLeaderBoard->setPosition(Point(320.0f, 114.0f));
	this->addChild(pLeaderBoard);

	return true;
}		

void AdvanceModeTargetNode::clickPlayAdvanceMode(Object* sender)
{	
	
}

void AdvanceModeTargetNode::clickClose(Object* sender)
{
	
}