#ifndef __ZOMBIE_H__
#define __ZOMBIE_H__

#include "cocos2d.h"
USING_NS_CC;

class Zombie : public Sprite {
public:
    virtual bool init();
    int getHealth();
    void takeDamage(int damage);
    void walkLeft();
    virtual void update(float dt) = 0;
protected:
    int _health;
    float _speed;
};

#endif // __ZOMBIE_H__