#pragma once
// Plant.h - 植物基类
#include "Entities.h"
class Plant : public Entity {
public:
    enum class PlantType {
        ATTACK,     // 攻击型
        DEFENSE,    // 防御型
        RESOURCE    // 资源型
    };

    virtual bool init() override;

    // 植物特有属性
    CC_SYNTHESIZE(int, _sunCost, SunCost);
    CC_SYNTHESIZE(float, _cooldown, Cooldown);
    CC_SYNTHESIZE(float, _attackRange, AttackRange);
    CC_SYNTHESIZE(PlantType, _type, Type);
    CC_SYNTHESIZE(cocos2d::Vec2, _gridPosition, GridPosition);

    // 植物行为
    virtual void attack(float delta);
    virtual void takeDamage(int damage);
    virtual void produceSun(); // 资源型植物使用
};