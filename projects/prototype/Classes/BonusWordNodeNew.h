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
	void update(float dt) override;

	void addLetter(const unsigned char letter);
	float displayEffect(const float& fTimeDelay);
	void closeBonusWord();

private:
	void popupBonusWordCallBack(cocos2d::Object* pSender);
	void updateLetterDisplay();
	void emptyArrayLetter();
	void updateLetterCollectForWord();
	void playEffectLetter(cocos2d::LabelTTF* pLabel);

	void playEffectFinishCollectWord();
	bool checkFinishCollectWord(const int& iIndexWord);
	void removeLabelFinishColectWord();
	float calculatorDelayTime();
	void updateTagSpriteMark(Node* pSender);
	
	cocos2d::Node* createNodeBonusWord(Word word);

	cocos2d::Array* m_pMarks;
	cocos2d::Array* m_LabelLetterDisplays;
	cocos2d::LabelTTF* m_pLabelEffect;
	cocos2d::LabelTTF* m_pLabelEffectFinish;

	std::vector<Word> m_bonusWords;
	std::vector<std::vector<int>> m_WordLetterIndex;
	std::vector<int> m_CompletedUnlockBonusWprds;
	std::vector<unsigned char> m_letters;

	int m_iCountWord;
	int m_iCountLetterWord;
	int m_iCountLetter;

	float m_fTimeAutoHiddenPopup;
	float m_fDeltaUpdate;

	bool m_bRunningCollectWord;
};

#endif;