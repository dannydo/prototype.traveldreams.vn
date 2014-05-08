#ifndef _END_GAME_H_
#define _END_GAME_H_

#include "cocos2d.h"
#include "GameDataStructure.h"
#include "LeaderBoardNode.h"	
#include "Database\LevelTable.h"
#include "Database\ChapterTable.h"
#include "ButtonManagerNode.h"

class EndGameNode : public cocos2d::Node
{
public:
	static EndGameNode* createLayoutWin(const int& iScore, const Word& mainWord, const int& iCurrentLevel, const std::string sChapterId, const int& iYellowStar, const int& iBonusQuestCompleted);
	static EndGameNode* createLayoutLose(const int& iScore, const Word& mainWord, const int& iCurrentLevel,  const std::string sChapterId);

	bool initWin();
	bool initLose();

private:
	void menuNextLevelCallBack(cocos2d::Object* sender);
	void menuRetryLevelLoseCallBack(cocos2d::Object* sender);
	void menuRetryLevelWinCallBack(cocos2d::Object* sender);
	void menuCloseWinCallBack(cocos2d::Object* sender);
	void menuCloseLoseCallBack(cocos2d::Object* sender);

	void generateLayoutStartAndBonusQuest();

	void loopUpdateStar();
	void sequenceUpdateStar();
	void updateStar();

	void loopUpdateBonusQuest();
	void sequenceUpdateBonusQuest();
	void updateBonusQuest();

	void updateDatabaseAndSync();

	cocos2d::SpriteBatchNode* m_pSpriteBatchNode;
	cocos2d::Node* m_pStarAndBonusQuestNode;

	LevelInfo m_levelInfo;
	ChapterInfo m_chapterInfo;
	Word m_mainWord;

	std::string m_sChapterId;
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