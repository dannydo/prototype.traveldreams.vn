#ifndef _MINI_GAME_SCENE_H_
#define _MINI_GAME_SCENE_H_

#include "cocos2d.h"
#include "Database\WordTable.h"
#include "FlashCardNode.h"
#include "ButtonManagerNode.h"
#include "ButtonNode.h"
#include "GameDataStructure.h"
#include "Database\WordTable.h"
#include "HeaderNode.h"

#define _GIFT_COIN_ 100

class MiniGameLayer : public cocos2d::Layer
{
public:
	MiniGameLayer() {};
	virtual ~MiniGameLayer();
	bool init();

	CREATE_FUNC(MiniGameLayer);

private:
	ButtonNode* createButtonPictureFlashcard(const WordInfo& wordInfo, const int& iIndexPicture);

	void clickChoosePicture(cocos2d::Object* sender);
	void clickCollect(cocos2d::Object* sender);
	void createLayout();
	void randownIndexWordNew();
	void randownIndexWords();

	void playEffectWin();
	void playEffectAddDescription();
	void playEffectLose();
	void playEffectAddLayout();
	void playEffectCollect();

	std::vector<WordInfo> m_ChapterWords;
	std::vector<WordInfo> m_WordsNew;
	cocos2d::LabelTTF* m_pLabelIndex;
	FlashCardNode* m_pFlashCard;

	cocos2d::Node* m_pChooseImageNode;
	WordInfo m_MaintWordInfo;

	HeaderNode* m_pHeaderNode;

	int m_iIndexWordNewCount;
	int m_iIndexWordNew;
	int m_iTotalWordNew;

	std::vector<int> m_arrIndexWordNew;
	std::vector<int> m_arrIndexWord;
};

class MiniGameScene : public cocos2d::CCScene
{
public:
	MiniGameScene():_layer(NULL) {};
	~MiniGameScene();
	bool init();
	
	CREATE_FUNC(MiniGameScene);
	CC_SYNTHESIZE_READONLY(MiniGameLayer*, _layer, Layer);
};

#endif;