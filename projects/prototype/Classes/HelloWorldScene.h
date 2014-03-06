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
#include "SettingMenuNode.h"


#define _MAX_CHARACTER_ID_	5
#define _BOSS_ZORDER_ 200


using namespace cocos2d;
USING_NS_CC_EXT;

enum TouchMoveState
{
	_TMS_NONE_,
	_TMS_BEGIN_IDENTIFY_,
	_TMS_MOVE_HORIZONTAL_,
	_TMS_MOVE_VERTICAL_,
	_TMS_BEGIN_ACTIVATE_COMBO_,
	_TMS_ACTIVATE_COMBO_LEFT_,
	_TMS_ACTIVATE_COMBO_UP_,
	_TMS_ACTIVATE_COMBO_RIGHT_,
	_TMS_ACTIVATE_COMBO_DOWN_
};

struct CellView
{
public:
	Sprite* m_pSprite;
	//int m_iCharacterID;
	//unsigned char m_iLetter;
	int m_iGemLetterBlockID;

	CellView()
	{
		m_pSprite = NULL;
		//m_iCharacterID = -1;
		//m_iLetter = 255;
		m_iGemLetterBlockID = -1;
	}
};

struct ComputeMoveResult
{
public:
	std::vector<ComboEffectCell> m_ConvertedComboCells;
	std::vector<Cell> m_BasicMatchingDestroyedCells;
	//std::vector<DoubleComboEffectBundle> m_DoubleComboList;
	std::vector<DoubleComboCreationInfo> m_NewDoubleComboList;
	std::vector<ComboEffectBundle*> m_ComboChainList;
	std::vector<ComboEffectBundle*> m_TriggeredCombo6ChainList;
	std::vector<ComboEffectCell> m_NewComboCells;
	std::vector<Cell> m_OriginalMovedCells;
	std::vector<Cell> m_TargetMovedCells;
	std::vector<NewCellInfo> m_UnlockedLetterCells;
	std::vector<NewCellInfo> m_NewCells;	
	

	void Reset(bool bResetConvertedCombo = false)
	{
		if (bResetConvertedCombo)
			m_ConvertedComboCells.clear();
		m_BasicMatchingDestroyedCells.clear();
		m_NewDoubleComboList.clear();
		m_ComboChainList.clear();
		m_TriggeredCombo6ChainList.clear();
		m_NewComboCells.clear();
		m_OriginalMovedCells.clear();
		m_TargetMovedCells.clear();
		m_UnlockedLetterCells.clear();
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
	int GetUnmoveCellsFromCurrentMovingRowOrColumn();

	inline int GetZOrder(const int& iRow, const int& iColumn, const bool& bIsBush) { return (m_GameBoardManager.GetRowNumber()-1-iRow)*m_GameBoardManager.GetColumnNumber() + iColumn + bIsBush*100;}

	void AdjustPosition(bool bIsBlocked, float fDeltaX, float fDeltaY, int iRowMove, int iColumnMove);
	
	void PlayEffect2(const bool& bIsBonusEndGamePhase,  std::vector<ComboEffectCell>& convertedToComboCells, 
		std::vector<Cell>& basicMatchingDestroyedCells,
		std::vector<ComboEffectBundle*>& comboChainList,
		std::vector<ComboEffectCell>& newComboCells, std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<NewCellInfo>& unlockedLetterCells,
		std::vector<NewCellInfo>& newCells, bool bIsNewMove);
		
	Sprite* AddObstacleToGem(Sprite* pGemSprite, const int& iBlockID, const int& iObstacleTypeID);
	Sprite* AddLetterToGem(const Cell& cell, const int& iGemID, const unsigned char& iLetter, const int& iGemLetterBlockID, bool bIsMirror=false);
	void AddNewComboCell(const ComboEffectCell& cell, const float& fDelayTime, const float& fEffectTime, bool bCreateMirror = true);

	void OnEndDragEffect();
	void CheckBoardStateAfterMove();
	void UpdateObstacleListAfterMove();


	void UpdatePostionOfSprite(const int& iRow, const int& iColumn, bool bIsMirror = false);

	void HorizontalMoveUlti(float fDeltaX);
	void VerticalMoveUlti(float fDeltaY);
	

	void PlayCombo4Effect(ComboEffectBundle* pComboEffectBundle, float fDelayTime, float fDisplayTime);
	void PlayCombo5Effect(ComboEffectBundle* pComboEffectBundle, float fDelayTime, float fDisplayTime);
	void PlayCombo6Effect(ComboEffectBundle* pComboEffectBundle, float fDelayTime);	

	void PlayCombo4_4Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime);
	void PlayCombo4_4_4Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime);

	void PlayCombo4_5Effect(ComboEffectBundle* pComboEffect, float fDelayTime);
	void PlayCombo5_5_5Effect(ComboEffectBundle* pComboEffect, float fDelayTime);
	

	void PlayCombo5_5Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime);

	void PlayCombo6_6Effect(ComboEffectBundle* pComboEffectBundle, float fDelayTime);	

	void PlayComboEndGameBonusEffect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime);
	void PlayCrazyPetEndGameBonusEffect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime);

	void ActivateImageEffect(Node* pSender);
	void BasicDestroyCellUlti(const int& iRow, const int & iColumn, const float& fDelay, const float fEffectDuration);
	void PlayChangeColorEffectOnSprite(Sprite* pSprite,const float& fDelayTime);


	// unlock letter flow
	void PlayUnlockLettersOfMainWordAnimation(const float& fDelayTime);	
	void EndUnlockLetterAnimation();

	// execute win game effect flow
	void ShowMainWordUnlockEffect();
	void StartWinBonusPhase();

	void ExecuteBonusWinGameEffect();	
	
	void ShowLevelCompleteEffect();
	void ShowWinGamePopup();

	// utility functions on boss sprite	
	void AddNewLetterToBossSprite(const float& fDelayTime);
	void AddHitPointSpritesToBossSprite(const float& fDelayTime);
	void RemoveLetterFromBossSprite(const float& fDelayTime);
	void Remove1HPFromBossSprite(const float& fDelayTime);
private:
	float m_fBoardLeftPosition, m_fBoardBottomPosition;
	float m_fBoardLeftClipPosition, m_fBoardBottomClipPosition;
	float m_fMaskWidth, m_fMaskHeight;

	cocos2d::CCSpriteBatchNode* m_pBoardBatchNode;
	cocos2d::Sprite* m_pMoveHintNode;
	cocos2d::CCSpriteBatchNode* m_pComboEffectBatchNode;
	cocos2d::Sprite* m_pBossSprite;


	CellView m_BoardViewMatrix[_BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
	CellView m_BoardViewMirrorMatrix[_BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];

	//Sprite* m_BoardObstaclesMatrix[ _BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_][_MAX_OBSTACLE_TYPE_COUNT_];
	Sprite* m_BoardObstaclesList[ _BOARD_MAX_ROW_NUMBER_* _BOARD_MAX_COLUMN_NUMBER_][_MAX_OBSTACLE_TYPE_COUNT_];

	CellView m_MovingCellList[_BOARD_MAX_LENGTH_];
	CellView m_MovingCellMirrorList[_BOARD_MAX_LENGTH_];
	int m_iMovingCellListLength;//this should be number of row or column

	NewGameBoardManager m_GameBoardManager;
	cocos2d::Size m_SymbolSize;

	TouchMoveState m_eTouchMoveState;
	Cell m_SelectedCell;
	Point m_StartTouchPosition;

	int m_iSaveLastCellMoveDelta;
	std::vector<Sprite*> m_HintSprites;

	WordCollectBoardRenderNode* m_pWordCollectBoardRenderNode;
	int m_iMoveCount;
	
	Sprite* m_pTempSpriteForAction; // temporary sprite for action/effect
	Touch* m_pSaveTouch;
	bool m_bIsEffectPlaying;
	bool m_bIsCellDragPlaying;
	TouchMoveState m_ePlayingDragEffect;
	bool m_bIsEndGamePhase;

	ComboCountRenderNode* m_pComboCountRenderNode;
	StatusLayer* m_pStatusLayer;	
	Layer* m_pHUDLayer;

	// 
	ComputeMoveResult m_ComputeMoveResult;

	// special region for end game effect
	cocos2d::LayerColor* m_pEndGameEffectLayer;

	SettingMenuNode* m_pSettingNode	;
	bool m_isShowSetting;
};

#endif // __HELLOWORLD_SCENE_H__
