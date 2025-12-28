#pragma once
#pragma once
#ifndef __SNOW_PEA_H__
#define __SNOW_PEA_H__

#include "Plant.h"
#include "./Entities/Projectile/Projectile.h"

class SnowPea : public Plant {
public:
    CREATE_FUNC(SnowPea);

    virtual bool init() override;

    // ¹¥»÷·½Ê½
    virtual void attack(float delta) override;
    virtual void onDead() override;

private:
    void shootIcePea();
};

#endif // __SNOW_PEA_H__