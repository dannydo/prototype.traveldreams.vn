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
	void addCharacterToWordNotification(const char character);
	inline cocos2d::Node* getLayoutNotification() { return m_pLayoutNotificationNode; };
	
	cocos2d::Node* createButtonShowPopupBonus();

private:
	cocos2d::Node* createLayoutWord(const Word word);
	void loopNotification();
	void updateLayoutNotification();
	void setVisibleNotification(const bool& bVisible);
	void menuShowPopupCallBack();
	
	std::vector<Word> m_subWord;
	cocos2d::Node* m_pLayoutNotificationNode;
	
	int m_iNotification;
	char m_cCharacter;
};

#endif;