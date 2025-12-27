#pragma once
#ifndef __SUNFLOWER_H__
#define __SUNFLOWER_H__

#include "Plant.h"

class Sunflower : public Plant {
public:
    CREATE_FUNC(Sunflower);

    virtual bool init() override;

    // 生产阳光方法
    virtual void produceSun() override;
    virtual void onDead() override;

private:
    void createSun();
};

#endif // __SUNFLOWER_H__