#include "Peashooter.h"
#include "./Game/GameManager.h"
#include "./Resources/AudioManager.h"
#include "./Resources/ResourceLoader.h"

USING_NS_CC;

bool Peashooter::init()
{
    if (!Plant::init())
    {
        return false;
    }

    // 初始化豌豆射手属性
    initPlant(PlantType::PEASHOOTER, 100, 7.5f, 100);

    // 设置豌豆射手特有属性
    _attackSpeed = 2.0f;
    _attackDamage = 20;

    return true;
}

void Peashooter::attack(float delta)
{
    shootPea();
}

void Peashooter::shootPea()
{
    // 播放射击音效
    AudioManager::getInstance()->playSoundEffect(
        ResourceLoader::getInstance()->getSoundEffectPath("sound_shoot")
    );

    // 创建豌豆子弹
    auto pea = Projectile::create();
    if (!pea)
    {
        return;
    }

    // 初始化豌豆子弹属性
    pea->initProjectile(ProjectileType::PEA, _attackDamage, 200.0f);

    // 设置发射位置（从豌豆射手右侧发射）
    Vec2 launchPosition = this->getPosition() + Vec2(30, 55);
    Vec2 direction = Vec2(1.0f, 0.0f);

    // 设置发射参数
    pea->setLaunchParams(launchPosition, direction);

    // 获取父节点并添加子弹
    auto parent = this->getParent();
    if (parent)
    {
        parent->addChild(pea, 5);

        // 添加到游戏管理器
        auto gameManager = GameManager::getInstance();
        if (gameManager)
        {
            gameManager->addProjectile(pea);
        }
    }
}

void Peashooter::onDead() {
    this->_state = PlantState::DEAD;
}