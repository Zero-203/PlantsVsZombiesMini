#pragma once
#ifndef __ZOMBIE_CONE_HEAD_H__
#define __ZOMBIE_CONE_HEAD_H__

#include "./Entities/Zombie/Zombie.h"

class ZombieConeHead : public Zombie {
public:
    static ZombieConeHead* create();
    virtual bool init() override;
    virtual void takeDamage(int damage) override;

protected:
    virtual bool initWithType(ZombieType type) override;
    virtual std::string getWalkAnimationName() const override;
    virtual std::string getAttackAnimationName() const override;
    virtual std::string getDeathAnimationName() const override;

private:
    bool _coneDestroyed; // 路障是否已被破坏
    int _coneHealth;     // 路障生命值
};

#endif // __ZOMBIE_CONE_HEAD_H__