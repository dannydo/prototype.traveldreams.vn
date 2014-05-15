#ifndef _END_GAME_CUSTOM_MODE_NODE_H_
#define _END_GAME_CUSTOM_MODE_NODE_H_

#include "cocos2d.h"
#include "ButtonManagerNode.h"
#include "Database\CSPackageTable.h"

class EndGameCustomModeNode : public cocos2d::Node
{
public:
	static EndGameCustomModeNode* createLayout(const int& iStage, const int& iWordNew, const std::string& sPackageId);
	bool init() override;
	void addYellowStar(const int& iYellowStar);

private:
	void clickRetry(cocos2d::Object* sender);
	void clickClose(cocos2d::Object* sender);

	/*
	void loopUpdateStar();
	void sequenceUpdateStar();
	void updateStar();
	*/

	cocos2d::String formatSecondsToDiaplay(const int& iSeconds);

	int m_iStage;
	int m_iWordNew;

	std::string m_sPackageId;
	CSPackageInfo m_csPackageInfo;
};

#endif;