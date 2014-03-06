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
	static EndGameNode* createLayoutWin(const int& iScore, const Word& pMainWord, const std::vector<Word>& pSubWord, const int& iCurrentLevel);
	static EndGameNode* createLayoutLose(const int& iScore, const Word& pMainWord, const int& iCurrentLevel);

	bool init() override;
	bool initWin(const int& iScore, const Word& pMainWord, const std::vector<Word>& pSubWord);
	bool initLose(const int& iScore, const Word& pMainWord);

	void addYellowStar(const int& iYellowStar);
	void addBonusQuestCompleted(const int& iBonusQuestCompleted);

private:
	void menuNextLevelCallBack();
	void menuRetryLevelLoseCallBack();
	void menuRetryLevelWinCallBack();
	void menuCloseCallBack();

	std::vector<int> generateArrayNumber(int iNumber);
	cocos2d::Node* generateLayoutScore(int iScore);
	cocos2d::Node* generateLayoutStar();
	cocos2d::Node* generateLayoutBonusQuest();

	cocos2d::Node* generateLayoutSubWord(const std::vector<Word>& subWord);
	cocos2d::Node* generateLayoutLevel(int iLevel);

	void loopUpdateStar();
	void sequenceUpdateStar();
	void updateStar();

	void loopUpdateBonusQuest();
	void sequenceUpdateBonusQuest();
	void updateBonusQuest();

	cocos2d::Node* m_pStarNode;
	cocos2d::Node* m_pBonusQuestNode;

	LevelInfo m_levelInfo;
	ChapterInfo m_chapterInfo;

	int m_iYellowStar;
	int m_iCountYellowStar;
	int m_iCurrentLevel;
	int m_iTotalBonusQuest;
	int m_iBonusQuestCompleted;
	int m_iCountBonusQuest;

	LeaderBoardtNode* m_pLeaderBoard;
};

#endif;