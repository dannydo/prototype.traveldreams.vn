#ifndef _BONUS_WORD_H_
#define _BONUS_WORD_H_

#include "cocos2d.h"
#include "GameDataStructure.h"

class BonusWordNode : public cocos2d::Node
{
public:
	static BonusWordNode* createLayout(const std::vector<Word> subWord);
	bool init() override;

	void addCharacterCollected(const char character);
	//void setVisibleWordNodes(const bool& bVisible);

private:
	std::vector<Word> m_subWord;
	cocos2d::Array* m_pSubWordNodes;
	//std::vector<int> m_subWordEnable;
};

#endif;