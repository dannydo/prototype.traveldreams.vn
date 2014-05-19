#include "PopupThanksNode.h"
#include "ButtonManagerNode.h"
#include "Database\CSPackageTable.h"
#include "MainMenuScene.h"

USING_NS_CC;

PopupThanksNode::~PopupThanksNode ()
{
}

PopupThanksNode* PopupThanksNode::createLayout()
{
	PopupThanksNode* pPopupConfirm = new PopupThanksNode();

	if(pPopupConfirm->init())
	{
		pPopupConfirm->autorelease();
		return pPopupConfirm;
	}

	CC_SAFE_DELETE(pPopupConfirm);
	return NULL;
}

bool PopupThanksNode::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundBoard = Sprite::create("popup/popup-thankyou.png");
	pBackgroundBoard->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackgroundBoard);
	
	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	Sprite* pButtonOkSprite = Sprite::create("LoadingAndMainMenu/btn_custom.png");
	ButtonNode* pButtonOkNode = ButtonNode::createButtonSprite(pButtonOkSprite, CC_CALLBACK_1(PopupThanksNode::clickAdvanceMode, this));
	pButtonOkNode->setPosition(Point(320.0f, 360.0f));
	pButtonManagerNode->addButtonNode(pButtonOkNode);

	Sprite* pButtonCloseSprite = Sprite::create("popup/btn_close.png");
	ButtonNode* pButtonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(PopupThanksNode::clickClose, this));
	pButtonCloseNode->setPosition(Point(570.0f, 647.0f));
	pButtonManagerNode->addButtonNode(pButtonCloseNode);
	
	return true;
}

bool PopupThanksNode::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void PopupThanksNode::clickClose(cocos2d::Object* pSender)
{
	this->getParent()->removeChild(this);
}

void PopupThanksNode::clickAdvanceMode(cocos2d::Object* pSender)
{
	Breadcrumb::getInstance()->resetSceneNodeToMainMenu();
	MainMenuScene* scene = MainMenuScene::create();
	Director::getInstance()->replaceScene(scene);
}