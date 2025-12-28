#pragma once
#ifndef __ZOMBIE_H__
#define __ZOMBIE_H__

#include "cocos2d.h"

class Plant;
class Projectile;

// 僵尸状态
enum class ZombieState {
    ALIVE,      // 存活
    ATTACKING,  // 攻击中
    DEAD,       // 死亡
    FROZEN      // 冰冻
};

// 僵尸类型
enum class ZombieType {
    NORMAL,     // 普通僵尸
    CONEHEAD,   // 路障僵尸
    BUCKETHEAD, // 铁桶僵尸
    FAST,       // 快速僵尸
    GARGANTUAR  // 巨人僵尸
};

class Zombie : public cocos2d::Sprite {
public:
    virtual bool init() override;
    virtual void update(float delta) override;

    // 工厂方法
    static Zombie* create(ZombieType type = ZombieType::NORMAL);

    // 属性获取
    ZombieType getType() const { return _type; }
    ZombieState getState() const { return _state; }
    int getRow() const { return _row; }
    int getHealth() const { return _health; }
    int getMaxHealth() const { return _maxHealth; }
    float getSpeed() const { return _speed; }
    int getDamage() const { return _damage; }
    bool isAlive() const { return _state != ZombieState::DEAD; }

    // 属性设置
    void setType(ZombieType type) { _type = type; }
    void setRow(int row) { _row = row; }
    void setHealth(int health) { _health = health; }
    void setSpeed(float speed) { _speed = speed; }
    void setDamage(int damage) { _damage = damage; }

    // 状态控制
    virtual void takeDamage(int damage);
    virtual void attack(Plant* plant);
    virtual void die();
    virtual void freeze(float duration);
    virtual void unFreeze();  // 新增：解除冰冻

    // 移动控制
    virtual void startMoving();
    virtual void stopMoving();

    // 动画控制
    virtual void playWalkAnimation();
    virtual void playAttackAnimation();
    virtual void playDeathAnimation();

    CREATE_FUNC(Zombie);

protected:
    Zombie();
    virtual ~Zombie();

    // 初始化特定僵尸类型
    virtual bool initWithType(ZombieType type);

    // 动画资源名称
    virtual std::string getWalkAnimationName() const;
    virtual std::string getAttackAnimationName() const;
    virtual std::string getDeathAnimationName() const;

    // 寻路和碰撞检测
    virtual void updateMovement(float delta);
    virtual void checkCollisions();
    virtual Plant* findPlantInFront();

protected:
    ZombieType _type;
    ZombieState _state;
    int _row;           // 所在行

    // 属性
    int _health;
    int _maxHealth;
    float _speed;       // 像素/秒
    int _damage;        // 每次攻击伤害
    float _originalSpeed;  // 存储原始速度
    bool _isFrozen;        // 是否处于冰冻状态

    // 状态计时器
    float _attackTimer;
    float _attackInterval;
    float _freezeTimer;

    // 目标植物
    Plant* _targetPlant;

    // 动画相关
    cocos2d::Animation* _walkAnimation;
    cocos2d::Animation* _attackAnimation;
    cocos2d::Animation* _deathAnimation;
};

#endif // __ZOMBIE_H__