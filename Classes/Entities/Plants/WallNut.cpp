#include "WallNut.h"

USING_NS_CC;

bool WallNut::init()
{
    if (!Plant::init())
    {
        return false;
    }

    // 初始化坚果墙属性
    initPlant(PlantType::WALLNUT, 50, 30.0f, 400); // 高生命值，长冷却

    // 设置坚果墙特有属性
    _crackLevel = 0;

    // 设置颜色和大小（棕色）
    this->setColor(Color3B(139, 69, 19));
    this->setContentSize(Size(60, 70));

    // 添加轻微晃动动画（表现坚固）
    auto shakeAction = RepeatForever::create(
        Sequence::create(
            RotateTo::create(0.5f, 1.0f),
            RotateTo::create(0.5f, -1.0f),
            nullptr
        )
    );
    this->runAction(shakeAction);

    return true;
}

void WallNut::takeDamage(int damage)
{
    Plant::takeDamage(damage);
    updateCrackLevel();
}

void WallNut::updateCrackLevel()
{
    // 根据生命值百分比更新裂缝等级
    float healthPercent = (float)_health / (float)_maxHealth;

    int newCrackLevel = 0;
    if (healthPercent < 0.33f)
    {
        newCrackLevel = 2; // 严重裂缝
    }
    else if (healthPercent < 0.66f)
    {
        newCrackLevel = 1; // 轻微裂缝
    }

    if (newCrackLevel != _crackLevel)
    {
        _crackLevel = newCrackLevel;

        // 根据裂缝等级改变颜色
        if (_crackLevel == 1)
        {
            this->setColor(Color3B(160, 80, 30)); // 变浅一点
        }
        else if (_crackLevel == 2)
        {
            this->setColor(Color3B(180, 100, 40)); // 更浅

            // 添加闪烁效果表示严重损坏
            auto blinkAction = RepeatForever::create(
                Sequence::create(
                    FadeTo::create(0.2f, 150),
                    FadeTo::create(0.2f, 255),
                    nullptr
                )
            );
            this->runAction(blinkAction);
        }

        log("WallNut: Crack level updated to %d", _crackLevel);
    }
}