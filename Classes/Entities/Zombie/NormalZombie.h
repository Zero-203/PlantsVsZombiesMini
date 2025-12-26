#ifndef __NORMALZOMBIE_H__
#define __NORMALZOMBIE_H__

#include "Zombie.h"

class NormalZombie : public Zombie {
public:
    static NormalZombie* create();
    bool init() override;
    void update(float dt) override;
};

#endif // __NORMALZOMBIE_H__