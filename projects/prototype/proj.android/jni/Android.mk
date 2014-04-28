LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
				   ../../Classes/GameBoardManager.cpp \
				   ../../Classes/NewGameBoardManager.cpp \
				   ../../Classes/WordCollectBoardRenderNode.cpp \
				   ../../Classes/MainMenuScene.cpp \
				   ../../Classes/ComboCountRenderNode.cpp \
				   ../../Classes/SoundManager.cpp \
				   ../../Classes/EndGameNode.cpp \
				   ../../Classes/GameTargetNode.cpp \
				   ../../Classes/StatusLayer.cpp \
				   ../../Classes/DictionaryNode.cpp \
				   ../../Classes/GameWordManager.cpp \
				   ../../Classes/IntroductionLayer.cpp \
				   ../../Classes/FlashCardScene.cpp \
				   ../../Classes/LevelMapScene.cpp \
				   ../../Classes/WorldMapScene.cpp \
				   ../../Classes/GameConfigManager.cpp \
				   ../../Classes/ObstacleProcessManager.cpp \
				   ../../Classes/GemLetterManager.cpp \
				   ../../Classes/LeaderBoardNode.cpp \
				   ../../Classes/LoadingScene.cpp \
				   ../../Classes/LifeSystemNode.cpp \
				   ../../Classes/BonusQuestManager.cpp \
				   ../../Classes/SettingMenuNode.cpp \
				   ../../Classes/FlashCardCollection.cpp \
				   ../../Classes/CustomNode.cpp \
				   ../../Classes/LoadingImageNode.cpp \
				   ../../Classes/Social/FacebookManager.cpp \
				   ../../Classes/APIService/UserService.cpp \
				   ../../Classes/APIService/InterfaceService.cpp \
				   ../../Classes/Database/InitDatabase.cpp \
				   ../../Classes/Database/sqlite3.c \
				   ../../Classes/Database/ChapterTable.cpp \
				   ../../Classes/Database/LevelTable.cpp \
				   ../../Classes/Database/UserTable.cpp \
				   ../../Classes/Database/DictionaryDatabase.cpp \
				   ../../Classes/Database/GameTracking.cpp \
				   ../../Classes/ButtonNode.cpp \
				   ../../Classes/ActionExtension.cpp \
				   ../../Classes/FooterNode.cpp \
				   ../../Classes/ButtonManagerNode.cpp \
				   ../../Classes/HeaderNode.cpp \
				   ../../Classes/ScrollManager.cpp \
				   ../../Classes/TimeCountDownNode.cpp \
				   ../../Classes/Database/PowerUpTable.cpp \
				   ../../Classes/Database/TransactionTable.cpp \
				   ../../Classes/Database/UnlockChapterTable.cpp \
				   ../../Classes/Database/VersionTable.cpp \
				   ../../Classes/Database/WordTable.cpp \
				   ../../Classes/APIService/SyncDataGame.cpp \
				   ../../Classes/SystemEventHandle.cpp \
				   ../../Classes/WaitingNode.cpp \
				   ../../Classes/ConnectFacebookConfirm.cpp \
				   ../../Classes/CustomPackageDownloaderNode.cpp \
				   ../../Classes/EndGameCustomModeNode.cpp \
				   ../../Classes/Database/CSPackageTable.cpp \
				   ../../Classes/Database/CSWordTable.cpp \
				   ../../Classes/ConfirmQuitLevelNode.cpp \
				   ../../Classes/FunctionCommon.cpp \
				   ../../Classes/OutOfMovesNode.cpp \
				   ../../Classes/PopupConfirmNode.cpp \
				   ../../Classes/GetMoreLifeNode.cpp \
				   ../../Classes/FlashCardNode.cpp \
				   ../../Classes/ExtendedNodes/Scale3Sprite.cpp \
				   ../../Classes/ClipMaskNode.cpp \
				   ../../Classes/QuitLevelFailedNode.cpp \
				   ../../Classes/HelloWorldScene.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../../../plugin/protocols/platform/android

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static \
PluginProtocolStatic
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dxandroid_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)
$(call import-module,protocols/android)
$(call import-module,cocos2dx/platform/android)
