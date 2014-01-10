#ifndef _GAME_TARGET_H_
#define _GAME_TARGET_H_

#include "cocos2d.h"
#include "GameDataStructure.h"

class GameTargetNode : public cocos2d::Node
{
public:
	static GameTargetNode* createLayout(const Word& pMainWord, const int& iCurrentLevel);
	bool initLayout(const Word& pMainWord);

private:
	void menuPlayLevelCallBack();
	void menuCloseCallBack();
	void menuOpenDictCallBack();
	void onTouchBackground(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent);
	std::vector<int> generateArrayNumber(int iNumber);
	cocos2d::Node* generateLayoutLevel(int iLevel);

	int m_iCurrentLevel;

	cocos2d::Sprite* m_pBackgroundBoard; 
};

#endif;