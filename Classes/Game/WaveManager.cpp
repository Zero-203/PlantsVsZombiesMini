#include "WaveManager.h"
#include "./Entities/Zombie/Zombie.h"
#include "./Entities/Zombie/ZombieNormal.h"
#include "./Entities/Zombie/ZombieConeHead.h"    // 添加这行
#include "./Entities/Zombie/ZombieBucketHead.h"  // 添加这行
#include "./Game/GameManager.h"
#include <cocos2d.h>

USING_NS_CC;

WaveManager* WaveManager::_instance = nullptr;

WaveManager* WaveManager::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new WaveManager();
    }
    return _instance;
}

WaveManager::WaveManager()
    : _currentWave(0)
    , _totalWaves(10)
    , _state(WaveState::PREPARING)
    , _zombiesSpawned(0)
    , _zombiesKilled(0)
    , _zombiesRemaining(0)
    , _zombiesReachedEnd(0)
    , _spawnTimer(0)
    , _waveTimer(0)
    , _preparationTimer(10.0f) // 初始准备时间10秒
{
}

WaveManager::~WaveManager()
{
    _instance = nullptr;
}

void WaveManager::init(int totalWaves)
{
    _totalWaves = totalWaves;
    _currentWave = 0;
    _state = WaveState::PREPARING;

    _zombiesSpawned = 0;
    _zombiesKilled = 0;
    _zombiesRemaining = 0;
    _zombiesReachedEnd = 0;

    _spawnTimer = 0;
    _waveTimer = 0;
    _preparationTimer = 2.0f; // 初始准备时间10秒

    _activeZombies.clear();

    log("WaveManager: Initialized with %d waves, preparation time: %.1f seconds",
        totalWaves, _preparationTimer);
}

void WaveManager::update(float delta)
{
    if (_state == WaveState::GAME_OVER || _state == WaveState::COMPLETED)
    {
        return;
    }

    switch (_state)
    {
        case WaveState::PREPARING:
            // 准备阶段倒计时
            _preparationTimer -= delta;
            if (_preparationTimer <= 0)
            {
                startNextWave();
            }
            break;

        case WaveState::SPAWNING:
            // 生成僵尸的逻辑保持不变
            if (_zombiesSpawned < _currentWaveData.zombieCount)
            {
                _spawnTimer += delta;
                if (_spawnTimer >= _currentWaveData.spawnInterval)
                {
                    spawnZombie();
                    _spawnTimer = 0.0f;
                }
            }

            // 清理並檢查
            cleanupZombieList();

            // 检查波次完成条件
            if (_zombiesSpawned >= _currentWaveData.zombieCount && _activeZombies.empty())
            {
                log("WaveManager: Wave %d completion condition met (spawned: %d, active: %d)",
                    _currentWave, _zombiesSpawned, (int)_activeZombies.size());
                completeWave();
            }
            break;
    }

    // 清理无效的僵尸指针
    cleanupZombieList();
}

void WaveManager::cleanupZombieList()
{
    // 移除已经被销毁的僵尸
    auto it = _activeZombies.begin();
    while (it != _activeZombies.end())
    {
        Zombie* zombie = *it;

        // 检查僵尸是否有效
        if (!zombie || !zombie->getParent() || !zombie->isAlive())
        {
            it = _activeZombies.erase(it);
            log("WaveManager: Cleaned up invalid zombie");
        }
        else
        {
            ++it;
        }
    }
}

void WaveManager::startNextWave()
{

    if (_currentWave < _totalWaves)
    {
        _currentWave++;
        startWave(_currentWave);
    }
    else
    {
        // 所有波次完成
        if (_allWavesCompletedCallback)
        {
            _allWavesCompletedCallback();
        }

        log("WaveManager: All waves completed!");
    }
}

void WaveManager::startWave(int waveNumber)
{
    _currentWaveData = WaveData(waveNumber);
    _zombiesSpawned = 0;
    _zombiesKilled = 0;
    _zombiesReachedEnd = 0;
    _zombiesRemaining = _currentWaveData.zombieCount;
    _spawnTimer = 0;
    _waveTimer = 0;
    _state = WaveState::SPAWNING;

    log("WaveManager: Starting wave %d with %d zombies", waveNumber, _currentWaveData.zombieCount);

    if (_waveStartedCallback)
    {
        _waveStartedCallback(waveNumber);
    }
}

void WaveManager::spawnZombie()
{
    log("WaveManager::spawnZombie called");

    Zombie* zombie = spawnRandomZombie();
    if (zombie)
    {
        zombieSpawned(zombie);
        _zombiesSpawned++;
        _zombiesRemaining--;

        log("WaveManager: Zombie spawned successfully! (%d/%d)",
            _zombiesSpawned, _currentWaveData.zombieCount);
    }
    else
    {
        log("ERROR: Failed to spawn zombie!");
    }
}

Zombie* WaveManager::spawnRandomZombie()
{
    log("WaveManager::spawnRandomZombie: Creating zombie...");

    // 1. 先檢查場景是否可用
    auto director = Director::getInstance();
    auto scene = director->getRunningScene();
    if (!scene)
    {
        log("ERROR: No running scene!");
        return nullptr;
    }


    log("WaveManager::spawnRandomZombie: Zombie created successfully");

    // 根据波次决定僵尸类型
    Zombie* zombie = nullptr;
    int zombieType = rand() % 100;

    // 随着波次增加，出现更强僵尸的概率增加
    int strongZombieChance = std::min(30, _currentWave * 5); // 最多30%概率

    if (zombieType < 70 - strongZombieChance) {
        // 普通僵尸 (70% - strongZombieChance)
        zombie = ZombieNormal::create();
    }
    else if (zombieType < 90) {
        // 路障僵尸 (20%)
        zombie = ZombieConeHead::create();
    }
    else {
        // 铁桶僵尸 (10%)
        zombie = ZombieBucketHead::create();
    }

    if (!zombie)
    {
        log("ERROR: Failed to create zombie!");
        return nullptr;
    }

    log("WaveManager::spawnRandomZombie: Zombie created successfully (Type: %d)", (int)zombie->getType());


    // 3. 設置位置
    auto visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();

    if (visibleSize.width <= 0 || visibleSize.height <= 0)
    {
        log("ERROR: Invalid visible size!");
        zombie->release();
        return nullptr;
    }

    // 隨機選擇行（0-4）
    int row = rand() % 5;
    float x = visibleSize.width + 100; // 從屏幕右側外生成
    float y = 150 + row * 130; // 根據網格系統調整

    zombie->setPosition(Vec2(x, y));
    zombie->setRow(row);

    log("WaveManager::spawnRandomZombie: Zombie position set to (%.1f, %.1f), row: %d", x, y, row);

    // 4. 添加到場景
    scene->addChild(zombie, 10); // 使用較高層級

    // 5. 添加到活躍列表
    _activeZombies.push_back(zombie);

    log("WaveManager::spawnRandomZombie: Zombie added successfully");
    return zombie;
}

void WaveManager::zombieSpawned(Zombie* zombie)
{
    // 这里可以添加僵尸生成时的特殊效果
}

void WaveManager::zombieKilled(Zombie* zombie)
{
    if (!zombie) return;

    _zombiesKilled++;
    log("WaveManager: Zombie killed (%d/%d)", _zombiesKilled, _currentWaveData.zombieCount);

    // 从活跃列表中移除
    auto it = std::find(_activeZombies.begin(), _activeZombies.end(), zombie);
    if (it != _activeZombies.end())
    {
        _activeZombies.erase(it);
    }
}

void WaveManager::zombieReachedEnd(Zombie* zombie)
{
    if (!zombie) return;

    _zombiesReachedEnd++;
    log("WaveManager: Zombie reached end (%d/%d)", _zombiesReachedEnd, _currentWaveData.zombieCount);

    // 从活跃列表中移除
    auto it = std::find(_activeZombies.begin(), _activeZombies.end(), zombie);
    if (it != _activeZombies.end())
    {
        _activeZombies.erase(it);
    }

    // 检查是否游戏结束
    if (_zombiesReachedEnd >= 3) // 3个僵尸到达终点则游戏结束
    {
        gameOver();
    }
}

void WaveManager::completeWave()
{
    _state = WaveState::COMPLETED;
    _waveTimer = 0;

    // 清除當前波次的數據
    _zombiesSpawned = 0;
    _zombiesKilled = 0;
    _zombiesReachedEnd = 0;

    log("WaveManager: Wave %d completed!", _currentWave);

    if (_waveCompletedCallback)
    {
        _waveCompletedCallback(_currentWave);
    }

}

void WaveManager::gameOver()
{
    _state = WaveState::GAME_OVER;
    log("WaveManager: Game Over!");

    if (_gameOverCallback)
    {
        _gameOverCallback();
    }
}

bool WaveManager::isWaveComplete() const
{
    return _zombiesSpawned >= _currentWaveData.zombieCount &&
        _activeZombies.empty();
}

void WaveManager::pauseWave()
{
    // 暂停所有活跃僵尸
    for (auto zombie : _activeZombies)
    {
        if (zombie)
        {
            zombie->pause();
        }
    }
}

void WaveManager::resumeWave()
{
    // 恢复所有活跃僵尸
    for (auto zombie : _activeZombies)
    {
        if (zombie)
        {
            zombie->resume();
        }
    }
}

void WaveManager::reset()
{
    // 清理所有活躍殭屍
    auto it = _activeZombies.begin();
    while (it != _activeZombies.end())
    {
        Zombie* zombie = *it;
        if (zombie && zombie->getParent())
        {
            zombie->removeFromParent();
        }
        it = _activeZombies.erase(it);
    }
    _activeZombies.clear();

    // 重新初始化
    _currentWave = 0;
    _state = WaveState::PREPARING;
    _zombiesSpawned = 0;
    _zombiesKilled = 0;
    _zombiesRemaining = 0;
    _zombiesReachedEnd = 0;
    _spawnTimer = 0;
    _waveTimer = 0;
    _preparationTimer = 10.0f; // 設置為3秒準備時間

    log("WaveManager: Reset complete, active zombies: %d", (int)_activeZombies.size());
}

void WaveManager::clearAllZombies()
{
    auto it = _activeZombies.begin();
    while (it != _activeZombies.end())
    {
        Zombie* zombie = *it;

        // 检查僵尸是否有效
        bool shouldRemove = false;

        if (!zombie) {
            shouldRemove = true;
        }
        else if (!zombie->getParent()) {
            // 僵尸已从场景移除
            shouldRemove = true;
            log("WaveManager: Removing zombie without parent");
        }
        else if (!zombie->isAlive()) {
            // 僵尸已死亡但未被正确清理
            shouldRemove = true;
            log("WaveManager: Removing dead zombie");
        }
        else if (zombie->getPositionX() < -100) {
            // 僵尸已经移出屏幕左侧（可能已经到达终点但未被正确处理）
            shouldRemove = true;
            log("WaveManager: Removing zombie that left screen");
            zombieReachedEnd(zombie);  // 记录僵尸到达终点
        }

        if (shouldRemove) {
            it = _activeZombies.erase(it);
            log("WaveManager: Cleaned up invalid zombie");
        }
        else {
            ++it;
        }
    }
    _activeZombies.clear();
}

std::vector<Zombie*> WaveManager::getZombiesInRow(int row) const{
    std::vector<Zombie*> zombiesInRow;
    for (auto zombie : _activeZombies) {
        if (zombie && zombie->isAlive() && zombie->getRow() == row) {
            zombiesInRow.push_back(zombie);
        }
    }
    return zombiesInRow;
}