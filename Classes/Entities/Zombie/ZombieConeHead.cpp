#include "ZombieConeHead.h"
#include "./Resources/ResourceLoader.h"
#include <cocos2d.h>

USING_NS_CC;

ZombieConeHead* ZombieConeHead::create()
{
    ZombieConeHead* zombie = new ZombieConeHead();
    if (zombie && zombie->init())
    {
        zombie->autorelease();
        return zombie;
    }
    delete zombie;
    return nullptr;
}

bool ZombieConeHead::init()
{
    if (!Zombie::init())
    {
        return false;
    }

    // 设置路障僵尸的特定属性
    _maxHealth = 280;     // 总生命值（路障200 + 僵尸80）
    _health = _maxHealth;
    _speed = 15.0f;       // 比普通僵尸慢
    _damage = 10;
    _attackInterval = 1.5f;

    _coneDestroyed = false;
    _coneHealth = 200;    // 路障生命值

    // 尝试加载动画
    ResourceLoader* resourceLoader = ResourceLoader::getInstance();
    if (resourceLoader)
    {
        // 尝试加载走动画
        auto walkAnim = resourceLoader->getCachedAnimation("zombie_conehead_walk");
        if (walkAnim)
        {
            _walkAnimation = walkAnim;
            this->runAction(RepeatForever::create(Animate::create(_walkAnimation)));
        }
        else
        {
            log("WARNING: zombie_conehead_walk animation not found, using default sprite");
            // 使用默认精灵
            this->setTextureRect(Rect(0, 0, 60, 100));
            this->setColor(Color3B(0, 150, 0)); // 绿色表示路障僵尸
        }
    }

    // 开始移动
    startMoving();

    // 启动更新
    this->scheduleUpdate();

    log("ZombieConeHead: Initialized successfully");
    return true;
}

bool ZombieConeHead::initWithType(ZombieType type)
{
    if (!Zombie::initWithType(type))
    {
        return false;
    }

    // 路障僵尸的特定初始化
    _maxHealth = 280;
    _health = _maxHealth;
    _coneHealth = 200;
    _coneDestroyed = false;
    _speed = 15.0f;
    _damage = 10;

    return true;
}

void ZombieConeHead::takeDamage(int damage)
{
    if (_state == ZombieState::DEAD)
    {
        return;
    }

    // 先扣除路障生命值
    if (!_coneDestroyed && _coneHealth > 0)
    {
        _coneHealth -= damage;

        // 路障被破坏的效果
        if (_coneHealth <= 0)
        {
            _coneDestroyed = true;
            log("ZombieConeHead: Cone destroyed");

            this->setColor(Color3B::WHITE); // 变成普通僵尸颜色

            // 路障被破坏时的效果
            auto fadeAction = FadeOut::create(0.3f);
            auto removeAction = RemoveSelf::create();
            this->runAction(Sequence::create(fadeAction, removeAction, nullptr));
        }

        // 受伤效果（针对路障）
        auto tintAction = Sequence::create(
            TintTo::create(0.1f, Color3B::GREEN),
            TintTo::create(0.1f, this->getColor()),
            nullptr
        );
        this->runAction(tintAction);

        log("ZombieConeHead: Cone took %d damage, cone health: %d", damage, _coneHealth);
        return;
    }

    // 路障已被破坏，伤害僵尸本体
    Zombie::takeDamage(damage);
}

std::string ZombieConeHead::getWalkAnimationName() const
{
    return "zombie_conehead_walk";
}

std::string ZombieConeHead::getAttackAnimationName() const
{
    return "zombie_conehead_attack";
}

std::string ZombieConeHead::getDeathAnimationName() const
{
    return "zombie_conehead_death";
}