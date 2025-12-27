// [file name]: SettingsLayer.h
#ifndef SETTINGS_LAYER_H
#define SETTINGS_LAYER_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "./Resources/AudioManager.h"

USING_NS_CC;

class SettingsLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(SettingsLayer);

    virtual bool init() override;
    void show();
    void hide();

private:
    cocos2d::LayerColor* _background;
    cocos2d::ui::Button* _closeButton;

    // 音量控制
    cocos2d::ui::Slider* _bgmSlider;
    cocos2d::ui::Slider* _sfxSlider;
    cocos2d::Label* _bgmLabel;
    cocos2d::Label* _sfxLabel;
    cocos2d::ui::CheckBox* _muteCheckBox;

    // 当前音量值显示
    cocos2d::Label* _bgmValueLabel;
    cocos2d::Label* _sfxValueLabel;

    // 触摸事件监听器
    cocos2d::EventListenerTouchOneByOne* _touchListener;
    bool _isListenerAdded;  // 标记监听器是否已添加

    void initUI();
    void onCloseButtonClicked(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    void onBgmSliderChanged(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type);
    void onSfxSliderChanged(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type);
    void onMuteCheckBoxChanged(cocos2d::Ref* sender, cocos2d::ui::CheckBox::EventType type);
    void updateVolumeLabels();

    // 滑动条渐变效果
    void showSliderChangeEffect(cocos2d::Node* target, float fromScale, float toScale);

    // 添加/移除触摸监听器
    void addTouchListener();
    void removeTouchListener();
};

#endif // SETTINGS_LAYER_H