#ifndef _WORD_COLLECT_BOARD_RENDER_NODE_H_
#define _WORD_COLLECT_BOARD_RENDER_NODE_H_

#include "cocos2d.h"
#include "GameBoardManager.h"
#include "GameWordManager.h"
#include "cocos-ext.h"

using namespace cocos2d;
USING_NS_CC_EXT;

using namespace cocos2d::extension::armature;

#define _WCBRN_MAX_FIRST_LINE_LETTERS_ 13
#define _WCBRN_MAX_SECOND_LINE_LETTERS_ 12

class WordCollectBoardRenderNode : public Layer //CCNodeRGBA
{
private:
	WordCollectBoardRenderNode();
public:
	static WordCollectBoardRenderNode* create();

	void GenerateLabels(GameModeType_e eGameModeType, int iTimeModeStage);//call this after generate level data
	//void draw() override;
	void update(float dt) override;	

	//void UpdateList();

	//void UnlockCharacter(const float& fDelayTime, const int& iLetterIndex);
	//void UnlockLetter(const int& iLetterIndex);
	//float PlayUnlockLettersAnimation(float fDelayTime);

	std::string GetImageFileFromLetter(unsigned char iLetter);	
	std::string GetImageInGemFileFromLetter(unsigned char iLetter);	
	std::string GetImageInFileFromBonusLetter(unsigned char iLetter);	

	inline void SetStartGameCallback( std::function<void()> startGameCallback) { m_StartGameCallback = startGameCallback;}

	//int GetNextCharacterID();

	bool onTouchBegan(Touch *pTouch, Event *pEvent) override;

	float PlaySpellingSound();

	void PlayUnlockLetterEffect(const int& iLetterIndex, const float& fDelayEffectTime, const unsigned char& iLetter, const Point& position);
	float PlayUnlockWordEffect();
	float PlayTimeMode_UnlockWordFailEffect();
private:
	//void LoadWords();
	//void GetWordIndex();

	virtual bool init();			

	void PlayTimeModeWordStart(int iTimeModeStage);
private:	
	std::function<void()> m_StartGameCallback;

	//std::vector<CCLabelTTF*> m_LabelList;	
	/*int m_iWordCount;	
	Word m_WordList[_GDS_WORD_MAX_COUNT_];

	int m_iWordIndex;
	int m_iNextCharacterID;
	
	bool m_ActivedCharacterFlags[_GDS_WORD_MAX_LENGTH_];*/
	const Word* m_pMainWord;	

	Sprite* m_LabelList[_GDS_WORD_MAX_LENGTH_];
	Sprite* m_BubbleList[_GDS_WORD_MAX_LENGTH_];
	
	//float m_LabelXPositionList[_GDS_WORD_MAX_LENGTH_];

	SpriteBatchNode* m_pBackgroundBatchNode;
	SpriteBatchNode* m_pFrontgroundBatchNode;
	//Sprite* m_pBackground;
	DrawNode* m_pColorNode;	
	//Sprite* m_pLetterPanel;

	Node* m_pHeaderNode;

	unsigned long m_iPreviousMainWordTapTime;
	//unsigned long m_iUnlockedLetterEffectEndTime;

	//Armature* m_pCharacter;

	//bool m_NewUnlockedLetterFlags[_GDS_WORD_MAX_LENGTH_];

	// position for bubble
	Point m_CharacterFirstLinePositions[_WCBRN_MAX_FIRST_LINE_LETTERS_];
	Point m_CharacterSecondLinePositions[_WCBRN_MAX_SECOND_LINE_LETTERS_];
};


#endif //_WORD_COLLECT_BOARD_RENDER_NODE_H_