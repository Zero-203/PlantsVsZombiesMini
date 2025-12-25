#include "NormalZombie.h"
#include "./Entities/Plants/Plant.h"
#include "./Game/GridSystem.h"

NormalZombie* NormalZombie::create() {
    auto ret = new NormalZombie();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool NormalZombie::init() {
    if (!Zombie::init()) return false;
    if (!initWithFile("Zombies/normal_zombie.png")) return false;
    _health = 200;
    _speed = 40.0f;
    this->scheduleUpdate();
    return true;
}

void NormalZombie::update(float dt) {
    // Check for plants in front
    auto grid = dynamic_cast<GridSystem*>(this->getParent()->getChildByName("grid")); // Assume named
    if (grid) {
        int row = (this->getPositionY() - GridSystem::GRID_HEIGHT / 2) / GridSystem::GRID_HEIGHT;
        int col = (this->getPositionX() - GridSystem::GRID_WIDTH / 2) / GridSystem::GRID_WIDTH;
        auto plant = grid->getPlantAt(row, col);
        if (plant) {
            this->stopAllActions();
            // Eat plant
            plant->takeDamage(10); // Per second
        }
        else {
            walkLeft();
        }
    }
}