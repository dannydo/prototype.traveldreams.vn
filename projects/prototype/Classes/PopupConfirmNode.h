#ifndef _POPUP_CONFIRM_NODE_H_
#define _POPUP_CONFIRM_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"


typedef enum {
	eNone = 0,
	eResetGame,
    eQuitGameCustomMode,
} PopupConfirmActionType;

typedef enum {
	eYesNo = 0,
    eOK,
	eConnectFacebook,
} PopupConfirmType;

class PopupConfirmNode : public CustomNode
{
public:
	PopupConfirmNode() {};
	virtual ~PopupConfirmNode();
	bool init() override;
	static PopupConfirmNode* createLayout(const std::string& sTitle, const std::string& sDescription, const PopupConfirmActionType& popupConfirmActionType, const PopupConfirmType& popupConfirmType);

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

private:
	void clickYes(cocos2d::Object* pSender);
	void clickNo(cocos2d::Object* pSender);
	void clickOk(cocos2d::Object* pSender);
	void clickLoginFacebook(cocos2d::Object* pSender);

	void runResetGame();
	void finishResetGame();

	std::string m_sTitle;
	std::string m_sDescription;
	PopupConfirmActionType m_popupConfirmActionType;
	PopupConfirmType m_popupConfirmType;
};

#endif;