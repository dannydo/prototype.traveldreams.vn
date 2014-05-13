#ifndef _TIME_COUNT_DOWN_NODE_H_
#define _TIME_COUNT_DOWN_NODE_H_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ExtendedNodes\Scale3Sprite.h"

using namespace cocos2d;
USING_NS_CC_EXT;

class TimeCountDownNode : public Node
{
public:
	static TimeCountDownNode* create(int iStageIndex, int iMaximumEnergy, int iEnergyLostPerSecond);

	inline void SetOutOfTimeCallback( std::function<void()> outOfTimeCallback) { m_OutOfTimeCallback = outOfTimeCallback;}
	inline int IsEnergyEmpty() { return (m_fCurrentEnergy <=0);}
	inline void SetSuspendingState(bool bIsSuspending) { m_bIsSuspendingDecreaseEnergy = bIsSuspending;}

	inline void Start() { m_bIsStarted = true;}

	void AddEnergy(int iIncrementEnergy) { 
		m_fStoredEnergyIncrement += iIncrementEnergy;
		m_fIncrementTimeRemain = 0.4f;
		/*if (m_fCurrentEnergy<=0) 
			return; 
		m_fCurrentEnergy += iIncrementEnergy; 
		if (m_fCurrentEnergy> m_fMaximumEnergy) 
			m_fCurrentEnergy = m_fMaximumEnergy;*/
	}

	void StopUpdate() { this->unscheduleUpdate();}

	void update(float fDeltaTime) override;
	void draw() override;
private:
	 void init(int iStageIndex, int iMaximumEnergy, int iEnergyLostPerSecond);
private:
	bool m_bIsStarted;
	float m_fCurrentEnergy;
	//float m_fNodeWidth, m_fNodeHeight, 
	float fEnergyWidth;

	float m_fMaximumEnergy;
	float m_fEnergyLostPersecond;
	float m_fStoredEnergyIncrement, m_fIncrementTimeRemain;
	bool m_bIsInNearOutOfEnergyState;
	bool m_bIsSuspendingDecreaseEnergy;

	std::function<void()> m_OutOfTimeCallback;


	SpriteBatchNode* m_pBatchNode;
	Sprite* m_pBackground;
	//Scale9Sprite* m_pMidEnergyBar;
	Scale3Sprite* m_pMidEnergyBar;
	Sprite* m_pMirrorMidEnergyBar;
	Sprite* m_pTimeIndicatorIcon;
};

#endif // _TIME_COUNT_DOWN_NODE_H_