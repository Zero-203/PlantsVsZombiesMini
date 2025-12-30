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
    // 先调用父类的initWithType，指定类型
    if (!Zombie::init())
    {
        return false;
    }

    // 鐵桶特有屬性
    _bucketHealth = 200;
    _bucketDestroyed = false;

    // 注意：父类Zombie::initWithType已经调用了scheduleUpdate()，这里不要重复调用
    // 設置物理尺寸和錨點
    this->setContentSize(Size(60, 100));
    this->setAnchorPoint(Vec2(0.5f, 0.3f)); // 腳部對齊地面

    // 加载特定动画
    ResourceLoader* resourceLoader = ResourceLoader::getInstance();
    if (resourceLoader)
    {
        // 嘗試加載鐵桶動畫
        _walkAnimation = resourceLoader->getCachedAnimation("zombie_buckethead_walk");
        _attackAnimation = resourceLoader->getCachedAnimation("zombie_buckethead_attack");
        _deathAnimation = resourceLoader->getCachedAnimation("zombie_normal_death");

        // 如果沒有鐵桶動畫，使用普通僵屍動畫作為備用
        if (!_walkAnimation)
        {
            _walkAnimation = resourceLoader->getCachedAnimation("zombie_normal_walk");
            log("WARNING: Using normal zombie walk animation for buckethead");
        }

        if (_walkAnimation)
        {
            log("ZombieBucketHead: Walk animation loaded successfully, frame count: %d",
                _walkAnimation->getFrames().size());
            this->runAction(RepeatForever::create(Animate::create(_walkAnimation)));
        }
        else
        {
            log("ZombieBucketHead: ERROR: Walk animation not loaded!");
            // 連備用動畫都沒有，創建簡單的視覺效果
            this->setTextureRect(Rect(0, 0, 60, 100));
            this->setColor(Color3B(50, 50, 50)); // 灰色表示鐵桶
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
    _bucketHealth = 200;
    _bucketDestroyed = false;

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
        if (_bucketHealth >= damage) {
            _bucketHealth -= damage;
            damage = 0;
        }
        else
        {
            damage -= _bucketHealth;
            _bucketHealth = 0;
        }

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
    }

    if(damage>0)
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
    return "zombie_normal_death";
}