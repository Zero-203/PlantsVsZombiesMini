#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "GridSystem.h"
#include <UI/PlantCard.h>
#include <Entities/Plants/Plant.h>

class GameScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    virtual void update(float delta) override;

    CREATE_FUNC(GameScene);

private:
    // UI元素
    cocos2d::ui::Button* _pauseButton;
    cocos2d::Label* _sunLabel;
    cocos2d::Label* _levelLabel;

    // 植物卡牌
    std::vector<PlantCard*> _plantCards;
    PlantType _selectedPlantType;
    bool _hasSelectedPlant;

    // 临时植物预览
    cocos2d::Sprite* _plantPreview;

    // 游戏元素
    std::vector<Plant*> _plants;

    // 初始化方法
    void initUI();
    void initGrid();
    void initPlantCards();
    void initTouchHandlers();

    // 游戏逻辑
    void updateSunDisplay();
    void updatePlantCards();

    // 事件处理
    void onPlantCardSelected(PlantType plantType);
    void onGridClicked(int row, int col, const cocos2d::Vec2& worldPos);
    void onPauseButtonClicked(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    void onBackButtonClicked(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

    // 植物操作
    void placePlant(PlantType plantType, int row, int col);
    void showPlantPreview(PlantType plantType, const cocos2d::Vec2& position);
    void hidePlantPreview();
    void updatePlantPreviewPosition(const cocos2d::Vec2& position);

    // 阳光操作
    void addSun(int amount);
    void spendSun(int amount);

    // 游戏控制
    void pauseGame();
    void resumeGame();
    void showPauseMenu();
    void hidePauseMenu();
    void restartGame();
    void exitToMenu();
};

#endif // __GAME_SCENE_H__