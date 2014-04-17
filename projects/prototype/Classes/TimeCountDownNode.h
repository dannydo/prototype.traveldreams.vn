#ifndef _TIME_COUNT_DOWN_NODE_H_
#define _TIME_COUNT_DOWN_NODE_H_

#include "cocos2d.h"

using namespace cocos2d;

class TimeCountDownNode : public Node
{
public:
	static TimeCountDownNode* create(int iMaximumEnergy, int iEnergyLostPerSecond);

	inline void SetOutOfTimeCallback( std::function<void()> outOfTimeCallback) { m_OutOfTimeCallback = outOfTimeCallback;}
	void AddEnergy(int iIncrementEnergy) { m_fCurrentEnergy += iIncrementEnergy;}
	void StopUpdate() { this->unscheduleUpdate();}

	void update(float fDeltaTime) override;
	void draw() override;
private:
	 void init(int iMaximumEnergy, int iEnergyLostPerSecond);
private:
	float m_fCurrentEnergy;
	//float m_fNodeWidth, m_fNodeHeight, 
	float fEnergyWidth;

	float m_fMaximumEnergy;
	float m_fEnergyLostPersecond;

	std::function<void()> m_OutOfTimeCallback;
};

#endif // _TIME_COUNT_DOWN_NODE_H_