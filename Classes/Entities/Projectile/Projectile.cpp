#include "Projectile.h"
#include "./Resources/ResourceLoader.h"
#include "./Entities/Zombie/Zombie.h"
#include <Game/WaveManager.h>

//#include "./Utils/AnimationHelper.h"

USING_NS_CC;

bool Projectile::init()
{
    if (!Sprite::init())
    {
        return false;
    }

    // 初始化默认值
    _type = ProjectileType::PEA;
    _state = ProjectileState::FLYING;

    _damage = 0;
    _speed = 0.0f;
    _lifeTime = 0.0f;
    _maxLifeTime = 5.0f;

    _direction = Vec2::ZERO;
    _velocity = Vec2::ZERO;
    _rotationSpeed = 0.0f;

    _hasTrailEffect = false;
    _hasHitEffect = false;
    _effectTimer = 0.0f;

    _resourceLoader = ResourceLoader::getInstance();
    _currentAnimate = nullptr;

    // 设置默认锚点
    this->setAnchorPoint(Vec2(0.5f, 0.5f));

    // 启用更新
    this->scheduleUpdate();

    return true;
}

bool Projectile::initProjectile(ProjectileType type, int damage, float speed,
    DamageType damageType) {
    _type = type;
    _damage = damage;
    _speed = speed;

    // 根据类型设置属性
    switch (type)
    {
    case ProjectileType::PEA:
        _flyAnimationName = "pea_fly";
        _hitAnimationName = "pea_hit";
        _maxLifeTime = 4.0f;
        break;
    case ProjectileType::SNOW_PEA:
        _flyAnimationName = "snow_pea_fly";
        _hitAnimationName = "pea_hit";
        _maxLifeTime = 4.0f;
        break;
    default:
        break;
    }

    return true;
}

void Projectile::update(float delta)
{
    if (!isAlive())
    {
        return;
    }

    _lifeTime += delta;

    // 检查生存时间
    if (_lifeTime >= _maxLifeTime)
    {
        destroy();
        return;
    }

    // 根据状态更新
    switch (_state)
    {
    case ProjectileState::FLYING:
        fly(delta);
        checkZombieCollisions();
        break;

    case ProjectileState::HIT:
        // 短暂延迟后销毁
        this->scheduleOnce([this](float dt) {
            destroy();
            }, 0.05f, "delayed_destroy");
        break;

    default:
        break;
    }
}

void Projectile::checkZombieCollisions() {
    if (_state != ProjectileState::FLYING) {
        return;
    }

    auto waveManager = WaveManager::getInstance();
    if (!waveManager) return;

    const auto& zombies = waveManager->getActiveZombies();
    Rect projectileRect = this->getBoundingBox();

    bool hitSomething = false;

    for (auto zombie : zombies) {
        if (!zombie || !zombie->isAlive()) continue;

        Rect zombieRect = zombie->getBoundingBox();

        if (projectileRect.intersectsRect(zombieRect)) {
            hitSomething = true;

            // 对僵尸造成伤害
            applyDamageToZombie(zombie);

            // 如果是单体伤害子弹，只攻击一个目标
            if (_damageType == DamageType::SINGLE_TARGET) {
                hitTarget();
                break;
            }
        }
    }

    // 如果是范围伤害，击中一个目标后继续飞行直到消失或超时
    if (hitSomething && _damageType == DamageType::AREA_OF_EFFECT) {
        // 范围伤害击中后可以继续飞行（可选）
        // 或者直接销毁
        // destroy();
    }
}

void Projectile::applyDamageToZombie(Zombie* zombie) {
    if (!zombie || !zombie->isAlive()) return;

    bool dead = zombie->getHealth() - _damage <= 0;
    // 基础伤害
    zombie->takeDamage(_damage);

    if (dead)
        return;
    // 根据子弹类型添加特效
    switch (_type) {
    case ProjectileType::SNOW_PEA:
        zombie->freeze(2.5f);  // 冰冻2.5秒
        break;
    case ProjectileType::FIRE_PEA:
        // 火焰持续伤害效果
        zombie->setColor(Color3B::RED);
        // 可以添加火焰特效标记
        break;
    default:
        break;
    }
}

void Projectile::setLaunchParams(const cocos2d::Vec2& startPos, const cocos2d::Vec2& direction)
{
    this->setPosition(startPos);
    _direction = direction.getNormalized();
    _velocity = _direction * _speed;

    // 设置旋转
    float angle = CC_RADIANS_TO_DEGREES(_direction.getAngle());
    this->setRotation(-angle);

    // 确保子弹可见
    this->setVisible(true);
    this->setOpacity(255);

    // 播放飞行动画 - 使用更可靠的方法
    if (!_flyAnimationName.empty())
    {
        // 尝试多种方法播放动画
        bool animationPlayed = false;
        
        //方法1：直接使用ResourceLoader
        if (_resourceLoader)
        {
            auto animation = _resourceLoader->getCachedAnimation(_flyAnimationName);
            if (animation)
            {
                auto animate = Animate::create(animation);
                if (animate)
                {
                    _currentAnimate = animate;
                    auto repeat = RepeatForever::create(animate);
                    this->runAction(repeat);
                    animationPlayed = true;
                    log("Projectile: Animation played via ResourceLoader");
                }
            }
        }

        // 方法2：如果所有方法都失败，设置备用外观
        if (!animationPlayed)
        {
            log("WARNING: Failed to play animation, using fallback appearance");

            // 设置一个简单的豌豆形状作为后备
            this->setColor(Color3B(0, 200, 0));
            this->setTextureRect(Rect(0, 0, 15, 8));
            this->setOpacity(200);

            // 添加简单的脉动动画
            auto pulseAction = RepeatForever::create(
                Sequence::create(
                    ScaleTo::create(0.3f, 1.2f, 1.0f),
                    ScaleTo::create(0.3f, 1.0f, 1.0f),
                    nullptr
                )
            );
            this->runAction(pulseAction);
        }
    }
    else
    {
        // 如果没有动画名称，直接设置备用外观
        this->setColor(Color3B(0, 200, 0));
        this->setTextureRect(Rect(0, 0, 15, 8));
        this->setOpacity(200);
    }
}

void Projectile::applySplashDamage(const Vec2& center) {
    auto waveManager = WaveManager::getInstance();
    if (!waveManager) return;

    const auto& zombies = waveManager->getActiveZombies();

    for (auto zombie : zombies) {
        if (!zombie || !zombie->isAlive()) continue;

        Vec2 zombiePos = zombie->getPosition();
        float distance = center.distance(zombiePos);

        if (distance <= _splashRadius) {
            // 根据距离计算伤害衰减
            float falloff = 1.0f - (distance / _splashRadius) * _damageFalloff;
            int splashDamage = _damage * falloff;

            if (splashDamage > 0) {
                zombie->takeDamage(splashDamage);

                // 如果是冰冻或火焰子弹，添加特效
                if (_type == ProjectileType::SNOW_PEA) {
                    zombie->freeze(2.5f * falloff);
                }
            }
        }
    }
}

void Projectile::fly(float delta)
{
    // 更新位置
    Vec2 newPos = this->getPosition() + _velocity * delta;
    this->setPosition(newPos);

    // 检查是否飞出屏幕
    Size visibleSize = Director::getInstance()->getVisibleSize();
    float margin = 100.0f;

    if (newPos.x > visibleSize.width + margin ||
        newPos.x < -margin ||
        newPos.y > visibleSize.height + margin ||
        newPos.y < -margin)
    {
        destroy();
    }
}

void Projectile::hitTarget()
{
    if (_state != ProjectileState::FLYING)
    {
        return;
    }

    _state = ProjectileState::HIT;

    this->fly(50.0f / _velocity.x);

    // 停止飞行动画
    stopCurrentAnimation();

    // 播放击中动画
    if (!_hitAnimationName.empty())
    {
        playAnimation(_hitAnimationName, false, [this]() {
            destroy();
            });
    }
    else
    {
        destroy();
    }
}

void Projectile::destroy()
{
    this->setVisible(false);
    if (_state == ProjectileState::DEAD)
    {
        return;
    }

    _state = ProjectileState::DEAD;

    // 停止所有动画和动作
    stopCurrentAnimation();
    this->stopAllActions();
    this->unscheduleUpdate();

    // 安全地从父节点移除
    this->scheduleOnce([this](float dt) {
        if (this->getParent())
        {
            this->removeFromParent();
        }
        }, 0.0f, "delayed_removal");

    onDestroy();
}

bool Projectile::checkCollision(cocos2d::Rect targetRect)
{
    if (_state != ProjectileState::FLYING)
    {
        return false;
    }

    // 简单矩形碰撞检测
    Rect projectileRect = this->getBoundingBox();
    return projectileRect.intersectsRect(targetRect);
}

void Projectile::playAnimation(const std::string& animationName, bool loop,
    const std::function<void()>& callback)
{

    // 停止当前动画
    stopCurrentAnimation();

    // 使用ResourceLoader获取动画
    if (_resourceLoader)
    {
        // 尝试多种方式获取动画
        Animation* animation = nullptr;

        // 1. 从缓存获取
        animation = _resourceLoader->getCachedAnimation(animationName);

        // 2. 从通用方法获取
        if (!animation)
        {
            animation = _resourceLoader->getAnimation(animationName);
        }

        if (animation)
        {
            _currentAnimate = Animate::create(animation);
            if (_currentAnimate)
            {
                // 如果有回调函数，创建序列动作
                if (callback)
                {
                    auto sequence = Sequence::create(_currentAnimate, CallFunc::create(callback), nullptr);
                    this->runAction(sequence);
                }
                else if (loop)
                {
                    auto repeat = RepeatForever::create(_currentAnimate);
                    this->runAction(repeat);
                }
                else
                {
                    this->runAction(_currentAnimate);
                }

                log("Projectile: Animation '%s' played successfully", animationName.c_str());
                return;
            }
        }
    }

}

void Projectile::stopCurrentAnimation()
{
    if (_currentAnimate)
    {
        this->stopAction(_currentAnimate);
        _currentAnimate = nullptr;
    }
}

void Projectile::addTrailEffect()
{
    // 空实现，保留以备后用
}

void Projectile::addHitEffect()
{
    // 空实现，保留以备后用
}

void Projectile::explode()
{
    // 空实现，保留以备后用
}

void Projectile::setupAnimations()
{
    // 空实现，动画已经在 initProjectile 中设置好了
}