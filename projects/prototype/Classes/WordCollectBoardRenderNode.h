#ifndef _WORD_COLLECT_BOARD_RENDER_NODE_H_
#define _WORD_COLLECT_BOARD_RENDER_NODE_H_

#include "cocos2d.h"
#include "GameBoardManager.h"
#include "GameWordManager.h"

using namespace cocos2d;

class WordCollectBoardRenderNode : public Layer //CCNodeRGBA
{
private:
	WordCollectBoardRenderNode();
public:
	static WordCollectBoardRenderNode* create();

	void draw() override;
	void update(float dt) override;	

	void UpdateList();

	void UnlockCharacter(int iCharacterID);
	std::string GetImageFileFromCharacterID(char iCharacter);	
	std::string GetImageFileFromCharacterID(int iCharacterID);	

	int GetNextCharacterID();

	bool onTouchBegan(Touch *pTouch, Event *pEvent) override;
private:
	void LoadWords();
	void GetWordIndex();

	virtual bool init();	

	void PlayVietnameseSpelling();
private:	
	//std::vector<CCLabelTTF*> m_LabelList;	
	int m_iWordCount;	
	Word m_WordList[_GDS_WORD_MAX_COUNT_];

	int m_iWordIndex;
	int m_iNextCharacterID;

	Sprite* m_LabelList[_GDS_WORD_MAX_LENGTH_];
	bool m_ActivedCharacterFlags[_GDS_WORD_MAX_LENGTH_];
	SpriteBatchNode* m_pBatchNode;
};


#endif //_WORD_COLLECT_BOARD_RENDER_NODE_H_