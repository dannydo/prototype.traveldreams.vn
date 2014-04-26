#ifndef _OUT_OF_MOVES_NODE_H_
#define _OUT_OF_MOVES_NODE_H_

#include "cocos2d.h"
#include "GameDataStructure.h"
#include "ButtonManagerNode.h"

class OutOfMovesNode : public cocos2d::Node
{
public:
	static OutOfMovesNode* createLayout(const int& iScore, const Word& mainWord, const int& iCurrentLevel,  const std::string sChapterId);

	bool init() override;

private:
	void clickBuy(cocos2d::Object* sender);
	void clickGiveUp(cocos2d::Object* sender);

	Word m_mainWord;

	std::string m_sChapterId;
	int m_iScore;
	int m_iCurrentLevel;
};

#endif;