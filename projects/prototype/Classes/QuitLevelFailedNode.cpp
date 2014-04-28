#include "QuitLevelFailedNode.h"
#include "ButtonManagerNode.h"
#include "GameTargetNode.h"
#include "GetMoreLifeNode.h"
#include "GameWordManager.h"

USING_NS_CC;

QuitLevelFailedNode::~QuitLevelFailedNode ()
{
}

QuitLevelFailedNode* QuitLevelFailedNode::createLayout(const int& iCurrentLevelId, const std::string& sChapterId)
{
	QuitLevelFailedNode* pQuitLevelFailed = new QuitLevelFailedNode();
	pQuitLevelFailed->m_iCurrentLevelId = iCurrentLevelId;
	pQuitLevelFailed->m_sChapterId = sChapterId;

	if(pQuitLevelFailed->init())
	{
		pQuitLevelFailed->autorelease();
		return pQuitLevelFailed;
	}

	CC_SAFE_DELETE(pQuitLevelFailed);
	return NULL;
}

bool QuitLevelFailedNode::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	this->removeAllChildren();
	
	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundBoard = Sprite::create("Target-End-Game/panel-level_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	Sprite* pLevelFailImage3 = Sprite::create("Target-End-Game/text_level_fail.png");
	pLevelFailImage3->setPosition(Point(320.0f, 747.0f));
	pLevelFailImage3->setScale(0.85);
	this->addChild(pLevelFailImage3);

	LabelBMFont *pLabelDecription = LabelBMFont::create("You pressed", "fonts/font_small_alert.fnt");
	pLabelDecription->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelDecription->setPosition(Point(320.0f, 650.0f));
	pLabelDecription->setScale(1.2f);
	this->addChild(pLabelDecription);

	LabelBMFont *pLabelDecription1 = LabelBMFont::create("the quit button", "fonts/font_small_alert.fnt");
	pLabelDecription1->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelDecription1->setPosition(Point(320.0f, 600.0f));
	pLabelDecription1->setScale(1.2f);
	this->addChild(pLabelDecription1);

	char sLevel[20];
	int iCalLevel = GameConfigManager::getInstance()->CountLevelOfPreviousChapters(m_sChapterId) + m_iCurrentLevelId;
	sprintf(sLevel, "Level %d", iCalLevel);
	LabelBMFont *pLabelLevel = LabelBMFont::create(sLevel, "fonts/font-bechic.fnt");
	pLabelLevel->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelLevel->setPosition(Point(320.0f, 870.0f));
	this->addChild(pLabelLevel);

	Sprite* pButtonRetrySprite = Sprite::create("Target-End-Game/btn_retry.png");
	ButtonNode* pButtonRetryNode = ButtonNode::createButtonSprite(pButtonRetrySprite, CC_CALLBACK_1(QuitLevelFailedNode::clickRetry, this));
	pButtonRetryNode->setPosition(Point(320.0f, 435.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* pButtonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(QuitLevelFailedNode::clickClose, this));
	pButtonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(pButtonRetryNode);
	pButtonManagerNode->addButtonNode(pButtonCloseNode);
	this->addChild(pButtonManagerNode);

	return true;
}

bool QuitLevelFailedNode::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void QuitLevelFailedNode::clickRetry(cocos2d::Object* pSender)
{
	UserTable::getInstance()->updateLife(0);
	if(UserTable::getInstance()->getUserInfo().iLife > 0)
	{
		GameWordManager* pGameWordManager = GameWordManager::getInstance();
		pGameWordManager->GenerateWordForNewLevel(m_sChapterId, m_iCurrentLevelId);

		GameTargetNode* pGameTargetNode = GameTargetNode::createLayout(pGameWordManager->GetMainWord(), m_iCurrentLevelId, m_sChapterId);
		this->getParent()->addChild(pGameTargetNode, 10);
		this->getParent()->removeChild(this);
	}
	else
	{
		GetMoreLifeNode* pGetMoreLife = GetMoreLifeNode::create();
		pGetMoreLife->setGetMoreLifeType(GetMoreLifeType::eClosePopup);
		this->getParent()->addChild(pGetMoreLife);
		this->getParent()->removeChild(this);
	}
}

void QuitLevelFailedNode::clickClose(cocos2d::Object* pSender)
{
	this->getParent()->removeChild(this);
}