#ifndef __PACKAGE_SERVICEW_H__
#define __PACKAGE_SERVICEW_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "InterfaceService.h"

class PackageService : public cocos2d::Object
{
public:
	PackageService();
	~PackageService();

	static PackageService* getInstance();
	static void releaseInstance();

	void getPackagePopular(const int& iStart, const int& iNumberRecords, const int& iTag);
	void setInterfaceServiceCallBack(InterfaceService* callBack);
	void removeInterfaceServiceCallBack();
	
private:
	void onHttpRequestCompleted(cocos2d::extension::HttpClient *sender, cocos2d::extension::HttpResponse *response);

	static PackageService* m_TrackingService;
	InterfaceService* m_callBack;

	static cocos2d::extension::HttpRequest* m_pRequest;
	static cocos2d::extension::HttpClient* m_pClient;

};
#endif