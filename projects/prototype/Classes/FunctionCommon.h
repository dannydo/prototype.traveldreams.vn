#include "cocos2d.h"

#define _URL_GOOLE_PLAY_APP_ "https://play.google.com/store/apps/details?id=com.kiss_concept.prototype"

cocos2d::String formatNumber (int n);
unsigned long getTimeLocalCurrent();
unsigned long getSecondsTimeLocalOfDay();
unsigned long getMilisecondsTimeLocalOfDay();
cocos2d::String formatHMSToDisplay(const int& iSeconds);
void openURL(const char* pszUrl);