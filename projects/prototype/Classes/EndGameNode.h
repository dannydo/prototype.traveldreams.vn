#ifndef _END_GAME_H_
#define _END_GAME_H_

#include "cocos2d.h"
#include "GameDataStructure.h"
#include "LeaderBoardNode.h"	
#include "Database\LevelTable.h"
#include "Database\ChapterTable.h"

class EndGameNode : public cocos2d::Node
{
public:
	static EndGameNode* createLayoutWin(const int& iScore, const Word& mainWord, const int& iCurrentLevel);
	static EndGameNode* createLayoutLose(const int& iScore, const Word& mainWord, const int& iCurrentLevel);

	bool init() override;
	bool initWin();
	bool initLose();

	void addYellowStar(const int& iYellowStar);
	void addBonusQuestCompleted(const int& iBonusQuestCompleted);

private:
	void menuNextLevelCallBack();
	void menuRetryLevelLoseCallBack();
	void menuRetryLevelWinCallBack();
	void menuCloseCallBack();

	void generateLayoutStartAndBonusQuest();

	void loopUpdateStar();
	void sequenceUpdateStar();
	void updateStar();

	void loopUpdateBonusQuest();
	void sequenceUpdateBonusQuest();
	void updateBonusQuest();

	cocos2d::SpriteBatchNode* m_pSpriteBatchNode;

	LevelInfo m_levelInfo;
	ChapterInfo m_chapterInfo;
	Word m_mainWord;

	int m_iScore;
	int m_iYellowStar;
	int m_iCountYellowStar;
	int m_iCurrentLevel;
	int m_iTotalBonusQuest;
	int m_iBonusQuestCompleted;
	int m_iCountBonusQuest;

	LeaderBoardtNode* m_pLeaderBoard;
};

#endif;