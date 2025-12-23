#include "Plant.h"

USING_NS_CC;

bool Plant::init()
{
    if (!Sprite::init())
    {
        return false;
    }

    // 默认值
    _type = PlantType::SUNFLOWER;
    _state = PlantState::IDLE;

    _sunCost = 0;
    _cooldown = 0.0f;
    _health = 100;
    _maxHealth = 100;

    _canAttack = false;
    _attackRange = 0.0f;
    _attackSpeed = 1.0f;
    _attackDamage = 0;
    _attackTimer = 0.0f;

    _canProduce = false;
    _produceInterval = 10.0f;
    _sunProduceAmount = 25;
    _produceTimer = 0.0f;

    _row = -1;
    _col = -1;

    // 设置默认纹理（占位符）
    this->setTextureRect(Rect(0, 0, 50, 50));

    // 设置锚点为底部中心（便于放置在地面上）
    this->setAnchorPoint(Vec2(0.5f, 0.0f));

    // 启用更新
    this->scheduleUpdate();

    return true;
}

bool Plant::initPlant(PlantType type, int sunCost, float cooldown, int health)
{
    _type = type;
    _sunCost = sunCost;
    _cooldown = cooldown;
    _health = health;
    _maxHealth = health;

    // 根据植物类型设置属性
    switch (type)
    {
    case PlantType::SUNFLOWER:
        _canProduce = true;
        _produceInterval = 24.0f; // 每24秒生产一次
        _sunProduceAmount = 25;
        this->setColor(Color3B::YELLOW); // 黄色代表向日葵
        break;

    case PlantType::PEASHOOTER:
        _canAttack = true;
        _attackRange = 500.0f;
        _attackSpeed = 1.5f; // 每秒1.5次
        _attackDamage = 20;
        this->setColor(Color3B::GREEN); // 绿色代表豌豆射手
        break;

    case PlantType::WALLNUT:
        _health = 400; // 坚果墙有更高的生命值
        _maxHealth = 400;
        this->setColor(Color3B(139, 69, 19)); // 棕色代表坚果墙
        break;

    case PlantType::CHERRY_BOMB:
        _canAttack = true;
        _attackRange = 100.0f; // 爆炸范围
        _attackDamage = 180;   // 高伤害
        this->setColor(Color3B::RED); // 红色代表樱桃炸弹
        break;

    default:
        break;
    }

    // 根据植物类型设置大小
    if (type == PlantType::WALLNUT)
    {
        this->setContentSize(Size(60, 70)); // 坚果墙较大
    }
    else
    {
        this->setContentSize(Size(50, 60)); // 标准大小
    }

    log("Plant: Initialized plant type %d, health: %d", (int)type, _health);
    return true;
}

void Plant::update(float delta)
{
    // 检查是否存活
    if (!isAlive())
    {
        return;
    }

    // 根据状态执行相应行为
    switch (_state)
    {
    case PlantState::IDLE:
        onIdle(delta);

        // 攻击型植物自动攻击
        if (_canAttack)
        {
            _attackTimer += delta;
            if (_attackTimer >= 1.0f / _attackSpeed)
            {
                _attackTimer = 0.0f;
                attack(delta);
            }
        }

        // 生产型植物生产阳光
        if (_canProduce)
        {
            _produceTimer += delta;
            if (_produceTimer >= _produceInterval)
            {
                _produceTimer = 0.0f;
                produceSun();
            }
        }
        break;

    case PlantState::ATTACKING:
        onAttack(delta);
        break;

    case PlantState::PRODUCING:
        onProduce(delta);
        break;

    case PlantState::DAMAGED:
        onDamaged();
        // 短暂显示受伤状态后回到IDLE
        _state = PlantState::IDLE;
        break;

    case PlantState::DYING:
        onDying();
        break;

    default:
        break;
    }
}

void Plant::setGridPosition(int row, int col)
{
    _row = row;
    _col = col;
}

void Plant::takeDamage(int damage)
{
    if (!isAlive())
    {
        return;
    }

    _health -= damage;

    // 显示受伤效果
    auto tintAction = TintTo::create(0.1f, Color3B::RED);
    auto reverseAction = TintTo::create(0.1f, this->getColor());
    this->runAction(Sequence::create(tintAction, reverseAction, nullptr));

    log("Plant: Took %d damage, health remaining: %d", damage, _health);

    if (_health <= 0)
    {
        die();
    }
    else
    {
        _state = PlantState::DAMAGED;
        onDamaged();
    }
}

void Plant::attack(float delta)
{
    // 基类实现为空，子类重写
    log("Plant: Attack!");
}

void Plant::produceSun()
{
    // 基类实现为空，子类重写
    log("Plant: Produced sun!");
}

void Plant::die()
{
    _state = PlantState::DYING;
    onDying();

    // 播放死亡动画
    auto fadeOut = FadeOut::create(0.5f);
    auto remove = RemoveSelf::create();
    this->runAction(Sequence::create(fadeOut, remove, nullptr));

    // 延迟设置为死亡状态
    this->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() {
            _state = PlantState::DEAD;
            onDead();
            }),
        nullptr
                ));

    log("Plant: Died");
}