#include "ScrollManager.h"

USING_NS_CC;

ScrollManager::ScrollManager()
{
	m_iTotalDataTouch = 0;
}

void ScrollManager::addDataToQueue(DataTouch dataTouch)
{
	if (m_iTotalDataTouch == 5)
	{
		for(int iIndex=0; iIndex<m_iTotalDataTouch-1; iIndex++)
		{
			m_Datatouchs[iIndex] = m_Datatouchs[iIndex+1];
		}
		dataTouch.lTime = getTimeLocalCurrent();
		m_Datatouchs[m_iTotalDataTouch-1] = dataTouch;
	}
	else
	{
		dataTouch.lTime = getTimeLocalCurrent();
		m_Datatouchs[m_iTotalDataTouch] = dataTouch;
		m_iTotalDataTouch++;
	}
}

DataTouch ScrollManager::getDistanceScrollY()
{
	DataTouch last = m_Datatouchs[m_iTotalDataTouch-1];
	for(int iIndex=m_iTotalDataTouch-2; iIndex>=0; iIndex--)
	{
		DataTouch dataTouch = m_Datatouchs[iIndex];
		float distance = last.point.y - dataTouch.point.y;
		float deltaTime = (last.lTime - dataTouch.lTime)/1000.0f; 

		if (fabsf(distance) > _MAX_DISTANCE_)
		{
			m_iTotalDataTouch=0;
			distance = distance + distance/deltaTime*_ACCELERATION_*1.0f;

			DataTouch dataTouch;
			dataTouch.fDeltaTime = deltaTime;
			dataTouch.point = Point(0.0f, distance);
			return dataTouch;
		}
		else if (iIndex == 0)
		{
			m_iTotalDataTouch=0;
			distance = distance + distance/deltaTime*_ACCELERATION_*1.0f;

			DataTouch dataTouch;
			dataTouch.fDeltaTime = deltaTime;
			dataTouch.point = Point(0.0f, distance);
			return dataTouch;
		}
	}
}

unsigned long ScrollManager::getTimeLocalCurrent()
{
	timeval now;
	gettimeofday(&now, NULL);
	unsigned long iCurrentTime = now.tv_sec*1000 + now.tv_usec/1000 ; //miliseconds
	return iCurrentTime;
}