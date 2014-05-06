#include "ScrollManager.h"
#include "FunctionCommon.h"

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
	if(m_iTotalDataTouch > 1)
	{
		DataTouch last = m_Datatouchs[m_iTotalDataTouch-1];
		for(int iIndex=m_iTotalDataTouch-2; iIndex>=0; iIndex--)
		{
			DataTouch dataTouch = m_Datatouchs[iIndex];
			float distanceY = last.point.y - dataTouch.point.y;
			float deltaTime = (last.lTime - dataTouch.lTime)/1000.0f; 

			if (fabsf(distanceY) > _MAX_DISTANCE_)
			{
				m_iTotalDataTouch=0;
				distanceY = distanceY + distanceY/deltaTime*_ACCELERATION_*1.0f;

				DataTouch dataTouch;
				dataTouch.fDeltaTime = deltaTime;
				dataTouch.point = Point(0.0f, distanceY);
				return dataTouch;
			}
			else if (iIndex == 0)
			{
				m_iTotalDataTouch=0;
				distanceY = distanceY + distanceY/deltaTime*_ACCELERATION_*1.0f;

				DataTouch dataTouch;
				dataTouch.fDeltaTime = deltaTime;
				dataTouch.point = Point(0.0f, distanceY);
				return dataTouch;
			}
		}
	}

	return DataTouch();
}

DataTouch ScrollManager::getDistanceScrollX()
{
	if(m_iTotalDataTouch > 1)
	{
		DataTouch last = m_Datatouchs[m_iTotalDataTouch-1];
		for(int iIndex=m_iTotalDataTouch-2; iIndex>=0; iIndex--)
		{
			DataTouch dataTouch = m_Datatouchs[iIndex];
			float distanceX = last.point.x - dataTouch.point.x;
			float deltaTime = (last.lTime - dataTouch.lTime)/1000.0f; 

			if (fabsf(distanceX) > _MAX_DISTANCE_)
			{
				m_iTotalDataTouch=0;
				distanceX = distanceX + distanceX/deltaTime*_ACCELERATION_*1.0f;

				DataTouch dataTouch;
				dataTouch.fDeltaTime = deltaTime;
				dataTouch.point = Point(distanceX, 0.0f);
				return dataTouch;
			}
			else if (iIndex == 0)
			{
				m_iTotalDataTouch=0;
				distanceX = distanceX + distanceX/deltaTime*_ACCELERATION_*1.0f;

				DataTouch dataTouch;
				dataTouch.fDeltaTime = deltaTime;
				dataTouch.point = Point(distanceX, 0.0f);
				return dataTouch;
			}
		}
	}

	return DataTouch();
}