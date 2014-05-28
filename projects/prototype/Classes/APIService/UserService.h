#ifndef __USER_SERVICE_H__
#define __USER_SERVICE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "InterfaceService.h"

class UserService : public cocos2d::Object
{
public:
    static UserService* getInstance();
    UserService();

    static void releaseInstance();

    void checkUserFacebook(const std::string strAccessToken, const int& iUserId, const int& iTag);
    void getLeaderBoardLevel(const std::string& sChapterId, const int& iLevel, const int& iTag);
    void getLeaderBoardAdvanceMode(const std::string& sPackageId, const int& iTag);
    void disableUser(const std::string& sUserToken);
    void getVersionGame();

    void addCallBackList(InterfaceService* callBack);
    void removeCallBackList(InterfaceService *callBack);

    cs::JsonDictionary* parseStringToJson(const char* sJson);

private:
    void onHttpRequestCompleted(cocos2d::extension::HttpClient *sender, cocos2d::extension::HttpResponse *response);
    void onCheckUserFacebookCompleted(cocos2d::extension::HttpClient *sender, cocos2d::extension::HttpResponse *response);
    void onGetVersionGameCompleted(cocos2d::extension::HttpClient *sender, cocos2d::extension::HttpResponse *response);

    static UserService* m_UserService;
    std::vector<InterfaceService*> m_callBackList;

    static cocos2d::extension::HttpRequest* m_pRequest;
    static cocos2d::extension::HttpClient* m_pClient;

    int m_iCurrentLevel;
    std::string m_sCurrentChapter;
    std::string m_sDataLeaderBoard;

    std::string m_sCurrentPackageId;
    std::string m_sDataLeaderBoardAdvanceMode;
};
#endif
