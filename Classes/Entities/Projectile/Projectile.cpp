#include "Projectile.h"
#include "./Resources/ResourceLoader.h"
#include "./Entities/Zombie/Zombie.h"

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

bool Projectile::initProjectile(ProjectileType type, int damage, float speed)
{
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
        break;

    case ProjectileState::HIT:
        // 短暂延迟后销毁
        this->scheduleOnce([this](float dt) {
            destroy();
            }, 0.1f, "delayed_destroy");
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
    // 空实现，因为我们当前不需要爆炸功能
}

void Projectile::setupAnimations()
{
    // 空实现，动画已经在 initProjectile 中设置好了
}