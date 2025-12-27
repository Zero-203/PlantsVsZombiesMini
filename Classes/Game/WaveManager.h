#pragma once
#ifndef __WAVE_MANAGER_H__
#define __WAVE_MANAGER_H__

#include "cocos2d.h"
#include <vector>
#include <functional>

class Zombie; // 前向声明

// 波次数据
struct WaveData {
    int waveNumber;          // 波次编号
    int zombieCount;         // 僵尸总数
    int zombieTypes;         // 僵尸类型数量
    float spawnInterval;     // 生成间隔（秒）
    float waveDelay;         // 波次间延迟（秒）

    WaveData(int num = 1) : waveNumber(num),
        zombieCount(2 + num),  // 減少初始數量以便測試
        zombieTypes(1),
        spawnInterval(1.5f),   // 減少生成間隔
        waveDelay(10.0f) {
        if (spawnInterval < 0.5f) spawnInterval = 0.5f;
    }
};

// 波次状态
enum class WaveState {
    PREPARING,      // 准备阶段
    SPAWNING,       // 生成中
    COMPLETED,      // 已完成
    GAME_OVER       // 游戏结束
};

typedef std::function<void(int)> WaveCallback; // 波次回调

class WaveManager {
public:
    static WaveManager* getInstance();

    // 初始化波次管理器
    void init(int totalWaves = 10);

    // 获取波次信息
    int getCurrentWave() const { return _currentWave; }
    int getTotalWaves() const { return _totalWaves; }
    int getZombiesRemaining() const { return _zombiesRemaining; }
    int getZombiesSpawned() const { return _zombiesSpawned; }
    WaveState getState() const { return _state; }

    // 波次控制
    void startNextWave();
    void pauseWave();
    void resumeWave();
    void reset();

    // 僵尸管理
    void zombieSpawned(Zombie* zombie);
    void zombieKilled(Zombie* zombie);
    void zombieReachedEnd(Zombie* zombie);

    // 更新
    void update(float delta);

    // 设置回调
    void setWaveStartedCallback(const WaveCallback& callback) { _waveStartedCallback = callback; }
    void setWaveCompletedCallback(const WaveCallback& callback) { _waveCompletedCallback = callback; }
    void setWaveAllCompletedCallback(const std::function<void()>& callback) { _allWavesCompletedCallback = callback; }
    void setGameOverCallback(const std::function<void()>& callback) { _gameOverCallback = callback; }

    // 获取当前波次数据
    const WaveData& getCurrentWaveData() const { return _currentWaveData; }

    // 生成殭屍（改為public以便測試）
    Zombie* spawnRandomZombie();

    // 添加清理方法
    void clearAllZombies();

    // 修改數據類型
    //std::vector<Zombie*> _activeZombies;

private:
    WaveManager();
    ~WaveManager();

    // 禁止复制
    WaveManager(const WaveManager&) = delete;
    WaveManager& operator=(const WaveManager&) = delete;

    // 生成僵尸
    void spawnZombie();

    // 波次管理
    void startWave(int waveNumber);
    void completeWave();
    void gameOver();

    // 检查波次完成条件
    bool isWaveComplete() const;

private:
    static WaveManager* _instance;

    // 波次数据
    int _currentWave;
    int _totalWaves;
    WaveData _currentWaveData;
    WaveState _state;

    // 僵尸数据
    int _zombiesSpawned;
    int _zombiesKilled;
    int _zombiesRemaining;
    int _zombiesReachedEnd;

    // 计时器
    float _spawnTimer;
    float _waveTimer;
    float _preparationTimer;

    // 回调函数
    WaveCallback _waveStartedCallback;
    WaveCallback _waveCompletedCallback;
    std::function<void()> _allWavesCompletedCallback;
    std::function<void()> _gameOverCallback;

    // 清理僵尸列表的函数声明
    void cleanupZombieList();

    // 僵尸列表
    std::vector<Zombie*> _activeZombies;
};

#endif // __WAVE_MANAGER_H__