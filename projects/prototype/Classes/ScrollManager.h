#ifndef _SCROLL_MANAGER_H_
#define _SCROLL_MANAGER_H_

#include "cocos2d.h"

#define _MAX_DISTANCE_ 20
#define _ACCELERATION_ 0.1

struct DataTouch
{
	cocos2d::Point point;
	unsigned long lTime;
	float fDeltaTime;
};

class ScrollManager
{
public:
	ScrollManager();
	void addDataToQueue(DataTouch dataTouch);
	DataTouch getDistanceScrollY();

private:
	unsigned long getTimeLocalCurrent();

	DataTouch m_Datatouchs[5];
	int m_iTotalDataTouch;
};

#endif;