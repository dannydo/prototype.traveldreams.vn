#ifndef _END_GAME_H_
#define _END_GAME_H_

#include "cocos2d.h"

class EndGameNode : public cocos2d::Node
{
public:
	static EndGameNode* createLayoutWin(const int& iScore, char* pMainWord, std::vector<char*> pSubWord, const int& iCurrentLevel);
	static EndGameNode* createLayoutLose(const int& iScore, char* pMainWord, const int& iCurrentLevel);

	bool init() override;
	bool initWin(const int& iScore, char* pMainWord, std::vector<char*> pSubWord);
	bool initLose(const int& iScore, char* pMainWord);

	void addYellowStar(int iYellowStar);

private:
	void menuNextLevelCallBack();
	void menuRetryLevelCallBack();
	void menuCloseCallBack();
	void menuOpenDictCallBack();

	std::vector<int> generateArrayNumber(int iNumber);
	Node* generateLayoutScore(int iScore);
	Node* generateLayoutStar();
	Node* generateLayoutWord(char* mainWord, std::vector<char*> subWord);

	void loopUpdateStar();
	void sequenceUpdateStar();
	void updateStar();

	Node* m_pStarNode;

	int m_iYellowStar;
	int m_iCountYellowStar;
	int m_iCurrentLevel;
};

#endif;