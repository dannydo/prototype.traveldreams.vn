#ifndef _LEADER_BOARD_H_
#define _LEADER_BOARD_H_

#include "cocos2d.h"

class LeaderBoardtNode : public cocos2d::Node
{
public:
	LeaderBoardtNode() {};
	virtual ~LeaderBoardtNode();
	bool init() override;

	CREATE_FUNC(LeaderBoardtNode);

	void addButtonShowMe();

private:
	cocos2d::Node* createItem(const int& iScore, const char* sName, const char* sPathAvatar, const int& iINdex);
	void menuShowMeCallback();

private:
	
};

#endif;