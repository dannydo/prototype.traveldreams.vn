#ifndef _COMBO_COUNT_RENDER_NODE_H_
#define _COMBO_COUNT_RENDER_NODE_H_

#include "cocos2d.h"
#include "GameBoardManager.h"

#define _WCBRN_WORD_LENGTH_	5

using namespace cocos2d;

class ComboCountRenderNode : public Layer //CCNodeRGBA
{
private:
	ComboCountRenderNode();
public:
	static ComboCountRenderNode* create();

	void draw() override;
	void update(float dt) override;	

	void IncreaseMove();
	void UpdateList(int iTypeID, int iComboCount);	
private:
	void GenerateLabels();
	
private:	
	int m_ComboCountList[_COMBO_TYPE_COUNT_];	
	LabelTTF* m_LabelList[_COMBO_TYPE_COUNT_];	

	LabelTTF* m_pMoveCountLabel;
	int m_iMoveCount;
};


#endif //_COMBO_COUNT_RENDER_NODE_H_