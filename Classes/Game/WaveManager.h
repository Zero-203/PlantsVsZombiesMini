#ifndef __WAVEMANAGER_H__
#define __WAVEMANAGER_H__

#include "cocos2d.h"
#include "Zombie.h"
USING_NS_CC;

class WaveManager : public Node {
public:
    static WaveManager* create();
    bool init();
    void startWaves();
    void spawnZombie(Zombie* zombie, int row);
private:
    void spawnWave(int waveNumber);
    int _currentWave;
};

#endif // __WAVEMANAGER_H__