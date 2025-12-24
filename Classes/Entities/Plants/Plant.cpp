// Plant.cpp - 完整文件
#include "Plant.h"
#include "./Resources/ResourceLoader.h"

USING_NS_CC;

bool Plant::init()
{
    if (!Sprite::init())
    {
        return false;
    }

    // 初始化
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

    // 动画相关初始化
    _currentAnimate = nullptr;

    // 自动获取 ResourceLoader 单例
    _resourceLoader = ResourceLoader::getInstance();

    // 设置默认大小
    this->setContentSize(Size(50, 60));

    // 设置锚点（底部中心）
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

    // 根据类型设置属性
    switch (type)
    {
    case PlantType::SUNFLOWER:
        _canProduce = true;
        _produceInterval = 24.0f;
        _sunProduceAmount = 25;
        _idleAnimationName = "sunflower_idle";
        _produceAnimationName = "sunflower_produce";
        break;

    case PlantType::PEASHOOTER:
        _canAttack = true;
        _attackRange = 500.0f;
        _attackSpeed = 1.5f;
        _attackDamage = 20;
        _idleAnimationName = "peashooter_idle";
        _attackAnimationName = "peashooter_attack";
        break;

    case PlantType::WALLNUT:
        _health = 400;
        _maxHealth = 400;
        _idleAnimationName = "wallnut_idle";
        _damagedAnimationName = "wallnut_damaged1";
        break;

    default:
        break;
    }

    // 直接尝试播放初始动画（不依赖resourceLoader）
    if (!_idleAnimationName.empty())
    {
        log("Plant::initPlant: Attempting to play idle animation: %s",
            _idleAnimationName.c_str());

        // 尝试从ResourceLoader获取动画（如果有）
        if (_resourceLoader)
        {
            auto animation = _resourceLoader->getCachedAnimation(_idleAnimationName);
            if (animation)
            {
                log("Plant::initPlant: Found animation in ResourceLoader cache");
                auto animate = Animate::create(animation);
                auto repeat = RepeatForever::create(animate);
                this->runAction(repeat);
                _currentAnimate = animate;
            }
            else
            {
                log("Plant::initPlant: Animation not found in cache: %s",
                    _idleAnimationName.c_str());
            }
        }
        else
        {
            log("Plant::initPlant: No ResourceLoader set, using fallback animation");
            // 使用简单的颜色变化作为备用动画
            Plant::setupFallbackAnimation();
        }
    }

    log("Plant: Initialized plant type %d, health: %d", (int)type, _health);
    return true;
}

// 添加备用动画方法
void Plant::setupFallbackAnimation()
{
    switch (_type)
    {
    case PlantType::SUNFLOWER:
    {
        // 向日葵的备用动画：轻微的缩放和颜色变化
        auto scaleAction = RepeatForever::create(
            Sequence::create(
                ScaleTo::create(1.0f, 1.05f),
                ScaleTo::create(1.0f, 1.0f),
                nullptr
            )
        );
        this->runAction(scaleAction);

        auto colorAction = RepeatForever::create(
            Sequence::create(
                TintTo::create(1.0f, Color3B(255, 255, 200)),
                TintTo::create(1.0f, Color3B(255, 255, 0)),
                nullptr
            )
        );
        this->runAction(colorAction);
        break;
    }

    case PlantType::PEASHOOTER:
    {
        // 豌豆射手的备用动画：颜色变化
        auto idleAction = RepeatForever::create(
            Sequence::create(
                TintTo::create(0.5f, Color3B(0, 200, 0)),
                TintTo::create(0.5f, Color3B(0, 150, 0)),
                nullptr
            )
        );
        this->runAction(idleAction);
        break;
    }

    case PlantType::WALLNUT:
    {
        // 坚果墙的备用动画：轻微晃动
        auto shakeAction = RepeatForever::create(
            Sequence::create(
                RotateTo::create(0.5f, 1.0f),
                RotateTo::create(0.5f, -1.0f),
                nullptr
            )
        );
        this->runAction(shakeAction);
        break;
    }

    default:
        break;
    }
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

        // 攻击型植物尝试攻击
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
        // 短暂受伤状态后返回IDLE
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

    // 播放受伤动画
    if (!_damagedAnimationName.empty())
    {
        playAnimation(_damagedAnimationName, false);
    }
    else
    {
        // 默认闪烁效果
        auto tintAction = TintTo::create(0.1f, Color3B::RED);
        auto reverseAction = TintTo::create(0.1f, this->getColor());
        this->runAction(Sequence::create(tintAction, reverseAction, nullptr));
    }

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
    // 基类为空实现，子类重写
    log("Plant: Attack!");
}

void Plant::produceSun()
{
    // 基类为空实现，子类重写
    log("Plant: Produced sun!");
}

void Plant::die()
{
    _state = PlantState::DYING;

    // 停止所有动画
    stopCurrentAnimation();
    this->stopAllActions();

    // 播放死亡动画
    if (!_dyingAnimationName.empty())
    {
        playAnimation(_dyingAnimationName, false, [this]() {
            onDyingComplete();
            });
    }
    else
    {
        // 默认淡出效果
        auto fadeOut = FadeOut::create(0.5f);
        auto remove = RemoveSelf::create();

        this->runAction(Sequence::create(
            fadeOut,
            CallFunc::create([this]() {
                onDyingComplete();
                }),
            remove,
                    nullptr
                    ));
    }

    log("Plant: Died");
}

void Plant::setResourceLoader(ResourceLoader* loader)
{
    _resourceLoader = loader;
}

void Plant::setAnimationHelper(AnimationHelper* helper)
{
    _animationHelper = helper;
}

// 在Plant.cpp的playAnimation函数中修改
#include "./Utils/AnimationHelper.h"  // 添加这行

void Plant::playAnimation(const std::string& animationName, bool loop,
    const std::function<void()>& callback)
{
    CCLOG("Plant::playAnimation: Attempting to play animation: %s, loop: %d", 
        animationName.c_str(), loop);
    
    // 停止当前动画
    stopCurrentAnimation();
    
    // 首先尝试从ResourceLoader获取动画
    if (_resourceLoader)
    {
        auto animation = _resourceLoader->getCachedAnimation(animationName);
        if (animation)
        {
            CCLOG("Plant::playAnimation: Found animation in ResourceLoader cache");
            
            // 创建Animate动作
            Animate* animate = Animate::create(animation);
            if (!animate)
            {
                CCLOG("Plant::playAnimation: Failed to create Animate from animation");
                return;
            }
            
            _currentAnimate = animate;
            
            if (loop)
            {
                auto repeat = RepeatForever::create(animate);
                this->runAction(repeat);
                CCLOG("Plant::playAnimation: Playing animation in loop mode");
            }
            else
            {
                if (callback)
                {
                    auto sequence = Sequence::create(animate, CallFunc::create(callback), nullptr);
                    this->runAction(sequence);
                }
                else
                {
                    this->runAction(animate);
                }
                CCLOG("Plant::playAnimation: Playing animation once");
            }
            return;
        }
        else
        {
            CCLOG("Plant::playAnimation: Animation not found in ResourceLoader cache: %s", 
                animationName.c_str());
        }
    }
    else
    {
        CCLOG("Plant::playAnimation: No ResourceLoader available");
    }
    
    // 如果没有ResourceLoader或动画不在缓存中，使用备用动画
    CCLOG("Plant::playAnimation: Using fallback animation for %s", animationName.c_str());
    setupFallbackAnimation();
}

// 添加一个检查资源的方法
void Plant::checkAndPlayIdleAnimation()
{
    if (!_idleAnimationName.empty())
    {
        playAnimation(_idleAnimationName, true);
    }
}

void Plant::stopCurrentAnimation()
{
    if (_currentAnimate)
    {
        this->stopAction(_currentAnimate);
        _currentAnimate = nullptr;
    }
}

void Plant::onDyingComplete()
{
    this->removeFromParent();
    _state = PlantState::DEAD;
    onDead();
}