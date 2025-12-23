#ifndef __PLANT_CARD_H__
#define __PLANT_CARD_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Entities/Plants/Plant.h"

class PlantCard : public cocos2d::ui::Button {
public:
    // 创建植物卡牌
    static PlantCard* create(PlantType plantType);

    // 初始化
    virtual bool init(PlantType plantType);

    // 更新状态
    void updateCardState(int currentSun);

    // 获取植物类型
    PlantType getPlantType() const { return _plantType; }

    // 获取阳光消耗
    int getSunCost() const { return _sunCost; }

    // 获取冷却时间
    float getCooldown() const { return _cooldown; }

    // 检查是否可用
    bool isAvailable() const { return _isAvailable; }

    // 检查是否冷却中
    bool isCoolingDown() const { return _isCoolingDown; }

    // 开始冷却
    void startCooldown();

    // 重置冷却
    void resetCooldown();

private:
    PlantType _plantType;
    int _sunCost;
    float _cooldown;
    float _cooldownTimer;
    bool _isAvailable;
    bool _isCoolingDown;

    // UI元素
    cocos2d::Label* _sunCostLabel;
    cocos2d::DrawNode* _cooldownOverlay;
    cocos2d::ProgressTimer* _cooldownProgress;

    // 更新冷却
    void updateCoolingDown(float delta);
};

#endif // __PLANT_CARD_H__