#include "WaveManager.h"
#include "NormalZombie.h" // Example

WaveManager* WaveManager::create() {
    auto ret = new WaveManager();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool WaveManager::init() {
    if (!Node::init()) return false;
    _currentWave = 0;
    return true;
}

void WaveManager::startWaves() {
    this->schedule([this](float dt) {
        _currentWave++;
        spawnWave(_currentWave);
        }, 30.0f, "waveTimer"); // Every 30 seconds
}

void WaveManager::spawnWave(int waveNumber) {
    // Example: Spawn 5 zombies per wave
    for (int i = 0; i < 5; ++i) {
        auto zombie = NormalZombie::create();
        int row = rand() % 5;
        spawnZombie(zombie, row);
    }
}

void WaveManager::spawnZombie(Zombie* zombie, int row) {
    zombie->setPosition(Vec2(Director::getInstance()->getVisibleSize().width + 50, row * GridSystem::GRID_HEIGHT + GridSystem::GRID_HEIGHT / 2));
    this->getParent()->addChild(zombie);
    zombie->walkLeft();
}