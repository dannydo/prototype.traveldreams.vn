#ifndef _BUTTON_NODE_H_
#define _BUTTON_NODE_H_

#include "cocos2d.h"



typedef std::function<void(cocos2d::Object*)> buttonNodeCallback;

class ButtonNode : public cocos2d::Node
{

public:
	ButtonNode() {};
	virtual ~ButtonNode();
	static ButtonNode* createButtonSprite(cocos2d::Sprite *pSpriteButton, const buttonNodeCallback& callBack);
	static ButtonNode* createButtonSprite(cocos2d::Sprite *pSpriteButton, cocos2d::Sprite *pSpriteButtonActive, const buttonNodeCallback& callBack);
	bool init() override;

	inline cocos2d::Sprite* getButtonSprite() { return m_pButtonSprite; };
	inline cocos2d::Sprite* getButtonSpriteActive() { return m_pButtonSpriteActive; };
	inline bool getStateActive() { return m_bIsActive; };
	inline buttonNodeCallback getCallBack() { return m_callBack; };

	void setStateActive(const bool& isActive);

private:
	cocos2d::Sprite* m_pButtonSprite;
	cocos2d::Sprite* m_pButtonSpriteActive;
	buttonNodeCallback m_callBack;

	bool m_bIsActive;
	bool m_bHaveStateButton;
};

#endif;