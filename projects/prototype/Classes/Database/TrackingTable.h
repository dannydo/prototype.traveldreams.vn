#ifndef __TRACKING_TABLE_H__
#define __TRACKING_TABLE_H__

#include "cocos2d.h"
#include "WordTable.h"

#define _LIMIT_PUSH_TRACKING_TO_SERVER_		50

typedef enum EventTracking_e {
	_ET_PLAY_LEVEL_ = 0,
	_ET_PLAY_ADVANCE_,
	_ET_OPEN_GAME_,
	_ET_PLAY_MINI_GAME_,
	_ET_PLAY_REVISE_GAME_,
};

struct TrackingInfo
{
	std::string sDeviceJson;
	std::string sPlayerJson;
	EventTracking_e eEventTracking;
	std::string sDataJson;
	unsigned long uStartTime;

	TrackingInfo()
	{
		sDeviceJson = "";
		sPlayerJson = "";
		uStartTime = 0;
		eEventTracking = EventTracking_e::_ET_OPEN_GAME_;
		sDataJson = "";
	}
};

class TrackingTable
{
public:
	static TrackingTable* getInstance();
	TrackingTable();
	bool init();
	static void releaseInstance();

	bool insertTracking(const TrackingInfo& trackingInfo);
	bool deleteTrackingDonePushToServer();

	std::string	genrateJsonTrackingPushToServer();
	std::string getStringEventTracking(EventTracking_e eEventTracking);

	bool trackingOpenGame();
	bool trackingPlayLevelNormalMode(const unsigned long startTime, const int& iLevel, const std::string sChapterId, std::string sWordId, 
		const char* sStatusGame, const int& iMoveLeft, const int& iLeterLeft, const int& iScore, const int& iStar, 
		const int& iTotalMove, const int& iTotalBonusQuest, const int& iBonusQuestCompleted);
	bool trackingPlayAdvanceMode(const unsigned long startTime, const std::string& sPackageId, const int& iStage, const std::vector<std::string>& CSWordIds);
	bool trackingPlayMiniGame(const unsigned long startTime, const int& iWordChooseRight, const int& iTotalWord, const std::vector<std::string>& WordIds, const std::vector<int>& ResultChooses);
	bool trackingPlayReviseGame(const unsigned long startTime, const int& iStage, const int& iBestStage, const std::vector<std::string>& WordIds);

private:
	std::string getDeviceJson();
	std::string getPlayerJson();

	static TrackingTable* m_TrackingTable;
};
#endif