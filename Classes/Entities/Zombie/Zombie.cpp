#include "Zombie.h"
#include "./Entities/Plants/Plant.h"
#include "./Entities/Projectile/Projectile.h"
#include "./Game/GameManager.h"
#include "./Resources/ResourceLoader.h"
#include <cocos2d.h>
#include "Game/WaveManager.h"
#include "Game/GridSystem.h"
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
    , _originalSpeed(20.0f)    
    , _isFrozen(false)         
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
            _originalSpeed = 20.0f;  // 添加
            _damage = 10;
            break;

        case ZombieType::CONEHEAD:
            _maxHealth = 280;
            _health = _maxHealth;
            _speed = 15.0f;
            _originalSpeed = 15.0f;  // 添加
            _damage = 10;
            break;

        case ZombieType::BUCKETHEAD:
            _maxHealth = 580;
            _health = _maxHealth;
            _speed = 12.0f;
            _originalSpeed = 12.0f;  // 添加
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

    // 確保設置物理尺寸（重要！）
    this->setContentSize(Size(60, 100));
    this->setAnchorPoint(Vec2(0.3f, 0.3f)); // 調整錨點使其腳部對齊地面

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

    this->scheduleUpdate();

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
            unFreeze();
            this->setColor(Color3B::WHITE);
        }
    }

    // 检查碰撞
    checkCollisions();

    _targetPlant = this->findPlantInFront();
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

    // 立即通知WaveManager
    auto waveManager = WaveManager::getInstance();
    if (waveManager)
    {
        waveManager->zombieKilled(this);
    }

    // 播放死亡動畫並立即移除
    if (_deathAnimation)
    {
        auto deathAction = Animate::create(_deathAnimation);
        auto removeAction = CallFunc::create([this]() {
            if (this->getParent())
            {
                this->removeFromParent();
            }
            });
        this->runAction(Sequence::create(deathAction, removeAction, nullptr));
    }
    else
    {
        // 沒有動畫，立即移除
        if (this->getParent())
        {
            this->removeFromParent();
        }
    }

    log("Zombie: Died");
}

void Zombie::freeze(float duration)
{
    if (_state == ZombieState::DEAD) return;

    _freezeTimer = duration;
    _isFrozen = true;

    // 保存原始速度（如果不是已经被冰冻）
    if (!_isFrozen) {
        _originalSpeed = _speed;
    }

    // 设置减速效果（例如减速到原速度的30%）
    float slowFactor = 0.3f;  // 调整为需要的减速比例
    _speed = _originalSpeed * slowFactor;

    // 冰冻视觉效果
    this->setColor(Color3B(100, 100, 255)); // 蓝色冰冻效果
    this->setOpacity(180); // 稍微透明一点

    log("Zombie: Frozen for %.1f seconds, speed reduced from %.1f to %.1f",
        duration, _originalSpeed, _speed);
}

void Zombie::unFreeze()
{
    if (!_isFrozen) return;

    // 恢复原始速度
    _speed = _originalSpeed;
    _isFrozen = false;

    // 恢复视觉外观
    this->setColor(Color3B::WHITE);
    this->setOpacity(255);

    log("Zombie: Unfrozen, speed restored to %.1f", _speed);
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
    auto gridSystem = GridSystem::getInstance();
    if (!gridSystem) return nullptr;

    // 获取僵尸当前位置对应的网格
    int col, row;
    Vec2 worldPos = this->getPosition();

    // 尝试将世界坐标转换为网格坐标
    if (gridSystem->worldToGrid(worldPos, row, col)) {
        // 检查当前格子是否有植物
        Plant* plant = gridSystem->getPlantAt(row, col);
        if (plant && plant->isAlive()) {
            float distance = std::abs(this->getPositionX() - plant->getPositionX());
            if (distance < 5) { // 攻击范围
                return plant;
            }
        }

        // 检查前方格子（左侧）
        if (col > 0) {
            plant = gridSystem->getPlantAt(row, col - 1);
            if (plant && plant->isAlive()) {
                float distance = std::abs(this->getPositionX() - plant->getPositionX());
                if (distance < 30) { // 稍大的检测范围
                    return plant;
                }
            }
        }
    }


    return nullptr;
}