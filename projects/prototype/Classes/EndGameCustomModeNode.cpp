#include "EndGameCustomModeNode.h"
#include "HelloWorldScene.h"
#include "LeaderBoardAdvanceModeNode.h"

USING_NS_CC;

EndGameCustomModeNode* EndGameCustomModeNode::createLayout(const int& iStage, const int& iWordNew, const std::string& sPackageId)
{
	EndGameCustomModeNode* pEndGameNode = new EndGameCustomModeNode();
	pEndGameNode->m_iStage = iStage;
	pEndGameNode->m_iWordNew = iWordNew;
	pEndGameNode->m_sPackageId = sPackageId;

	if(pEndGameNode->init())
	{
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

bool EndGameCustomModeNode::init()
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

	Sprite* pNewRecord = Sprite::create("AdvanceMode/new-record.png");
	pNewRecord->setPosition(Point(548.0f, 668.0f));
	this->addChild(pNewRecord);

	Sprite* pIconEye1 = Sprite::create("AdvanceMode/eye-icon.png");
	pIconEye1->setPosition(Point(500.0f, 550.0f));
	this->addChild(pIconEye1);

	Sprite* pIconEye2 = Sprite::create("AdvanceMode/eye-icon.png");
	pIconEye2->setPosition(Point(500.0f, 480.0f));
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

	char sStage[20];
	sprintf(sStage, "Stage %d", m_csPackageInfo.iStage);
	LabelBMFont *pLabelStage = LabelBMFont::create(sStage, "fonts/font_score.fnt", 310.0f);
	pLabelStage->setPosition(Point(320.0f, 630));
	pLabelStage->setScale(1.4);
	this->addChild(pLabelStage);

	LabelBMFont *pLabelTotalWord = LabelBMFont::create("Total Words", "fonts/font_small_alert.fnt", 310.0f);
	pLabelTotalWord->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelTotalWord->setPosition(Point(122.0f, 550));
	this->addChild(pLabelTotalWord);

	char sTotalWord[20];
	sprintf(sTotalWord, "%d/%d", m_csPackageInfo.iTotalWordUnlock, m_csPackageInfo.iTotalWord);
	LabelBMFont *pLabelNumberTotalWord = LabelBMFont::create(sTotalWord, "fonts/font_score.fnt");
	pLabelNumberTotalWord->setAnchorPoint(Point(1.0f, 0.5f));
	pLabelNumberTotalWord->setPosition(Point(475.0f, 550));
	pLabelNumberTotalWord->setScale(1.2);
	this->addChild(pLabelNumberTotalWord);

	LabelBMFont *pLabelWordNew = LabelBMFont::create("New words", "fonts/font_small_alert.fnt", 310.0f);
	pLabelWordNew->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelWordNew->setPosition(Point(122.0f, 480));
	this->addChild(pLabelWordNew);

	char sTotalWordNew[10];
	sprintf(sTotalWordNew, "%d", m_iWordNew);
	LabelBMFont *pLabelNumberWordNew = LabelBMFont::create(sTotalWordNew, "fonts/font_score.fnt");
	pLabelNumberWordNew->setAnchorPoint(Point(1.0f, 0.5f));
	pLabelNumberWordNew->setPosition(Point(475.0f, 480));
	pLabelNumberWordNew->setScale(1.2);
	this->addChild(pLabelNumberWordNew);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	Sprite* m_pButtonPlayImage = Sprite::create("AdvanceMode/btn_replay.png");
	ButtonNode* pButtonPlay = ButtonNode::createButtonSprite(m_pButtonPlayImage, CC_CALLBACK_1(EndGameCustomModeNode::clickRetry, this));
	pButtonPlay->setPosition(Point(320.0f, 345.0f));
	pButtonManagerNode->addButtonNode(pButtonPlay);

	Sprite* m_pButtonCloseImage = Sprite::create("AdvanceMode/btn_close.png");
	ButtonNode* pButtonClose = ButtonNode::createButtonSprite(m_pButtonCloseImage, CC_CALLBACK_1(EndGameCustomModeNode::clickClose, this));
	pButtonClose->setPosition(Point(580.0f, 898.0f));
	pButtonManagerNode->addButtonNode(pButtonClose);

	LeaderBoardAdvanceModeNode* pLeaderBoard = LeaderBoardAdvanceModeNode::createLayout(m_sPackageId);
	pLeaderBoard->setPosition(Point(320.0f, 114.0f));
	this->addChild(pLeaderBoard);

	return true;
}

void EndGameCustomModeNode::clickRetry(Object* sender)
{
	auto timeModeConfig = &GameConfigManager::getInstance()->GetTimeModeDemoConfig();
	GameWordManager::getInstance()->GenerateWordForNewLevelOfTimeMode(timeModeConfig, true);

	auto scene = HelloWorld::createScene( _GMT_TIME_MODE_);
	Director::getInstance()->replaceScene(scene);
}

void EndGameCustomModeNode::clickClose(Object* sender)
{
	
}

/*
void EndGameCustomModeNode::addYellowStar(const int& iYellowStar)
{
	m_iYellowStar = iYellowStar;
	m_iCountYellowStar = 0;

	this->sequenceUpdateStar();
}

void EndGameCustomModeNode::sequenceUpdateStar()
{
	auto actionUpdateStar = CallFunc::create(this, callfunc_selector(EndGameCustomModeNode::updateStar));
	auto delay = DelayTime::create(0.5f);
	if (m_iCountYellowStar < m_iYellowStar) {
		this->runAction(Sequence::create(delay->clone(), actionUpdateStar, NULL));
	}
}

void EndGameCustomModeNode::updateStar()
{
	if (m_iCountYellowStar < m_iYellowStar) {
		Sprite* pStarYellowImage = Sprite::create("Target-End-Game/star_win_small.png");

		if (m_iCountYellowStar == 0)
		{
			pStarYellowImage->setScale(1.4f);
			pStarYellowImage->setPosition(Point(220.0f + 1*100.0f, 760.0f));
		}
		else if(m_iCountYellowStar == 1)
		{
			pStarYellowImage->setPosition(Point(220.0f + 0*100.0f, 760.0f));
		}
		else if(m_iCountYellowStar == 2)
		{
			pStarYellowImage->setPosition(Point(220.0f + 2*100.0f, 760.0f));
		}

		this->addChild(pStarYellowImage);
		m_iCountYellowStar++;
		this->sequenceUpdateStar();
	}
}
*/
