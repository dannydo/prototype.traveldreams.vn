#ifndef _END_GAME_H_
#define _END_GAME_H_

#include "cocos2d.h"
#include "GameDataStructure.h"
#include "LeaderBoardNode.h"

class EndGameNode : public cocos2d::Node
{
public:
	static EndGameNode* createLayoutWin(const int& iScore, const Word& pMainWord, const std::vector<Word>& pSubWord, const int& iCurrentLevel);
	static EndGameNode* createLayoutLose(const int& iScore, const Word& pMainWord, const int& iCurrentLevel);

	bool init() override;
	bool initWin(const int& iScore, const Word& pMainWord, const std::vector<Word>& pSubWord);
	bool initLose(const int& iScore, const Word& pMainWord);

	void addYellowStar(int iYellowStar);

private:
	void menuNextLevelCallBack();
	void menuRetryLevelCallBack();
	void menuCloseCallBack();
	void menuOpenDictCallBack();

	std::vector<int> generateArrayNumber(int iNumber);
	cocos2d::Node* generateLayoutScore(int iScore);
	cocos2d::Node* generateLayoutStar();

	cocos2d::Node* generateLayoutSubWord(const std::vector<Word>& subWord);
	cocos2d::Node* generateLayoutLevel(int iLevel);

	void loopUpdateStar();
	void sequenceUpdateStar();
	void updateStar();

	cocos2d::Node* m_pStarNode;

	int m_iYellowStar;
	int m_iCountYellowStar;
	int m_iCurrentLevel;

	LeaderBoardtNode* m_pLeaderBoard;
};

#endif;