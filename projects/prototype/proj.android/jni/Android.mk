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
				   ../../Classes/BonusWordNodeNew.cpp \
				   ../../Classes/IntroductionLayer.cpp \
				   ../../Classes/FlashCardScene.cpp \
				   ../../Classes/LevelMapScene.cpp \
				   ../../Classes/WorldMapScene.cpp \
                   ../../Classes/HelloWorldScene.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
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
$(call import-module,cocos2dx/platform/android)
