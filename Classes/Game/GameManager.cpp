#include "GameManager.h"
#include "./UI/MenuScene.h"
#include "GameScene.h"
#include "./Resources/AudioManager.h"
#include "./Resources/ResourceLoader.h"
#include <Entities/Projectile/Projectile.h>

USING_NS_CC;

GameManager* GameManager::_instance = nullptr;

GameManager* GameManager::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new GameManager();
        if (_instance && _instance->init())
        {
            // 在Cocos2d-x 3.17中，Ref的autorelease()管理方式不同
            // 我们使用手动内存管理
        }
        else
        {
            delete _instance;
            _instance = nullptr;
        }
    }
    return _instance;
}

GameManager::GameManager()
    : _currentState(GameState::MENU)
    , _sunCount(0)
    , _currentLevel(1)
    , _playerScore(0)
    , _isSoundEnabled(true)
    , _isMusicEnabled(true)
{
}

GameManager::~GameManager()
{
    _instance = nullptr;
}

bool GameManager::init()
{
    // 加载游戏配置
    auto userDefaults = UserDefault::getInstance();

    _currentLevel = userDefaults->getIntegerForKey("current_level", 1);
    _playerScore = userDefaults->getIntegerForKey("player_score", 0);
    _isSoundEnabled = userDefaults->getBoolForKey("sound_enabled", true);
    _isMusicEnabled = userDefaults->getBoolForKey("music_enabled", true);

    return true;
}

bool GameManager::spendSun(int amount)
{
    if (_sunCount >= amount)
    {
        _sunCount -= amount;
        return true;
    }
    return false;
}

void GameManager::startNewGame()
{
    // 重置游戏数据
    _sunCount = 500; // 初始阳光
    _currentState = GameState::PLAYING;
    _playerScore = 0;
    _projectiles.clear();
    clearAllProjectiles();
    // 切换到游戏场景
    goToGameScene();
}

void GameManager::pauseGame()
{
    if (_currentState == GameState::PLAYING)
    {
        _currentState = GameState::PAUSED;

        auto director = Director::getInstance();

        // Cocos2d-x 3.17中，使用以下方式暂停
        director->pause();

        // 暂停背景音乐
        AudioManager::getInstance()->pauseBackgroundMusic();
    }
}

void GameManager::resumeGame()
{
    if (_currentState == GameState::PAUSED)
    {
        _currentState = GameState::PLAYING;

        auto director = Director::getInstance();

        // Cocos2d-x 3.17中，使用以下方式恢复
        director->resume();

        // 恢复背景音乐
        AudioManager::getInstance()->resumeBackgroundMusic();
    }
}

void GameManager::restartGame()
{
    // 重置游戏数据
    _sunCount = 500;
    _playerScore = 0;
    _currentState = GameState::PLAYING;
    clearAllProjectiles();
    // 重新加载游戏场景
    goToGameScene();
}

void GameManager::gameOver(bool isWin)
{
    _currentState = GameState::GAME_OVER;

    // 保存游戏数据
    auto userDefaults = UserDefault::getInstance();
    userDefaults->setIntegerForKey("player_score", _playerScore);
    userDefaults->setIntegerForKey("current_level", _currentLevel);
    userDefaults->flush();

    // 延迟返回菜单
    auto director = Director::getInstance();
    director->getRunningScene()->runAction(
        Sequence::create(
            DelayTime::create(2.0f),
            CallFunc::create([]() {
                GameManager::getInstance()->goToMenuScene();
                }),
            nullptr
                    )
    );
}

void GameManager::goToMenuScene()
{
    _currentState = GameState::MENU;

    auto director = Director::getInstance();
    auto scene = MenuScene::createScene();

    // 切换场景的过渡效果
    director->replaceScene(TransitionFade::create(0.5f, scene));
}

void GameManager::goToGameScene()
{
    _currentState = GameState::PLAYING;

    auto director = Director::getInstance();
    auto scene = GameScene::createScene();  // 使用新的GameScene

    // 切换场景的过渡效果
    director->replaceScene(TransitionFade::create(0.5f, scene));

    // 播放游戏背景音乐（在GameScene中已经处理）
    // AudioManager::getInstance()->playBackgroundMusic("Sounds/BGM/game_bgm.mp3", true);
}

void GameManager::addProjectile(Projectile* projectile)
{
    if (!projectile)
    {
        return;
    }

    // 检查是否已经存在
    auto it = std::find(_projectiles.begin(), _projectiles.end(), projectile);
    if (it == _projectiles.end())
    {
        _projectiles.push_back(projectile);
        log("GameManager: Projectile added, total: %d", (int)_projectiles.size());
    }
}

void GameManager::updateProjectiles(float delta)
{
    // 使用迭代器安全地遍历和移除
    for (auto it = _projectiles.begin(); it != _projectiles.end(); )
    {
        Projectile* projectile = *it;

        // 检查指针是否有效
        if (!projectile)
        {
            log("GameManager: Removing null projectile");
            it = _projectiles.erase(it);
            continue;
        }

        // 检查子弹是否已经被销毁或移除
        if (!projectile->isAlive() || 
            projectile->getState() == ProjectileState::DEAD ||
            !projectile->getParent())  // 新增检查：子弹是否已经从场景中移除
        {
            log("GameManager: Removing dead or detached projectile");
            
            // 安全地从列表中移除，不需要从父节点移除（因为可能已经被移除了）
            it = _projectiles.erase(it);
            continue;
        }

        // 更新存活的子弹
        projectile->update(delta);
        ++it;
    }
}

void GameManager::clearAllProjectiles()
{
    log("GameManager: Clearing all projectiles (%d)", (int)_projectiles.size());

    for (auto projectile : _projectiles)
    {
        if (projectile && projectile->getParent())
        {
            projectile->removeFromParent();
        }
    }
    _projectiles.clear();
}
