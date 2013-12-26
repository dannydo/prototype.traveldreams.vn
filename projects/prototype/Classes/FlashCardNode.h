#ifndef _FLASH_CARD_H_
#define _FLASH_CARD_H_

#include "cocos2d.h"
#include "GameDataStructure.h"

class FlashCardNode : public cocos2d::Node
{
public:
	static FlashCardNode* createLayout(const Word word);
	bool init() override;

	void displayEffect(const float& fTimeDisplay, const float& fTimeEffect);

private:
	void removeLayout();

	cocos2d::Sprite* m_pBackground;
	cocos2d::Sprite* m_pFlashCard;
	cocos2d::LabelTTF* m_pTitleWord;
	cocos2d::LabelTTF* m_pMeaningWord;

	Word m_word;
};

#endif;