#pragma once
#ifndef __PLANT_FACTORY_H__
#define __PLANT_FACTORY_H__

#include "cocos2d.h"
#include "Plant.h"
#include "Peashooter.h"
#include "Sunflower.h"
#include "WallNut.h"
#include "CherryBomb.h"
#include "SnowPea.h"
#include "PotatoMine.h"

class PlantFactory {
public:
    static Plant* createPlant(PlantType type) {
        Plant* plant = nullptr;

        switch (type) {
        case PlantType::SUNFLOWER:
            plant = Sunflower::create();
            break;
        case PlantType::PEASHOOTER:
            plant = Peashooter::create();
            break;
        case PlantType::WALLNUT:
            plant = WallNut::create();
            break;
        case PlantType::CHERRY_BOMB:
            plant = CherryBomb::create();
            break;
        case PlantType::SNOW_PEA:
            plant = SnowPea::create();
            break;
        case PlantType::POTATO_MINE:
            plant = PotatoMine::create();
            break;
        default:
            CCLOG("PlantFactory: Unknown plant type");
            return nullptr;
        }

        if (plant) {
            plant->initPlant(type, getSunCost(type), getCooldown(type), getHealth(type));
        }

        return plant;
    }

    static int getSunCost(PlantType type) {
        switch (type) {
        case PlantType::SUNFLOWER: return 50;
        case PlantType::PEASHOOTER: return 100;
        case PlantType::WALLNUT: return 50;
        case PlantType::CHERRY_BOMB: return 150;
        case PlantType::SNOW_PEA: return 150;
        case PlantType::POTATO_MINE: return 25;
        default: return 0;
        }
    }

    static float getCooldown(PlantType type) {
        switch (type) {
        case PlantType::SUNFLOWER: return 7.5f;
        case PlantType::PEASHOOTER: return 7.5f;
        case PlantType::WALLNUT: return 15.0f;
        case PlantType::CHERRY_BOMB: return 22.5f;
        case PlantType::SNOW_PEA: return 7.5f;
        case PlantType::POTATO_MINE: return 15.0f;
        default: return 0.0f;
        }
    }

    static int getHealth(PlantType type) {
        switch (type) {
        case PlantType::SUNFLOWER: return 100;
        case PlantType::PEASHOOTER: return 100;
        case PlantType::WALLNUT: return 600;
        case PlantType::CHERRY_BOMB: return 600;
        case PlantType::SNOW_PEA: return 100;
        case PlantType::POTATO_MINE: return 100;
        default: return 0;
        }
    }

    static std::string getPlantName(PlantType type) {
        switch (type) {
        case PlantType::SUNFLOWER: return "Sunflower";
        case PlantType::PEASHOOTER: return "Peashooter";
        case PlantType::WALLNUT: return "Wall-nut";
        case PlantType::CHERRY_BOMB: return "Cherry Bomb";
        case PlantType::SNOW_PEA: return "Snow Pea";
        case PlantType::POTATO_MINE: return "Potato Mine";
        default: return "Unknown";
        }
    }
};

#endif // __PLANT_FACTORY_H__