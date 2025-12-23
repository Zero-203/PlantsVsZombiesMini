#pragma once
// Zombie.h - 僵尸基类
#include "Entities.h"
#include "Plant.h"

class Zombie : public Entity {
public:
    enum class ZombieType {
        NORMAL,
        CONE,
        BUCKET
    };

    virtual bool init() override;
    virtual void update(float delta) override;

    // 僵尸特有属性
    CC_SYNTHESIZE(float, _moveSpeed, MoveSpeed);
    CC_SYNTHESIZE(ZombieType, _type, Type);
    CC_SYNTHESIZE(int, _lane, Lane); // 所在行

    // 僵尸行为
    virtual void move(float delta);
    virtual void eatPlant(Plant* plant);
};