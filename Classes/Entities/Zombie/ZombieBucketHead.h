#pragma once
#ifndef __ZOMBIE_BUCKET_HEAD_H__
#define __ZOMBIE_BUCKET_HEAD_H__

#include "./Entities/Zombie/Zombie.h"

class ZombieBucketHead : public Zombie {
public:
    static ZombieBucketHead* create();
    virtual bool init() override;
    virtual void takeDamage(int damage) override;

protected:
    virtual bool initWithType(ZombieType type) override;
    virtual std::string getWalkAnimationName() const override;
    virtual std::string getAttackAnimationName() const override;
    virtual std::string getDeathAnimationName() const override;

private:
    bool _bucketDestroyed; // 铁桶是否已被破坏
    int _bucketHealth;     // 铁桶生命值
};

#endif // __ZOMBIE_BUCKET_HEAD_H__