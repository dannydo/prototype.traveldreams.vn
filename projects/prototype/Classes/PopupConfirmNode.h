#ifndef _POPUP_CONFIRM_NODE_H_
#define _POPUP_CONFIRM_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"


typedef enum {
	eResetGame = 0,
    eQuitGameCustomMode,
} PopupConfirmType;

class PopupConfirmNode : public CustomNode
{
public:
	PopupConfirmNode() {};
	virtual ~PopupConfirmNode();
	bool init() override;
	static PopupConfirmNode* createLayout(const std::string& sTitle, const PopupConfirmType& popupConfirmType);

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

private:
	void clickYes(cocos2d::Object* pSender);
	void clickNo(cocos2d::Object* pSender);

	void runResetGame();
	void finishResetGame();

	std::string m_sTitle;
	PopupConfirmType m_popupConfirmType;
};

#endif;