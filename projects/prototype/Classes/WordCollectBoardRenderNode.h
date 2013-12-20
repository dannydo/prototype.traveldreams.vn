#ifndef _WORD_COLLECT_BOARD_RENDER_NODE_H_
#define _WORD_COLLECT_BOARD_RENDER_NODE_H_

#include "cocos2d.h"
#include "GameBoardManager.h"
#include "GameWordManager.h"
#include "cocos-ext.h"

using namespace cocos2d;
USING_NS_CC_EXT;

using namespace cocos2d::extension::armature;

class WordCollectBoardRenderNode : public Layer //CCNodeRGBA
{
private:
	WordCollectBoardRenderNode();
public:
	static WordCollectBoardRenderNode* create();

	void GenerateLabels();//call this after generate level data
	//void draw() override;
	void update(float dt) override;	

	//void UpdateList();

	void UnlockCharacter(int iLetterIndex);
	std::string GetImageFileFromLetter(unsigned char iLetter);	
	std::string GetImageInGemFileFromLetter(unsigned char iLetter);	

	//int GetNextCharacterID();

	bool onTouchBegan(Touch *pTouch, Event *pEvent) override;

	void PlaySpellingSound();
private:
	//void LoadWords();
	//void GetWordIndex();

	virtual bool init();		

	void PlayVietnameseSpelling();

	void PlayCharacterAnim(int iAnimIndex, bool bIsLoop = false);
private:	
	//std::vector<CCLabelTTF*> m_LabelList;	
	/*int m_iWordCount;	
	Word m_WordList[_GDS_WORD_MAX_COUNT_];

	int m_iWordIndex;
	int m_iNextCharacterID;
	
	bool m_ActivedCharacterFlags[_GDS_WORD_MAX_LENGTH_];*/
	const Word* m_pMainWord;

	Sprite* m_LabelList[_GDS_WORD_MAX_LENGTH_];

	float m_fStartPositionX;
	float m_LabelXPositionList[_GDS_WORD_MAX_LENGTH_];

	SpriteBatchNode* m_pBatchNode;

	unsigned long m_iPreviousMainWordTapTime;

	Armature* m_pCharacter;
};


#endif //_WORD_COLLECT_BOARD_RENDER_NODE_H_