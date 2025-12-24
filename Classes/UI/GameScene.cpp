#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene() {
    return GameScene::create();
}

bool GameScene::init() {
    if (!Scene::init()) return false;

    // 初始化波次管理器
    _waveManager = WaveManager::create();
    if (_waveManager) {
        this->addChild(_waveManager);
        _waveManager->startWaves(); // 开始生成僵尸波
    }

    return true;
}