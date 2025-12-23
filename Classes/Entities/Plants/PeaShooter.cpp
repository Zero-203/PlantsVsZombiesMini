#include "Peashooter.h"
#include "./Game/GameManager.h"
#include "./Resources/AudioManager.h"

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
    _attackSpeed = 1.5f; // 每秒发射1.5颗豌豆

    // 设置颜色和大小
    this->setColor(Color3B(0, 200, 0)); // 亮绿色
    this->setContentSize(Size(50, 70));

    // 添加动画（简化为颜色变化）
    auto idleAction = RepeatForever::create(
        Sequence::create(
            TintTo::create(0.5f, Color3B(0, 200, 0)),
            TintTo::create(0.5f, Color3B(0, 150, 0)),
            nullptr
        )
    );
    this->runAction(idleAction);

    return true;
}

void Peashooter::attack(float delta)
{
    shootPea();
}

void Peashooter::shootPea()
{
    log("Peashooter: Shooting pea!");

    // 播放射击音效
    auto audioManager = AudioManager::getInstance();
    if (audioManager)
    {
        audioManager->playSoundEffect("Sounds/SFX/shoot.mp3");
    }

    // 创建豌豆子弹（简化版：使用一个Sprite）
    auto pea = Sprite::create();
    pea->setTextureRect(Rect(0, 0, 10, 10));
    pea->setColor(Color3B(0, 255, 0));
    pea->setPosition(this->getPosition() + Vec2(30, 20)); // 从豌豆射手嘴部发射

    // 获取父节点并添加豌豆
    auto parent = this->getParent();
    if (parent)
    {
        parent->addChild(pea, 5); // 较高层级

        // 豌豆移动动画
        float screenWidth = Director::getInstance()->getVisibleSize().width;
        float moveDistance = screenWidth - pea->getPositionX();
        float moveDuration = moveDistance / 200.0f; // 速度200像素/秒

        auto moveAction = MoveBy::create(moveDuration, Vec2(moveDistance, 0));
        auto removeAction = RemoveSelf::create();

        pea->runAction(Sequence::create(moveAction, removeAction, nullptr));

        // 简化：这里应该添加碰撞检测，后续在碰撞管理器中实现
    }
}