#pragma once
#pragma once
#ifndef __CHERRY_BOMB_H__
#define __CHERRY_BOMB_H__

#include "./Entities/Plants/Plant.h"

class CherryBomb : public Plant {
public:
    CREATE_FUNC(CherryBomb);

    virtual bool init() override;

    // 特殊方法
    void explode();
    void createExplosionEffect();
    float getExplosionDelay() const { return _explosionDelay; }
    float getExplosionRange() const { return _explosionRange; }
    int getExplosionDamage() const { return _explosionDamage; }

protected:
    // 爆炸相关属性
    float _explosionDelay;      // 爆炸延迟时间
    float _explosionRange;      // 爆炸范围
    int _explosionDamage;       // 爆炸伤害
    bool _isExploding;          // 是否正在爆炸
    float _explosionTimer;      // 爆炸计时器

    // 覆盖基类方法
    virtual void onIdle(float delta) override;
    //virtual void onExploding(float delta);
    virtual void onDead() override;

private:
    void playExplosionSound();
    void onExplodingComplete();
};

#endif // __CHERRY_BOMB_H__