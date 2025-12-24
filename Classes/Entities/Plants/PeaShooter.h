#pragma once
#ifndef __PEASHOOTER_H__
#define __PEASHOOTER_H__

#include "Plant.h"
#include "./Entities/Projectile/Projectile.h"

class Peashooter : public Plant {
public:
    CREATE_FUNC(Peashooter);

    virtual bool init() override;

    // ¹¥»÷·½·¨
    virtual void attack(float delta) override;

private:
    void shootPea();
};

#endif // __PEASHOOTER_H__