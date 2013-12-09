#ifndef _STATUS_H_
#define _STATUS_H_

#include "cocos2d.h"

class StatusLayer : public cocos2d::Layer
{
public:
	bool init() override;
	void update(float dt) override;
	void draw() override;
	void updateScore();

	CREATE_FUNC(StatusLayer);

	void setCurrentScore(const int& iCurrentScore);
	void setScoreForStar(const float& iScore1Star, const float& iScore2Star, const float& iScore3Star, const float& iMaxScoreLevel);
	void setCurrentMove(const int& iCurrentMove);

private:
	void loopUpdateScore();
	void sequenceUpdateScore();
	void setPositionForStar(const float& iScoreStar, cocos2d::Sprite* pStar);
	void clippingNodeCircle();
	std::vector<int> generateArrayNumber(int iNumber);
	void generateLayoutMove();
	void generateLayoutScore(int iScore);

	cocos2d::SpriteBatchNode* m_pBatchNode;

	cocos2d::Sprite* m_pStarYellow1;
	cocos2d::Sprite* m_pStarYellow2;
	cocos2d::Sprite* m_pStarYellow3;
	cocos2d::Sprite* m_pStarBlack1;
	cocos2d::Sprite* m_pStarBlack2;
	cocos2d::Sprite* m_pStarBlack3;
	cocos2d::Sprite* m_pMarkCircle;

	cocos2d::Node* m_pMove;
	cocos2d::Node* m_pScore;

	cocos2d::Point m_centerCircle;

	int m_iCurrentScore;
	int m_iScoreOld;
	int m_iCurrentMove;

	float m_fRadiusCircle;
	float m_iScore1Star;
	float m_iScore2Star;
	float m_iScore3Star;  
	float m_iMaxScoreLevel;
};

#endif;