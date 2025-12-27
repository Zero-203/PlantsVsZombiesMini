#include "ZombieBucketHead.h"
#include "./Resources/ResourceLoader.h"
#include <cocos2d.h>

USING_NS_CC;

ZombieBucketHead* ZombieBucketHead::create()
{
    ZombieBucketHead* zombie = new ZombieBucketHead();
    if (zombie && zombie->init())
    {
        zombie->autorelease();
        return zombie;
    }
    delete zombie;
    return nullptr;
}

bool ZombieBucketHead::init()
{
    if (!Zombie::init())
    {
        return false;
    }

    // 设置铁桶僵尸的特定属性
    _maxHealth = 580;     // 总生命值（铁桶500 + 僵尸80）
    _health = _maxHealth;
    _speed = 12.0f;       // 最慢的僵尸
    _damage = 15;         // 伤害更高
    _attackInterval = 2.0f;

    _bucketDestroyed = false;
    _bucketHealth = 500;  // 铁桶生命值

    // 尝试加载动画
    ResourceLoader* resourceLoader = ResourceLoader::getInstance();
    if (resourceLoader)
    {
        // 尝试加载走动画
        auto walkAnim = resourceLoader->getCachedAnimation("zombie_buckethead_walk");
        if (walkAnim)
        {
            _walkAnimation = walkAnim;
            this->runAction(RepeatForever::create(Animate::create(_walkAnimation)));
        }
        else
        {
            log("WARNING: zombie_buckethead_walk animation not found, using default sprite");
            // 使用默认精灵
            this->setTextureRect(Rect(0, 0, 60, 100));
            this->setColor(Color3B(100, 100, 100)); // 灰色表示铁桶僵尸
        }
    }

    // 开始移动
    startMoving();

    // 启动更新
    this->scheduleUpdate();

    log("ZombieBucketHead: Initialized successfully");
    return true;
}

bool ZombieBucketHead::initWithType(ZombieType type)
{
    if (!Zombie::initWithType(type))
    {
        return false;
    }

    // 铁桶僵尸的特定初始化
    _maxHealth = 580;
    _health = _maxHealth;
    _bucketHealth = 500;
    _bucketDestroyed = false;
    _speed = 12.0f;
    _damage = 15;

    return true;
}

void ZombieBucketHead::takeDamage(int damage)
{
    if (_state == ZombieState::DEAD)
    {
        return;
    }

    // 先扣除铁桶生命值
    if (!_bucketDestroyed && _bucketHealth > 0)
    {
        _bucketHealth -= damage;

        // 铁桶被破坏的效果
        if (_bucketHealth <= 0)
        {
            _bucketDestroyed = true;
            log("ZombieBucketHead: Bucket destroyed");

            // 铁桶被破坏时的效果
            auto scaleAction = ScaleTo::create(0.2f, 1.0f, 0.8f);
            auto restoreAction = ScaleTo::create(0.2f, 1.0f, 1.0f);
            this->runAction(Sequence::create(scaleAction, restoreAction, nullptr));

            // 改变颜色表示铁桶已破
            this->setColor(Color3B::WHITE);
        }

        // 受伤效果（针对铁桶）
        auto tintAction = Sequence::create(
            TintTo::create(0.1f, Color3B(200, 200, 200)),
            TintTo::create(0.1f, this->getColor()),
            nullptr
        );
        this->runAction(tintAction);

        log("ZombieBucketHead: Bucket took %d damage, bucket health: %d", damage, _bucketHealth);
        return;
    }

    // 铁桶已被破坏，伤害僵尸本体
    Zombie::takeDamage(damage);
}

std::string ZombieBucketHead::getWalkAnimationName() const
{
    return "zombie_buckethead_walk";
}

std::string ZombieBucketHead::getAttackAnimationName() const
{
    return "zombie_buckethead_attack";
}

std::string ZombieBucketHead::getDeathAnimationName() const
{
    return "zombie_buckethead_death";
}