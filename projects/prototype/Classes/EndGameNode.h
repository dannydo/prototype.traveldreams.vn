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
	void setEnableAction(const bool& bEnable);

private:
	void menuNextLevelCallBack();
	void menuRetryLevelCallBack();
	void menuCloseCallBack();
	void menuOpenDictCallBack();

	std::vector<int> generateArrayNumber(int iNumber);
	cocos2d::Node* generateLayoutScore(int iScore);
	cocos2d::Node* generateLayoutStar();
	cocos2d::Node* generateLayoutWord(char* mainWord, std::vector<char*> subWord);

	void loopUpdateStar();
	void sequenceUpdateStar();
	void updateStar();

	cocos2d::Node* m_pStarNode;
	cocos2d::Menu* m_pMenuClose;
	cocos2d::Menu* m_pMenuDict;
	cocos2d::Menu* m_pMenuWin;
	cocos2d::Menu* m_pMenuLose;

	int m_iYellowStar;
	int m_iCountYellowStar;
	int m_iCurrentLevel;
};

#endif;