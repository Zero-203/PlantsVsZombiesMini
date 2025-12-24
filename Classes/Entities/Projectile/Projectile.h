#pragma once
#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "cocos2d.h"
#include <string>

// 子弹类型枚举
enum class ProjectileType {
    PEA,            // 豌豆子弹
    SNOW_PEA,       // 寒冰豌豆子弹
    FIRE_PEA,       // 火焰豌豆子弹
    CABBAGE,        // 卷心菜子弹
    MELON,          // 西瓜子弹
    SPIKEWEED       // 尖刺子弹
};

// 子弹状态
enum class ProjectileState {
    FLYING,         // 飞行中
    HIT,            // 击中目标
    EXPLODING,      // 爆炸中
    DEAD            // 已销毁
};

class Projectile : public cocos2d::Sprite {
public:
    CREATE_FUNC(Projectile);

    virtual bool init() override;
    virtual void update(float delta) override;

    // 初始化子弹
    virtual bool initProjectile(ProjectileType type, int damage, float speed);

    // 子弹属性
    ProjectileType getType() const { return _type; }
    int getDamage() const { return _damage; }
    float getSpeed() const { return _speed; }
    ProjectileState getState() const { return _state; }
    bool isAlive() const { return _state != ProjectileState::DEAD; }

    // 设置发射参数
    void setLaunchParams(const cocos2d::Vec2& startPos, const cocos2d::Vec2& direction);

    // 子弹行为
    virtual void fly(float delta);
    virtual void hitTarget();
    virtual void explode();
    virtual void destroy();

    // 碰撞检测
    virtual bool checkCollision(cocos2d::Rect targetRect);

    // 特效
    void addTrailEffect(); // 添加拖尾效果
    void addHitEffect();   // 添加击中效果

protected:
    // 子类可重写的函数
    virtual void onFlying(float delta) {}
    virtual void onHit() {}
    virtual void onExplode() {}
    virtual void onDestroy() {}

    // 动画相关
    virtual void setupAnimations();
    bool isReadyForRemoval() const {
        return _state == ProjectileState::DEAD || !this->getParent();
    }

protected:
    ProjectileType _type;          // 子弹类型
    ProjectileState _state;        // 子弹状态

    // 基础属性
    int _damage;                   // 伤害值
    float _speed;                  // 飞行速度
    float _lifeTime;               // 生存时间
    float _maxLifeTime;            // 最大生存时间

    // 物理属性
    cocos2d::Vec2 _direction;      // 飞行方向
    cocos2d::Vec2 _velocity;       // 速度向量
    float _rotationSpeed;          // 旋转速度

    // 特效属性
    bool _hasTrailEffect;          // 是否有拖尾效果
    bool _hasHitEffect;            // 是否有击中效果
    float _effectTimer;            // 特效计时器

    // 动画名称
    std::string _flyAnimationName;    // 飞行动画
    std::string _hitAnimationName;    // 击中动画
    std::string _explodeAnimationName; // 爆炸动画

    // 资源引用
    class ResourceLoader* _resourceLoader;
    class AnimationHelper* _animationHelper;

    // 动画动作
    cocos2d::Animate* _currentAnimate;

public:
    // 设置资源加载器
    void setResourceLoader(class ResourceLoader* loader);

    // 设置动画辅助器
    void setAnimationHelper(class AnimationHelper* helper);

    // 播放动画
    void playAnimation(const std::string& animationName, bool loop = false,
        const std::function<void()>& callback = nullptr);

    // 停止当前动画
    void stopCurrentAnimation();
};

#endif // __PROJECTILE_H__