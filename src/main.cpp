#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/GJGameState.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/cocos/CCDirector.h>
#include <chrono>
#include <random>
#include "AudioInfo.hpp"

#define NUM_IMAGES 7
#define PAUSE_DUR 2.5f

using namespace geode::prelude;

static std::random_device RD;
static std::mt19937 GEN(RD());

static int getRandLTE(int x){
    std::uniform_int_distribution<> dist(1,x);
    return dist(GEN);
}

class $modify(ModGL, GJBaseGameLayer){
    struct Fields{
        bool m_isPaused = false;
        float m_pauseTimeElapsed;
        cocos2d::CCLayerColor* m_pauseLayer;
        CCSprite* m_phonkSpr = nullptr;
    };

    bool init(){
        bool ret = GJBaseGameLayer::init();
        const cocos2d::ccColor4B colour = cocos2d::ccColor4B({0,0,0,0});
        m_fields->m_pauseLayer = cocos2d::CCLayerColor::create(colour);
        this->addChild(m_fields->m_pauseLayer);
        return ret;
    }

    void displaySigmaImage(){
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        int imagenum = getRandLTE(NUM_IMAGES);
        auto spr = CCSprite::createWithSpriteFrameName((std::string(""_spr) + std::to_string(imagenum) + std::string(".png")).c_str());
        spr->setAnchorPoint({0.5f,0.5f});
        spr->setScale(0.3f);
        spr->setPosition({screenSize.width / 2 , screenSize.height / 5});
        spr->setZOrder(2);
        this->addChild(spr);
        m_fields->m_phonkSpr = spr;
    }

    void removeSigmaImage(){
        if(m_fields->m_phonkSpr == nullptr){
            log::info("No sprite");
            return;
        }
        m_fields->m_phonkSpr->removeFromParent();
        m_fields->m_phonkSpr = nullptr;
    }

    void stopGame(){
        int audioSelection =  getRandLTE(NUM_SONGS);
        std::string prfx(""_spr);
        std::string path(".ogg");
        path = prfx + std::to_string(audioSelection) + path;
        log::info("{}", audioSelection);
        log::info("path: {}", path);

        if(m_fields->m_isPaused == true){
            return;
        }
        m_fields->m_isPaused = true;
        m_fields->m_pauseTimeElapsed = 0.f;
        FMODAudioEngine::get()->pauseAllMusic(true);
        FMODAudioEngine::get()->playEffectAdvanced(path, EFFECT_ARGS(audioSelection));
        m_fields->m_pauseLayer->setOpacity(150);
        displaySigmaImage();
    }

    void resumeGame(){
        m_fields->m_isPaused = false;
        FMODAudioEngine::get()->resumeAllMusic();
        m_fields->m_pauseLayer->setOpacity(0);
        removeSigmaImage();
    }

    void update(float p0) {
        if(!m_fields->m_isPaused){
            GJBaseGameLayer::update(p0);
            return;
        }
        if(m_fields->m_pauseTimeElapsed >  PAUSE_DUR){
            resumeGame();
        } else {
            m_fields->m_pauseTimeElapsed += p0;
        }
    }

    bool isNonJumpGamemode(PlayerObject* player){
        if(player == nullptr){
            return false;
        }
        return player->m_isShip || player->m_isDart || player->m_isSwing || player->m_isBird;
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        if(!down){
        FMODAudioEngine::get()->resumeAllMusic();
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }
        if(isNonJumpGamemode(m_player1) || (m_gameState.m_isDualMode ? isNonJumpGamemode(m_player2) : false)){
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            stopGame();
            return;
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};

class $modify(PlayLayer){
    void resume(){
        PlayLayer::resume();
        FMODAudioEngine::get()->resumeAllEffects();
    }
};

class $modify(PlayerObject){
    void updateJump(float p0) {
        ModGL* gl = (ModGL*)GJBaseGameLayer::get();
        if(gl != nullptr && p0 == 0.f){
            gl->stopGame();
            return;
        }
        PlayerObject::updateJump(p0);
    }

    void ringJump(RingObject* p0, bool p1) {
        bool pressedThisFrame = m_stateRingJump;
        PlayerObject::ringJump(p0, p1);
        ModGL* gl = (ModGL*)GJBaseGameLayer::get();
        if(gl != nullptr && pressedThisFrame){
            gl->stopGame();
            return;
        }
    }
};