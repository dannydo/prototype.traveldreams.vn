#include "FunctionCommon.h"

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

String formatNumber (int n)
{
	String sNumber="";
    int n2 = 0;
    int scale = 1;

    if (n < 0) {
        sNumber.append ("-");
        n = -n;
    }

    while (n >= 1000)
	{
        n2 = n2 + scale * (n % 1000);
        n /= 1000;
        scale *= 1000;
    }

	sNumber.appendWithFormat("%d", n);

    while (scale != 1)
	{
        scale /= 1000;
        n = n2 / scale;
        n2 = n2  % scale;
		sNumber.appendWithFormat(",%03d", n);
    }

	return sNumber;
}

unsigned long getTimeLocalCurrent()
{
	time_t  timev;
	time(&timev);

	return timev;  //seconds
}

unsigned long getSecondsTimeLocalOfDay()
{
	timeval now;
	gettimeofday(&now, NULL);
	unsigned long iCurrentTime = now.tv_sec + now.tv_usec/1000000 ; //seconds
	return iCurrentTime;
}

unsigned long getMilisecondsTimeLocalOfDay()
{
	timeval now;
	gettimeofday(&now, NULL);
	unsigned long iCurrentTime = now.tv_sec*1000 + now.tv_usec/1000 ; //miliseconds
	return iCurrentTime;
}

String formatHMSToDisplay(const int& iSeconds)
{
	String str = "";

	if(iSeconds/3600 < 10)
		str.append("0");
	str.appendWithFormat("%d:", iSeconds/3600);
	int iSecondsTemp = iSeconds%3600;

	if(iSecondsTemp/60 < 10)
		str.append("0");
	str.appendWithFormat("%d:", iSecondsTemp/60);

	if(iSecondsTemp%60 < 10)
		str.append("0");	
	str.appendWithFormat("%d", iSecondsTemp%60);

	return str;
}

void openURL(const char* pszUrl)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
	FacebookManager::getInstance()->openURL(pszUrl);
#else (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	ShellExecuteA(NULL, "open", pszUrl, NULL, NULL, SW_SHOWNORMAL);
#endif
}