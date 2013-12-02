#ifndef _WORD_COLLECT_BOARD_RENDER_NODE_H_
#define _WORD_COLLECT_BOARD_RENDER_NODE_H_

#include "cocos2d.h"
#include "GameBoardManager.h"

#define _WCBRN_WORD_LENGTH_	5

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
	static std::string GetImageFileFromCharacterID(int iCharacterID);	

	int GetNextCharacterID();
private:
	void GenerateLabels();
	int m_iNextCharacterID;
private:	
	//std::vector<CCLabelTTF*> m_LabelList;	

	Sprite* m_LabelList[_WCBRN_WORD_LENGTH_];
	bool m_ActivedCharacterFlags[_WCBRN_WORD_LENGTH_];
	SpriteBatchNode* m_pBatchNode;
};


#endif //_WORD_COLLECT_BOARD_RENDER_NODE_H_