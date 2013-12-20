#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

//#include "GameBoardManager.h"
#include "NewGameBoardManager.h"
#include "WordCollectBoardRenderNode.h"
#include "ComboCountRenderNode.h"
#include "StatusLayer.h"
//#include "BonusWordNode.h"
#include "BonusWordNodeNew.h"


#define _MAX_CHARACTER_ID_	5

using namespace cocos2d;
USING_NS_CC_EXT;

enum TouchMoveState
{
	_TMS_NONE_,
	_TMS_BEGIN_IDENTIFY_,
	_TMS_MOVE_HORIZONTAL_,
	_TMS_MOVE_VERTICAL_
};

struct CellView
{
public:
	Sprite* m_pSprite;
	//int m_iCharacterID;
	unsigned char m_iLetter;

	CellView()
	{
		m_pSprite = NULL;
		//m_iCharacterID = -1;
		m_iLetter = 255;
	}
};

struct ComputeMoveResult
{
public:
	std::vector<ComboEffectCell> m_ConvertedComboCells;
	std::vector<Cell> m_BasicMatchingDestroyedCells;
	std::vector<DoubleComboEffectBundle> m_DoubleComboList;
	std::vector<ComboEffectBundle*> m_ComboChainList;
	std::vector<ComboEffectCell> m_NewComboCells;
	std::vector<Cell> m_OriginalMovedCells;
	std::vector<Cell> m_TargetMovedCells;
	std::vector<Cell> m_NewCells;

	void Reset(bool bResetConvertedCombo = false)
	{
		if (bResetConvertedCombo)
			m_ConvertedComboCells.clear();
		m_BasicMatchingDestroyedCells.clear();
		m_DoubleComboList.clear();
		m_ComboChainList.clear();
		m_NewComboCells.clear();
		m_OriginalMovedCells.clear();
		m_TargetMovedCells.clear();
		m_NewCells.clear();
	}
};

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene(int iLevel=1);

	~HelloWorld();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    virtual void initLevel(int iLevel=1);
    // a selector callback
    void menuCloseCallback(Object* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	bool onTouchBegan(Touch *pTouch, Event *pEvent) override;
	void onTouchEnded(Touch* pTouch, Event* pEvent) override;
	void onTouchMoved(Touch *pTouch, Event *pEvent) override;
	void onTouchCancelled(Touch *pTouch, Event *pEvent) override;
	void update(float fDeltaTime);
	void visit() override;
protected:
	std::string GetImageFileFromGemID(int iGemID, GemComboType_e eGemComboType=_GCT_NONE_);	
	void AdjustPosition(float fDeltaX, float fDeltaY, int iRowMove, int iColumnMove);
	
	void PlayEffect2(std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboEffectBundle> doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList,std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<Cell>& newCells, bool bIsNewMove);
	
	void OnEndDragEffect();
	void CheckBoardStateAfterMove();


	void UpdatePostionOfSprite(const int& iRow, const int& iColumn, bool bIsMirror = false);

	void HorizontalMoveUlti(float fDeltaX);
	void VerticalMoveUlti(float fDeltaY);

	void ActivateImageEffect(Node* pSender);

	void BasicDestroyCellUlti(const int& iRow, const int & iColumn, const float& fDelay, const float& fEffectDuration);


	// execute win game effect flow
	void ShowMainWordUnlockEffect();
	void StartWinBonusPhase();

	void ExecuteBonusWinGameEffect();	
	void PlayBonusEndEffect( std::vector<ComboEffectCell>& convertedToComboCells,
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboEffectBundle> doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList,std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<Cell>& newCells);

	void ShowLevelCompleteEffect();
	void ShowWinGamePopup();
private:
	float m_fBoardLeftPosition, m_fBoardBottomPosition;
	float m_fBoardLeftClipPosition, m_fBoardBottomClipPosition;
	cocos2d::CCSpriteBatchNode* m_pBoardBatchNode;
	//cocos2d::CCSprite* m_BoardSpriteMatrix[ _BOARD_MAX_ROW_NUMBER_*3][ _BOARD_MAX_COLUMN_NUMBER_*3];
	/*cocos2d::CCSprite* m_BoardSpriteMatrixLeft[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	cocos2d::CCSprite* m_BoardSpriteMatrixTop[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	cocos2d::CCSprite* m_BoardSpriteMatrixRight[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	cocos2d::CCSprite* m_BoardSpriteMatrixBottom[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];*/

	CellView m_BoardViewMatrix[_BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	CellView m_BoardViewMirrorMatrix[_BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];

	CellView m_MovingCellList[_BOARD_MAX_LENGTH_];
	CellView m_MovingCellMirrorList[_BOARD_MAX_LENGTH_];
	int m_iMovingCellListLength;//this should be number of row or column

	NewGameBoardManager m_GameBoardManager;
	cocos2d::Size m_SymbolSize;

	TouchMoveState m_eTouchMoveState;
	Cell m_SelectedCell;
	Point m_StartTouchPosition;

	WordCollectBoardRenderNode* m_pWordCollectBoardRenderNode;
	int m_iMoveCount;
	
	Sprite* m_pTempSpriteForAction; // temporary sprite for action/effect
	Touch* m_pSaveTouch;
	bool m_bIsEffectPlaying;
	bool m_bIsCellDragPlaying;
	TouchMoveState m_ePlayingDragEffect;

	ComboCountRenderNode* m_pComboCountRenderNode;
	StatusLayer* m_pStatusLayer;
	BonusWordNodeNew* m_pBonusWordNode;
	Layer* m_pHUDLayer;

	// 
	ComputeMoveResult m_ComputeMoveResult;

	// special region for end game effect
	cocos2d::LayerColor* m_pEndGameEffectLayer;
	

};

#endif // __HELLOWORLD_SCENE_H__
