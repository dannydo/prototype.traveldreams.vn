#include "PopupUnderContructionNode.h"
#include "ButtonManagerNode.h"

USING_NS_CC;

PopupUnderContructionNode::~PopupUnderContructionNode ()
{
}

PopupUnderContructionNode* PopupUnderContructionNode::createLayout()
{
	PopupUnderContructionNode* pPopupConfirm = new PopupUnderContructionNode();

	if(pPopupConfirm->init())
	{
		pPopupConfirm->autorelease();
		return pPopupConfirm;
	}

	CC_SAFE_DELETE(pPopupConfirm);
	return NULL;
}

bool PopupUnderContructionNode::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundBoard = Sprite::create("popup/popup-underconstruction.png");
	pBackgroundBoard->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackgroundBoard);
	
	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	Sprite* pButtonOkSprite = Sprite::create("popup/ok.png");
	ButtonNode* pButtonOkNode = ButtonNode::createButtonSprite(pButtonOkSprite, CC_CALLBACK_1(PopupUnderContructionNode::clickOk, this));
	pButtonOkNode->setPosition(Point(320.0f, 360.0f));
	pButtonManagerNode->addButtonNode(pButtonOkNode);
	
	return true;
}

bool PopupUnderContructionNode::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void PopupUnderContructionNode::clickOk(cocos2d::Object* pSender)
{
	this->getParent()->removeChild(this);
}