#include "Zombie.h"

bool Zombie::init() {
    if (!Sprite::init()) return false;
    _health = 200; // Default
    _speed = 50.0f;
    return true;
}

int Zombie::getHealth() {
    return _health;
}

void Zombie::takeDamage(int damage) {
    _health -= damage;
    if (_health <= 0) {
        this->removeFromParent();
    }
}

void Zombie::walkLeft() {
    auto move = MoveBy::create(1.0f, Vec2(-_speed, 0));
    this->runAction(RepeatForever::create(move));
}