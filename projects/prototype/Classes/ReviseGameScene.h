#ifndef _REVISE_GAME_SCENE_H_
#define _REVISE_GAME_SCENE_H_

#include "cocos2d.h"
#include "Database\WordTable.h"
#include "FlashCardNode.h"
#include "ButtonManagerNode.h"
#include "ButtonNode.h"
#include "GameDataStructure.h"
#include "Database\WordTable.h"
#include "HeaderNode.h"
#include "Database\ChapterTable.h"

class ReviseGameLayer : public cocos2d::Layer
{
public:
	ReviseGameLayer() {};
	virtual ~ReviseGameLayer();
	bool init();

	static ReviseGameLayer* createLayout(const ChapterInfo& chapterInfo);

private:
	ButtonNode* createButtonPictureFlashcard(const WordInfo& wordInfo, const int& iIndexPicture);

	void clickChoosePicture(cocos2d::Object* sender);
	void clickCollect(cocos2d::Object* sender);
	void createLayout();
	void randownIndexWordCollected();
	void randownIndexWords();

	void playEffectWin();
	void playEffectLose();
	void playEffectAddLayout();

	//void updateSeconds(float dt);
	void update(float delta) override;
	void addPopupEndGame();

	void clickRetry(Object* sender);
	void clickClose(Object* sender);

	void updateIsEndGame();

	ChapterInfo m_chapterInfo;

	std::vector<WordInfo> m_ChapterWords;
	std::vector<WordInfo> m_WordsCollected;

	FlashCardNode* m_pFlashCard;

	cocos2d::Node* m_pChooseImageNode;

	WordInfo m_MaintWordInfo;

	HeaderNode* m_pHeaderNode;

	int m_iIndexWordCollected;
	int m_iStage;

	std::vector<int> m_arrIndexWord;
	cocos2d::LabelTTF* m_pLabelClock;
	//int m_iSeconds;
	float m_fSeconds;

	cocos2d::LayerColor* m_pPopupEndGame;
	bool m_bIsEndGame;
};

class ReviseGameScene : public cocos2d::CCScene
{
public:
	ReviseGameScene():_layer(NULL) {};
	~ReviseGameScene();
	bool init();

	static ReviseGameScene* createScene(const ChapterInfo& chapterInfo);
	
	CC_SYNTHESIZE_READONLY(ReviseGameLayer*, _layer, Layer);

private:
	ChapterInfo m_chapterInfo;
};

#endif;