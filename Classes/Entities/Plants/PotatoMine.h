#pragma once
#ifndef __POTATO_MINE_H__
#define __POTATO_MINE_H__

#include "./Entities/Plants/Plant.h"

class PotatoMine : public Plant {
public:
    CREATE_FUNC(PotatoMine);

    virtual bool init() override;

    // 地雷特有方法
    void arm();                    // 激活地雷
    void trigger();                // 触发爆炸
    bool checkZombieInTriggerRange();
    void createMineEffect();       // 创建地雷特效

    // 获取器
    float getArmingTime() const { return _armingTime; }
    float getExplosionRange() const { return _explosionRange; }
    int getExplosionDamage() const { return _explosionDamage; }
    bool isArmed() const { return _isArmed; }
    bool isTriggered() const { return _isTriggered; }

protected:
    // 重写虚函数
    virtual void onIdle(float delta) override;
    virtual void onDead() override;

private:
    // 地雷特有属性
    float _armingTime;          // 准备时间（秒）
    float _armingTimer;         // 准备计时器
    bool _isArmed;              // 是否已激活
    bool _isTriggered;          // 是否已触发

    float _triggerRange;        // 触发范围（僵尸进入此范围即触发）
    float _explosionRange;      // 爆炸范围
    int _explosionDamage;       // 爆炸伤害

    // 内部方法
    void playArmingSound();
    void playExplosionSound();
    void onExplosionComplete();

    // 触发检测
    bool checkZombieInRange();
};

#endif // __POTATO_MINE_H__