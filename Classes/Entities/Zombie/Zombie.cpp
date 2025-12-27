#include "Zombie.h"
#include "./Entities/Plants/Plant.h"
#include "./Entities/Projectile/Projectile.h"
#include "./Game/GameManager.h"
#include "./Resources/ResourceLoader.h"
#include <cocos2d.h>
#include "Game/WaveManager.h"
USING_NS_CC;

Zombie::Zombie()
    : _type(ZombieType::NORMAL)
    , _state(ZombieState::ALIVE)
    , _row(0)
    , _health(100)
    , _maxHealth(100)
    , _speed(20.0f)
    , _damage(10)
    , _attackTimer(0)
    , _attackInterval(1.0f)
    , _freezeTimer(0)
    , _targetPlant(nullptr)
    , _walkAnimation(nullptr)
    , _attackAnimation(nullptr)
    , _deathAnimation(nullptr)
{
}

Zombie::~Zombie()
{
    _targetPlant = nullptr;
}

bool Zombie::init()
{
    if (!Sprite::init())
    {
        return false;
    }

    return initWithType(ZombieType::NORMAL);
}

bool Zombie::initWithType(ZombieType type)
{
    _type = type;

    // 根据类型设置属性
    switch (type)
    {
        case ZombieType::NORMAL:
            _maxHealth = 100;
            _health = _maxHealth;
            _speed = 20.0f;
            _damage = 10;
            break;

        case ZombieType::CONEHEAD:
            _maxHealth = 280;
            _health = _maxHealth;
            _speed = 15.0f;
            _damage = 10;
            break;

        case ZombieType::BUCKETHEAD:
            _maxHealth = 580;
            _health = _maxHealth;
            _speed = 12.0f;
            _damage = 15;
            break;

        default:
            break;
    }

    // 设置初始状态
    _state = ZombieState::ALIVE;
    _attackTimer = 0;
    _freezeTimer = 0;
    _targetPlant = nullptr;

    // 加载动画资源
    ResourceLoader* resourceLoader = ResourceLoader::getInstance();
    if (resourceLoader)
    {
        _walkAnimation = resourceLoader->getCachedAnimation(getWalkAnimationName());
        _attackAnimation = resourceLoader->getCachedAnimation(getAttackAnimationName());
        _deathAnimation = resourceLoader->getCachedAnimation(getDeathAnimationName());

        if (_walkAnimation)
        {
            this->runAction(RepeatForever::create(Animate::create(_walkAnimation)));
        }
    }

    // 开始移动
    startMoving();

    log("Zombie: Initialized type %d at row %d", (int)type, _row);
    return true;
}

Zombie* Zombie::create(ZombieType type)
{
    Zombie* zombie = new Zombie();
    if (zombie && zombie->initWithType(type))
    {
        zombie->autorelease();
        return zombie;
    }
    delete zombie;
    return nullptr;
}

void Zombie::update(float delta)
{
    if (_state == ZombieState::DEAD)
    {
        return;
    }

    // 更新冰冻状态
    if (_freezeTimer > 0)
    {
        _freezeTimer -= delta;
        if (_freezeTimer <= 0)
        {
            // 解除冰冻
            this->setColor(Color3B::WHITE);
        }
    }

    // 检查碰撞
    checkCollisions();

    // 如果有目标植物，则攻击
    if (_targetPlant && _targetPlant->isAlive())
    {
        _attackTimer += delta;
        if (_attackTimer >= _attackInterval)
        {
            attack(_targetPlant);
            _attackTimer = 0;
        }

        // 播放攻击动画
        if (_state != ZombieState::ATTACKING)
        {
            _state = ZombieState::ATTACKING;
            playAttackAnimation();
        }
    }
    else
    {
        // 没有目标，移动
        if (_state == ZombieState::ATTACKING)
        {
            _state = ZombieState::ALIVE;
            playWalkAnimation();
        }

        updateMovement(delta);
    }
}

void Zombie::takeDamage(int damage)
{
    if (_state == ZombieState::DEAD)
    {
        return;
    }

    _health -= damage;

    // 受伤效果
    auto tintAction = Sequence::create(
        TintTo::create(0.1f, Color3B::RED),
        TintTo::create(0.1f, this->getColor()),
        nullptr
    );
    this->runAction(tintAction);

    log("Zombie: Took %d damage, health: %d/%d", damage, _health, _maxHealth);

    if (_health <= 0)
    {
        die();
    }
}

void Zombie::attack(Plant* plant)
{
    if (plant && plant->isAlive())
    {
        plant->takeDamage(_damage);
        log("Zombie: Attacked plant for %d damage", _damage);
    }
}

// Zombie.cpp 中的 die() 方法修改：
void Zombie::die()
{
    if (_state == ZombieState::DEAD)
    {
        return;
    }

    _state = ZombieState::DEAD;
    stopMoving();

    // 停止所有動作
    this->stopAllActions();

    // 停止更新
    this->unscheduleUpdate();

    // 播放死亡動畫
    playDeathAnimation();

    // 延遲移除
    this->runAction(Sequence::create(
        DelayTime::create(1.0f),
        CallFunc::create([this]() {
            // 通知 WaveManager
            auto waveManager = WaveManager::getInstance();
            if (waveManager)
            {
                waveManager->zombieKilled(this);
            }

            // 從場景移除
            if (this->getParent())
            {
                this->removeFromParent();
            }
            }),
        nullptr
    ));

    log("Zombie: Died");
}

void Zombie::freeze(float duration)
{
    _freezeTimer = duration;
    this->setColor(Color3B(100, 100, 255)); // 蓝色冰冻效果
    _speed *= 0.5f; // 减速50%
}

void Zombie::startMoving()
{
    // 开始移动逻辑
    log("Zombie: Started moving");
}

void Zombie::stopMoving()
{
    // 停止移动逻辑
    log("Zombie: Stopped moving");
}

void Zombie::playWalkAnimation()
{
    if (_walkAnimation)
    {
        this->stopAllActions();
        this->runAction(RepeatForever::create(Animate::create(_walkAnimation)));
    }
}

void Zombie::playAttackAnimation()
{
    if (_attackAnimation)
    {
        this->stopAllActions();
        this->runAction(RepeatForever::create(Animate::create(_attackAnimation)));
    }
}

void Zombie::playDeathAnimation()
{
    if (_deathAnimation)
    {
        this->stopAllActions();
        this->runAction(Sequence::create(
            Animate::create(_deathAnimation),
            nullptr
        ));
    }
}

std::string Zombie::getWalkAnimationName() const
{
    switch (_type)
    {
        case ZombieType::NORMAL:
            return "zombie_normal_walk";
        case ZombieType::CONEHEAD:
            return "zombie_conehead_walk";
        case ZombieType::BUCKETHEAD:
            return "zombie_buckethead_walk";
        default:
            return "zombie_normal_walk";
    }
}

std::string Zombie::getAttackAnimationName() const
{
    switch (_type)
    {
        case ZombieType::NORMAL:
            return "zombie_normal_attack";
        case ZombieType::CONEHEAD:
            return "zombie_conehead_attack";
        case ZombieType::BUCKETHEAD:
            return "zombie_buckethead_attack";
        default:
            return "zombie_normal_attack";
    }
}

std::string Zombie::getDeathAnimationName() const
{
    switch (_type)
    {
        case ZombieType::NORMAL:
            return "zombie_normal_death";
        case ZombieType::CONEHEAD:
            return "zombie_conehead_death";
        case ZombieType::BUCKETHEAD:
            return "zombie_buckethead_death";
        default:
            return "zombie_normal_death";
    }
}

void Zombie::updateMovement(float delta)
{
    if (_state == ZombieState::DEAD || _state == ZombieState::ATTACKING)
    {
        return;
    }

    // 计算实际速度（考虑冰冻效果）
    float actualSpeed = _speed;
    if (_freezeTimer > 0)
    {
        actualSpeed *= 0.5f;
    }

    // 向左移动
    float moveDistance = -actualSpeed * delta;
    this->setPositionX(this->getPositionX() + moveDistance);

    // 检查是否到达终点（屏幕左侧）
    auto visibleSize = Director::getInstance()->getVisibleSize();
    if (this->getPositionX() < -50)
    {
        log("Zombie: Reached end of screen");

        // 立即通知 WaveManager
        auto waveManager = WaveManager::getInstance();
        if (waveManager)
        {
            waveManager->zombieReachedEnd(this);
        }

        // 停止所有動作後移除
        this->stopAllActions();
        if (this->getParent())
        {
            this->removeFromParent();
        }
    }
}

void Zombie::checkCollisions()
{
    // 检查与植物的碰撞
    auto gameManager = GameManager::getInstance();
    if (!gameManager) return;

    // 这里需要根据实际网格系统实现碰撞检测
    // 暂时使用简单的距离检测

    if (!_targetPlant || !_targetPlant->isAlive())
    {
        // 寻找前方的植物
        _targetPlant = findPlantInFront();
    }
}

Plant* Zombie::findPlantInFront()
{
    // 实际实现需要与GridSystem交互
    // 这里提供一个框架

    /*
    auto gridSystem = GridSystem::getInstance();
    if (!gridSystem) return nullptr;

    // 获取当前僵尸所在的行
    int currentCol = 0;
    // 需要将世界坐标转换为网格坐标

    // 检查前方格子是否有植物
    for (int col = currentCol; col >= 0; col--)
    {
        Plant* plant = gridSystem->getPlantAt(_row, col);
        if (plant && plant->isAlive())
        {
            // 检查距离
            float distance = abs(this->getPositionX() - plant->getPositionX());
            if (distance < 50) // 攻击范围
            {
                return plant;
            }
        }
    }
    */

    return nullptr;
}