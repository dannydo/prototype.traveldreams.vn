#ifndef FLASH_CARD_NODE_H_
#define FLASH_CARD_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"
#include "GameDataStructure.h"	
#include "ButtonManagerNode.h"

class FlashCardNode	: public cocos2d::Node
{
public:
	FlashCardNode() {};
	virtual ~FlashCardNode();
	bool init() override;
	static FlashCardNode* createLayout(const Word& word);

	void addLayoutDescriptionWord();
	void addButtonPlaySoundWord();
	void addLayoutQuestion();

	cocos2d::Node* getNodeDescription() { return m_pNodeDescription; };
	void removeButtonManageQuestion();

private:

	void playVoiceWord(cocos2d::Object* pSender);
	void playVoiceSentence(cocos2d::Object* pSender);
	void clickHintQuestion(Object* pSender);

	cocos2d::Node* m_pNodeDescription;
	ButtonManagerNode* m_pButtonManagerNodeQuestion;
	ButtonNode* m_pButtonQuestion;

	cocos2d::Node* m_pFlashCardContent;
	cocos2d::LabelTTF* m_pLabelGold;

	const Word* m_pWord;
};

#endif;