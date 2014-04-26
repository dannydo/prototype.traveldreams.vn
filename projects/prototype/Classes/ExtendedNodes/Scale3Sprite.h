#ifndef _SCALE_3_SPRITE_H_
#define _SCALE_3_SPRITE_H_

#include "cocos2d.h"
 
using namespace cocos2d;

// special scale-3-sprite helper that work with a batch node
class Scale3Sprite : public NodeRGBA
{
public:
	static Scale3Sprite* create( SpriteBatchNode* pBatchNode, SpriteFrame* pParentSpriteFrame, float fInsetLeft, float fInsetRight, int iZOrder=0);

	virtual ~Scale3Sprite();

	virtual bool init(SpriteBatchNode* pBatchNode, SpriteFrame* pParentSpriteFrame, float fInsetLeft, float fInsetRight, int iZOrder);
	virtual void setContentSize(const Size & size) override;
	virtual void setVisible(bool bIsVisible) override;
private:
	Scale3Sprite();
private:
	SpriteBatchNode* m_pBatchNode;
	Sprite *m_pLeftSprite, *m_pCenterSprite, *m_pRightSprite;
};


#endif