#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "cocos2d.h"

class GameManager : public cocos2d::Ref
{
public:
    // 单例访问点
    static GameManager* getInstance();

    // 游戏状态枚举
    enum class GameState
    {
        MENU,       // 菜单状态
        PLAYING,    // 游戏进行中
        PAUSED,     // 游戏暂停
        GAME_OVER   // 游戏结束
    };

    // 游戏状态管理
    GameState getCurrentState() const { return _currentState; }
    void setCurrentState(GameState state) { _currentState = state; }

    // 资源管理
    int getSunCount() const { return _sunCount; }
    void setSunCount(int count) { _sunCount = count; }
    void addSun(int amount) { _sunCount += amount; }
    bool spendSun(int amount);

    // 游戏控制
    void startNewGame();
    void pauseGame();
    void resumeGame();
    void restartGame();
    void gameOver(bool isWin);

    // 场景管理
    void goToMenuScene();
    void goToGameScene();

private:
    GameManager();
    virtual ~GameManager();

    // 禁止拷贝
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    // 初始化
    bool init();

private:
    static GameManager* _instance;

    // 游戏状态
    GameState _currentState;

    // 游戏资源
    int _sunCount;

    // 游戏配置
    int _currentLevel;
    int _playerScore;
    bool _isSoundEnabled;
    bool _isMusicEnabled;
};

#endif // GAME_MANAGER_H