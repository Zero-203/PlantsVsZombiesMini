#include "ZombieNormal.h"
#include "./Resources/ResourceLoader.h"
#include <cocos2d.h>

USING_NS_CC;

ZombieNormal* ZombieNormal::create()
{
    ZombieNormal* zombie = new ZombieNormal();
    if (zombie && zombie->init())
    {
        zombie->autorelease();
        return zombie;
    }
    delete zombie;
    return nullptr;
}

bool ZombieNormal::init()
{
    if (!Zombie::init())
    {
        return false;
    }


    // 設置普通殭屍的特定屬性
    _maxHealth = 100;
    _health = _maxHealth;
    _speed = 30.0f; // 稍微加快速度
    _damage = 10;
    _attackInterval = 1.5f;

    // 創建默認的精靈（如果動畫加載失敗）
    this->setTextureRect(Rect(0, 0, 60, 100));
    this->setColor(Color3B::WHITE);

    // 嘗試加載動畫
    ResourceLoader* resourceLoader = ResourceLoader::getInstance();
    if (resourceLoader)
    {
        // 嘗試加載走動動畫
        auto walkAnim = resourceLoader->getCachedAnimation("zombie_normal_walk");
        if (walkAnim)
        {
            _walkAnimation = walkAnim;
            this->runAction(RepeatForever::create(Animate::create(_walkAnimation)));
        }
        else
        {
            log("WARNING: zombie_normal_walk animation not found, using default sprite");
        }
    }

    // 開始移動
    startMoving();

    // 啟動更新
    this->scheduleUpdate();

    log("ZombieNormal: Initialized successfully at row %d", _row);
    return true;
}

bool ZombieNormal::initWithType(ZombieType type)
{
    if (!Zombie::initWithType(type))
    {
        return false;
    }

    // 普通僵尸的特定初始化
    _maxHealth = 100;
    _health = _maxHealth;
    _speed = 20.0f;
    _damage = 10;

    // 加载普通僵尸的特定资源
    ResourceLoader* resourceLoader = ResourceLoader::getInstance();
    if (resourceLoader)
    {
        // 如果没有动画，创建默认的
        if (!resourceLoader->hasAnimation("zombie_normal_walk"))
        {
            // 创建默认动画帧
            Vector<SpriteFrame*> frames;
            for (int i = 1; i <= 4; i++)
            {
                std::string frameName = StringUtils::format("Images/Zombies/Normal/walk_%02d.png", i);
                auto frame = SpriteFrame::create(frameName, Rect(0, 0, 100, 100));
                if (frame)
                {
                    frames.pushBack(frame);
                }
            }

            if (!frames.empty())
            {
                auto animation = Animation::createWithSpriteFrames(frames, 0.2f);
                resourceLoader->cacheAnimation("zombie_normal_walk", animation);
            }
        }
    }

    return true;
}

std::string ZombieNormal::getWalkAnimationName() const
{
    return "zombie_normal_walk";
}

std::string ZombieNormal::getAttackAnimationName() const
{
    return "zombie_normal_attack";
}

std::string ZombieNormal::getDeathAnimationName() const
{
    return "zombie_normal_death";
}