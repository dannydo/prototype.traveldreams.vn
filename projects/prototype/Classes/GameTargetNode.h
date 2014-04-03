#ifndef _GAME_TARGET_H_
#define _GAME_TARGET_H_

#include "cocos2d.h"
#include "GameDataStructure.h"

class GameTargetNode : public cocos2d::Node
{
public:
	static GameTargetNode* createLayout(const Word& pMainWord, const int& iCurrentLevel, const std::string sChapterId);
	bool initLayout(const Word& pMainWord);

private:
	void menuPlayLevelCallBack(cocos2d::Object* sender);
	void menuCloseCallBack(cocos2d::Object* sender);
	
	std::vector<int> generateArrayNumber(int iNumber);
	cocos2d::Node* generateLayoutLevel(int iLevel);

	void generateLayoutStartAndBonusQuest();

	cocos2d::SpriteBatchNode* m_pSpriteBatchNode;
	int m_iCurrentLevel;
	std::string m_sChapterId;
};

#endif;