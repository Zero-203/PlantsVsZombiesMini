#pragma once
#ifndef __ZOMBIE_FACTORY_H__
#define __ZOMBIE_FACTORY_H__

#include "./Entities/Zombie/Zombie.h"
#include "./Entities/Zombie/ZombieNo mal.h"
#include "./Entities/Zombie/ZombieConeHead.h"
#include "./Entities/Zombie/ZombieConeHead.h"

class ZombieFactory {
public:
    static Zombie* createZombie(ZombieType type) {
        switch (type) {
            case ZombieType::NORMAL:
                return ZombieNormal::create();
            case ZombieType::CONEHEAD:
                return ZombieConeHead::create();
            case ZombieType::BUCKETHEAD:
                return ZombieBucketHead::create();
            default:
                return ZombieNormal::create();
        }
    }

    static Zombie* createRandomZombie(int waveNumber) {
        int random = rand() % 100;

        if (waveNumber <= 2) {
            // 前2波只有普通僵尸
            return createZombie(ZombieType::NORMAL);
        }
        else if (waveNumber <= 5) {
            // 3-5波有普通和路障僵尸
            if (random < 80) return createZombie(ZombieType::NORMAL);
            else return createZombie(ZombieType::CONEHEAD);
        }
        else {
            // 6波之后有所有类型
            if (random < 60) return createZombie(ZombieType::NORMAL);
            else if (random < 85) return createZombie(ZombieType::CONEHEAD);
            else return createZombie(ZombieType::BUCKETHEAD);
        }
    }
};

#endif // __ZOMBIE_FACTORY_H__