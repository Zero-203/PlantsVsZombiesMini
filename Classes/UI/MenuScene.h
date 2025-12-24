#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class MenuScene : public cocos2d::Scene
{
public:
    // 创建场景的静态方法
    static cocos2d::Scene* createScene();

    // 初始化
    virtual bool init() override;

    // 更新函数
    void update(float delta) override;

    // 创建方法
    CREATE_FUNC(MenuScene);

private:
    // UI元素
    cocos2d::ui::Button* _startButton;
    cocos2d::ui::Button* _settingsButton;
    cocos2d::ui::Button* _exitButton;

    cocos2d::Sprite* _background;
    cocos2d::Sprite* _logo;

    // 初始化UI
    void initUI();

    // 按钮回调
    void onStartButtonClicked(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    void onSettingsButtonClicked(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    void onExitButtonClicked(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

    // 动画效果
    void playButtonAnimation(cocos2d::ui::Button* button);

    // 资源加载
    void preloadResources();
};

#endif // MENU_SCENE_H