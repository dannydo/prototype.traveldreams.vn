#ifndef _LEADER_BOARD_ADVANCE_MODE_NODE_H_
#define _LEADER_BOARD_ADVANCE_MODE_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"
#include "APIService\InterfaceService.h" 
#include "ScrollManager.h"
#include "ButtonManagerNode.h"

struct LeaderBoardAdvanceModeInfo
{
	std::string sName;
	std::string sUrlImage;
	int iScore;
	int iStar;
	int iIndex;
};

class LeaderBoardAdvanceModeNode : public CustomNode, InterfaceService
{
public:
	LeaderBoardAdvanceModeNode() {};
	virtual ~LeaderBoardAdvanceModeNode();
	bool init() override;
	static LeaderBoardAdvanceModeNode* createLayout(const std::string sPackageId);

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchCustomNodeMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override; 
	void onTouchCustomNodeEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	
	void resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey) override;

private:
	void addItemToSlide(const int& iStage, const char* sName, const char* sFacebookId, const int& iRank);
	void parseJsonToLeadeBoard(cs::JsonDictionary* pJsonItem, const int& iRank);
	void clickNo1(cocos2d::Object* pSender);
	void clickMe(cocos2d::Object* pSender);
	void clickInviteFriends(cocos2d::Object* pSender);
	
	cocos2d::Node* m_pSlideShow;
	cocos2d::Sprite* m_pSpriteBackground;

	cocos2d::Point m_pointNo1;
	cocos2d::Point m_pointMe;

	float m_fBeginX;
	float m_fXMoved;
	float m_fMinPositionLeft;

	int m_iLeaderBoardCount;

	std::string m_sPackageId;
	
	ScrollManager* m_pScrollManager;
	ButtonManagerNode* m_pButtonManagerNode;

	int m_iConnectServer;
};

#endif;
