#ifndef _CONFIRM_QUIT_LEVEL_NODE_H_
#define _CONFIRM_QUIT_LEVEL_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"
#include "Database\UserTable.h"

class ConfirmQuitLevelNode : public CustomNode
{
public:
	ConfirmQuitLevelNode() {};
	virtual ~ConfirmQuitLevelNode();
	bool init() override;
	static ConfirmQuitLevelNode* createLayout(const int& iCurrentLevelId, const std::string& sChapterId);

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

private:

	void clickYes(cocos2d::Object* pSender);
	void clickNo(cocos2d::Object* pSender);

	int m_iCurrentLevelId;
	std::string m_sChapterId;
};

#endif;