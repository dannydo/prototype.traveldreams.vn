#ifndef __TRACKING_SERVICEW_H__
#define __TRACKING_SERVICEW_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class TrackingService : public cocos2d::Object
{
public:
	TrackingService();
	~TrackingService();

	static TrackingService* getInstance();
	static void releaseInstance();

	void pushTrackingToServer();
	inline const bool& getIsFinishSync() { return m_bIsFinishTracking; };

private:
	void onHttpRequestCompleted(cocos2d::extension::HttpClient *sender, cocos2d::extension::HttpResponse *response);

	static TrackingService* m_TrackingService;

	static cocos2d::extension::HttpRequest* m_pRequest;
	static cocos2d::extension::HttpClient* m_pClient;

	bool m_bIsFinishTracking;
};
#endif