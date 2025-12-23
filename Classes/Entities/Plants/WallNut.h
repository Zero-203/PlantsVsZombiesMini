#pragma once
#ifndef __WALLNUT_H__
#define __WALLNUT_H__

#include "Plant.h"

class WallNut : public Plant {
public:
    CREATE_FUNC(WallNut);

    virtual bool init() override;

    // 受伤时的特殊处理
    virtual void takeDamage(int damage) override;

private:
    void updateCrackLevel();
    int _crackLevel; // 裂缝等级（0-2）
};

#endif // __WALLNUT_H__