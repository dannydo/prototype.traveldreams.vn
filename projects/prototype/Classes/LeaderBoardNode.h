#ifndef _LEADER_BOARD_H_
#define _LEADER_BOARD_H_

#include "cocos2d.h"
#include "CustomNode.h"
#include "APIService\InterfaceService.h" 
#include "ScrollManager.h"

struct LeaderBoardInfo
{
	std::string sName;
	std::string sUrlImage;
	int iScore;
	int iStar;
	int iIndex;
};

class LeaderBoardtNode : public CustomNode, InterfaceService
{
public:
	LeaderBoardtNode() {};
	virtual ~LeaderBoardtNode();
	bool init() override;
	static LeaderBoardtNode* createLayout(const int& iLevel);

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchCustomNodeMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override; 
	void onTouchCustomNodeEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	
	void resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey) override;

private:
	void addItemToSlide(const int& iScore, const char* sName, const char* sFacebookId, const int& iRank, const int& iIndex);
	void parseJsonToLeadeBoard(cs::JsonDictionary* pJsonItem, const int& iRank, const int& iIndex);

	cocos2d::Node* m_pSlideShow;
	cocos2d::Sprite* m_pSpriteBackground;

	float m_fBeginX;
	float m_fXMoved;
	float m_fMinPositionLeft;

	int m_iLevel;
	int m_iLeaderBoardCount;
	int m_arrIndex[6];
	
	ScrollManager* m_pScrollManager;
	bool m_bIsSwipe;
};

#endif;