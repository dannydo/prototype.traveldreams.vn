#ifndef __SYNC_DATA_GAME_H__
#define __SYNC_DATA_GAME_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class SyncDataGame : public cocos2d::Object
{
public:
	SyncDataGame();
	~SyncDataGame();

	static SyncDataGame* getInstance();
	static void releaseInstance();

	void runSyncDataGame();
	inline const bool& getIsFinishSync() { return m_bIsFinishSync; };

private:
	void onHttpRequestCompleted(cocos2d::extension::HttpClient *sender, cocos2d::extension::HttpResponse *response);

	static SyncDataGame* m_SyncDataGame;

	static cocos2d::extension::HttpRequest* m_pRequest;
	static cocos2d::extension::HttpClient* m_pClient;

	bool m_bIsFinishSync;
};
#endif