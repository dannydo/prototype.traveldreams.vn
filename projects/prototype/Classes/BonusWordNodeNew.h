#ifndef _BONUS_WORD_NEW_H_
#define _BONUS_WORD_NEW_H_

#include "cocos2d.h"
#include "GameDataStructure.h"

class BonusWordNodeNew : public cocos2d::Node
{
public:
	~BonusWordNodeNew();

	static BonusWordNodeNew* createLayout(const std::vector<Word> bonusWords);
	bool init() override;

	void addLetter(const unsigned char letter);

private:
	void popupBonusWordCallBack(cocos2d::Object* pSender);
	void updateLetterDisplay(int iIndexWord);
	void setVisibleWordNode(const bool& bVisible);
	cocos2d::Node* createNodeBonusWord(Word word);


	cocos2d::Array* m_LabelLetterDisplays;

	std::vector<Word> m_bonusWords;
	std::vector<std::vector<int>> m_WordLetterIndex;
};

#endif;