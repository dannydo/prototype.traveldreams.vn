#ifndef _END_GAME_CUSTOM_MODE_NODE_H_
#define _END_GAME_CUSTOM_MODE_NODE_H_

#include "cocos2d.h"
#include "ButtonManagerNode.h"

class EndGameCustomModeNode : public cocos2d::Node
{
public:
	static EndGameCustomModeNode* createLayout(const int& iStage, const int& iTotalWord, const int& iDurationSecond, const std::string& sPackageId);
	bool init() override;
	void addYellowStar(const int& iYellowStar);

private:
	void menuRetryLevelCallBack(cocos2d::Object* sender);
	void menuCloseCallBack(cocos2d::Object* sender);

	void generateLayoutStart();

	void loopUpdateStar();
	void sequenceUpdateStar();
	void updateStar();

	cocos2d::String formatSecondsToDiaplay(const int& iSeconds);

	int m_iStage;
	int m_iTotalWord;
	int	m_iDurationSecond;
	int m_iYellowStar;
	int m_iCountYellowStar;
	std::string m_sPackageId;
};

#endif;