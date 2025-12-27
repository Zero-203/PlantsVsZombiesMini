#pragma once
#ifndef __ZOMBIE_NORMAL_H__
#define __ZOMBIE_NORMAL_H__

#include "Zombie.h"

class ZombieNormal : public Zombie {
public:
    static ZombieNormal* create();
    virtual bool init() override;


protected:
    virtual bool initWithType(ZombieType type) override;
    virtual std::string getWalkAnimationName() const override;
    virtual std::string getAttackAnimationName() const override;
    virtual std::string getDeathAnimationName() const override;
};

#endif // __ZOMBIE_NORMAL_H__