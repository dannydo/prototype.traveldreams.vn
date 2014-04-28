#ifndef _QUIT_LEVEL_FAILED_NODE_H_
#define _QUIT_LEVEL_FAILED_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"
#include "Database\UserTable.h"

class QuitLevelFailedNode : public CustomNode
{
public:
	QuitLevelFailedNode() {};
	virtual ~QuitLevelFailedNode();
	bool init() override;
	static QuitLevelFailedNode* createLayout(const int& iCurrentLevelId, const std::string& sChapterId);

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

private:
	void createLayoutLevelFail();

	void clickRetry(cocos2d::Object* pSender);
	void clickClose(cocos2d::Object* pSender);

	int m_iCurrentLevelId;
	std::string m_sChapterId;
};

#endif;