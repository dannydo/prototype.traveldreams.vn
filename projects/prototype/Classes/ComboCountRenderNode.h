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

	void UpdateGemList(int iGemID, int iCount);	
	void UpdateComboList(int iTypeID, int iComboCount);	
	void UpdateBonusWordQuest(int iRemainLetterCount);
	void GenerateLabels(int iType, int iValue =0);
	void CompleteQuest(int iType);
private:	
	int m_ComboCountList[_COMBO_TYPE_COUNT_];	
	int m_DestroyGemCountList[_MAX_GEM_ID_];
	int m_iTotalBonusLetterLength;

	LabelTTF* m_QuestLabelList[3];
	LabelTTF* m_LabelGemCountList[_MAX_GEM_ID_];	
	LabelTTF* m_LabelComboCountList[_COMBO_TYPE_COUNT_];	
	LabelTTF* m_LabelBonusWord;
};


#endif //_COMBO_COUNT_RENDER_NODE_H_