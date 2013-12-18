#ifndef _GAME_TARGET_H_
#define _GAME_TARGET_H_

#include "cocos2d.h"

class GameTargetNode : public cocos2d::Node
{
public:
	static GameTargetNode* createLayout(char* pMainWord, const int& iCurrentLevel);
	bool initLayout(char* pMainWord);

private:
	void menuPlayLevelCallBack();
	void menuCloseCallBack();
	void menuOpenDictCallBack();
	void onTouchBackground(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent);

	int m_iCurrentLevel;

	cocos2d::Sprite* m_pBackgroundBoard; 
};

#endif;