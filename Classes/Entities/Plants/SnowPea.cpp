#include "SnowPea.h"
#include "./Game/GameManager.h"
#include "./Resources/AudioManager.h"
#include "./Resources/ResourceLoader.h"

USING_NS_CC;

bool SnowPea::init()
{
    if (!Plant::init())
    {
        return false;
    }

    // 初始化寒冰射手属性
    initPlant(PlantType::SNOW_PEA, 150, 7.5f, 100);  // 阳光消耗150，冷却7.5秒，生命值100

    // 设置攻击属性
    _attackSpeed = 2.0f;     // 攻击速度略慢于普通豌豆
    _attackDamage = 20;      // 伤害与普通豌豆相同
    _attackRange = 800.0f;   // 攻击范围

    return true;
}

void SnowPea::attack(float delta)
{
    shootIcePea();
}

void SnowPea::shootIcePea()
{
    // 播放射击音效（可以使用与普通豌豆不同的音效）
    AudioManager::getInstance()->playSoundEffect(
        ResourceLoader::getInstance()->getSoundEffectPath("sound_shoot")
    );

    // 创建寒冰豌豆子弹
    auto icePea = Projectile::create();
    if (!icePea)
    {
        return;
    }

    // 初始化寒冰豌豆子弹属性
    icePea->initProjectile(ProjectileType::SNOW_PEA, _attackDamage, 200.0f);

    // 设置发射位置和方向（基于植物的朝向）
    Vec2 launchPosition = this->getPosition() + Vec2(30, 55);  // 稍微偏移
    Vec2 direction = Vec2(1.0f, 0.0f);  // 向右发射

    // 设置发射参数
    icePea->setLaunchParams(launchPosition, direction);

    // 添加到场景中
    auto parent = this->getParent();
    if (parent)
    {
        parent->addChild(icePea, 5);

        // 通知游戏管理器添加子弹
        auto gameManager = GameManager::getInstance();
        if (gameManager)
        {
            gameManager->addProjectile(icePea);
        }
    }
}

void SnowPea::onDead() {
    this->_state = PlantState::DEAD;
}