#ifndef FLASH_CARD_NODE_H_
#define FLASH_CARD_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"
#include "GameDataStructure.h"

class FlashCardNode	: public cocos2d::Node
{
public:
	FlashCardNode() {};
	virtual ~FlashCardNode();
	bool init() override;
	static FlashCardNode* createLayout(const Word& word);

private:
	Word m_Word;
};

#endif;