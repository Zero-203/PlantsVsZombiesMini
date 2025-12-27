// [file name]: SettingsLayer.cpp
#include "SettingsLayer.h"
#include "./Resources/AudioManager.h"

USING_NS_CC;
using namespace ui;

bool SettingsLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 初始状态
    _isListenerAdded = false;

    // 1. 半透明背景层
    _background = LayerColor::create(Color4B(0, 0, 0, 180), visibleSize.width, visibleSize.height);
    _background->setPosition(origin);
    this->addChild(_background, 0);

    // 2. 设置面板背景
    auto drawNode = DrawNode::create();
    drawNode->drawSolidRect(Vec2(-220, -180), Vec2(220, 180), Color4F(0.2f, 0.2f, 0.3f, 0.95f));
    drawNode->drawRect(Vec2(-220, -180), Vec2(220, 180), Color4F(0.8f, 0.8f, 1.0f, 1.0f));
    drawNode->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(drawNode, 1);

    // 3. 标题
    auto titleLabel = Label::createWithTTF("音频设置", "fonts/Marker Felt.ttf", 36);
    if (!titleLabel) {
        titleLabel = Label::createWithSystemFont("音频设置", "Arial", 36);
    }
    titleLabel->setColor(Color3B::YELLOW);
    titleLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 120));
    this->addChild(titleLabel, 2);

    // 4. 关闭按钮
    _closeButton = Button::create();
    _closeButton->setTitleText("关闭");
    _closeButton->setTitleFontSize(24);
    _closeButton->setTitleColor(Color3B::WHITE);
    _closeButton->setContentSize(Size(80, 40));

    // 创建简单的按钮背景
    auto closeBtnDraw = DrawNode::create();
    closeBtnDraw->drawSolidRect(Vec2(-40, -20), Vec2(40, 20), Color4F(0.6f, 0.2f, 0.2f, 1.0f));
    closeBtnDraw->drawRect(Vec2(-40, -20), Vec2(40, 20), Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    _closeButton->addChild(closeBtnDraw, -1);

    _closeButton->setPosition(Vec2(visibleSize.width / 2 + 180, visibleSize.height / 2 + 120));
    _closeButton->addTouchEventListener(CC_CALLBACK_2(SettingsLayer::onCloseButtonClicked, this));
    this->addChild(_closeButton, 2);

    // 5. 初始化音量控制UI
    initUI();

    // 6. 创建触摸事件监听器（但不立即添加）
    _touchListener = EventListenerTouchOneByOne::create();
    _touchListener->setSwallowTouches(true);
    _touchListener->onTouchBegan = [](Touch* touch, Event* event) {
        return true; // 吞噬触摸事件
    };
    auto panelCenter = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    _touchListener->onTouchEnded = [this, panelCenter](Touch* touch, Event* event) {
        // 获取触摸位置
        auto location = touch->getLocation();

        // 检查是否点击在背景上（但不在面板内）
        auto panelRect = Rect(
            panelCenter.x - 220,
            panelCenter.y - 180,
            440,  // 面板宽度
            360   // 面板高度
        );

        if (!panelRect.containsPoint(location)) {
            // 点击背景外部，关闭设置界面
            this->hide();
        }
    };

    // 初始隐藏
    this->setVisible(false);

    return true;
}

void SettingsLayer::initUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // BGM音量控制
    _bgmLabel = Label::createWithTTF("背景音乐", "fonts/Marker Felt.ttf", 28);
    if (!_bgmLabel) {
        _bgmLabel = Label::createWithSystemFont("背景音乐", "Arial", 28);
    }
    _bgmLabel->setColor(Color3B::WHITE);
    _bgmLabel->setPosition(Vec2(visibleSize.width / 2 - 120, visibleSize.height / 2 + 50));
    this->addChild(_bgmLabel, 2);

    _bgmSlider = Slider::create();
    _bgmSlider->setScale9Enabled(true);
    _bgmSlider->setContentSize(Size(200, 20));

    // 创建滑块背景
    auto sliderTrack = DrawNode::create();
    sliderTrack->drawSolidRect(Vec2(-100, -10), Vec2(100, 10), Color4F(0.3f, 0.3f, 0.3f, 1.0f));
    _bgmSlider->addChild(sliderTrack, -1);

    // 创建滑块进度
    auto sliderProgress = DrawNode::create();
    sliderProgress->drawSolidRect(Vec2(-100, -10), Vec2(100, 10), Color4F(0.2f, 0.6f, 0.2f, 1.0f));
    _bgmSlider->addChild(sliderProgress, -2);

    // 创建滑块按钮
    auto sliderThumb = DrawNode::create();
    sliderThumb->drawSolidCircle(Vec2(0, 0), 15, 0, 20, Color4F(0.8f, 0.8f, 0.8f, 1.0f));
    _bgmSlider->addChild(sliderThumb, -1);

    _bgmSlider->setPosition(Vec2(visibleSize.width / 2 + 40, visibleSize.height / 2 + 50));
    _bgmSlider->setPercent(AudioManager::getInstance()->getBackgroundMusicVolume() * 100);
    _bgmSlider->addEventListener(CC_CALLBACK_2(SettingsLayer::onBgmSliderChanged, this));
    this->addChild(_bgmSlider, 2);

    _bgmValueLabel = Label::createWithTTF(
        StringUtils::format("%d%%", (int)(AudioManager::getInstance()->getBackgroundMusicVolume() * 100)),
        "fonts/Marker Felt.ttf", 24
    );
    if (!_bgmValueLabel) {
        _bgmValueLabel = Label::createWithSystemFont(
            StringUtils::format("%d%%", (int)(AudioManager::getInstance()->getBackgroundMusicVolume() * 100)),
            "Arial", 24
        );
    }
    _bgmValueLabel->setColor(Color3B::GREEN);
    _bgmValueLabel->setPosition(Vec2(visibleSize.width / 2 + 160, visibleSize.height / 2 + 50));
    this->addChild(_bgmValueLabel, 2);

    // 音效音量控制
    _sfxLabel = Label::createWithTTF("游戏音效", "fonts/Marker Felt.ttf", 28);
    if (!_sfxLabel) {
        _sfxLabel = Label::createWithSystemFont("游戏音效", "Arial", 28);
    }
    _sfxLabel->setColor(Color3B::WHITE);
    _sfxLabel->setPosition(Vec2(visibleSize.width / 2 - 120, visibleSize.height / 2));
    this->addChild(_sfxLabel, 2);

    _sfxSlider = Slider::create();
    _sfxSlider->setScale9Enabled(true);
    _sfxSlider->setContentSize(Size(200, 20));

    // 创建滑块背景
    auto sfxSliderTrack = DrawNode::create();
    sfxSliderTrack->drawSolidRect(Vec2(-100, -10), Vec2(100, 10), Color4F(0.3f, 0.3f, 0.3f, 1.0f));
    _sfxSlider->addChild(sfxSliderTrack, -1);

    // 创建滑块进度
    auto sfxSliderProgress = DrawNode::create();
    sfxSliderProgress->drawSolidRect(Vec2(-100, -10), Vec2(100, 10), Color4F(0.2f, 0.6f, 0.2f, 1.0f));
    _sfxSlider->addChild(sfxSliderProgress, -2);

    // 创建滑块按钮
    auto sfxSliderThumb = DrawNode::create();
    sfxSliderThumb->drawSolidCircle(Vec2(0, 0), 15, 0, 20, Color4F(0.8f, 0.8f, 0.8f, 1.0f));
    _sfxSlider->addChild(sfxSliderThumb, -1);

    _sfxSlider->setPosition(Vec2(visibleSize.width / 2 + 40, visibleSize.height / 2));
    _sfxSlider->setPercent(AudioManager::getInstance()->getSoundEffectsVolume() * 100);
    _sfxSlider->addEventListener(CC_CALLBACK_2(SettingsLayer::onSfxSliderChanged, this));
    this->addChild(_sfxSlider, 2);

    _sfxValueLabel = Label::createWithTTF(
        StringUtils::format("%d%%", (int)(AudioManager::getInstance()->getSoundEffectsVolume() * 100)),
        "fonts/Marker Felt.ttf", 24
    );
    if (!_sfxValueLabel) {
        _sfxValueLabel = Label::createWithSystemFont(
            StringUtils::format("%d%%", (int)(AudioManager::getInstance()->getSoundEffectsVolume() * 100)),
            "Arial", 24
        );
    }
    _sfxValueLabel->setColor(Color3B::GREEN);
    _sfxValueLabel->setPosition(Vec2(visibleSize.width / 2 + 160, visibleSize.height / 2));
    this->addChild(_sfxValueLabel, 2);

    // 静音复选框
    auto muteLabel = Label::createWithTTF("静音", "fonts/Marker Felt.ttf", 28);
    if (!muteLabel) {
        muteLabel = Label::createWithSystemFont("静音", "Arial", 28);
    }
    muteLabel->setColor(Color3B::WHITE);
    muteLabel->setPosition(Vec2(visibleSize.width / 2 - 120, visibleSize.height / 2 - 50));
    this->addChild(muteLabel, 2);

    // 创建简单的复选框
    _muteCheckBox = CheckBox::create();
    _muteCheckBox->setZoomScale(0.1f);

    // 创建复选框背景
    auto checkBoxNormal = DrawNode::create();
    checkBoxNormal->drawSolidRect(Vec2(-15, -15), Vec2(15, 15), Color4F(0.4f, 0.4f, 0.4f, 1.0f));
    checkBoxNormal->drawRect(Vec2(-15, -15), Vec2(15, 15), Color4F(1.0f, 1.0f, 1.0f, 1.0f));

    auto checkBoxSelected = DrawNode::create();
    checkBoxSelected->drawSolidRect(Vec2(-15, -15), Vec2(15, 15), Color4F(0.2f, 0.7f, 0.2f, 1.0f));
    checkBoxSelected->drawRect(Vec2(-15, -15), Vec2(15, 15), Color4F(1.0f, 1.0f, 1.0f, 1.0f));

    _muteCheckBox->addChild(checkBoxNormal, -1);
    _muteCheckBox->addChild(checkBoxSelected, -1);

    checkBoxSelected->setVisible(false);

    _muteCheckBox->setPosition(Vec2(visibleSize.width / 2 + 40, visibleSize.height / 2 - 50));
    _muteCheckBox->setSelected(AudioManager::getInstance()->isMuted());
    _muteCheckBox->addEventListener(CC_CALLBACK_2(SettingsLayer::onMuteCheckBoxChanged, this));
    this->addChild(_muteCheckBox, 2);

    // 根据静音状态更新UI
    updateVolumeLabels();
}

void SettingsLayer::addTouchListener()
{
    if (_touchListener && !_isListenerAdded)
    {
        _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
        _isListenerAdded = true;
    }
}

void SettingsLayer::removeTouchListener()
{
    if (_touchListener && _isListenerAdded)
    {
        _eventDispatcher->removeEventListener(_touchListener);
        _isListenerAdded = false;
    }
}

void SettingsLayer::show()
{
    log("SettingsLayer::show() called");

    // 添加触摸事件监听器
    addTouchListener();

    // 设置为可见
    this->setVisible(true);

    // 更新UI状态为当前音频设置
    if (_bgmSlider)
        _bgmSlider->setPercent(AudioManager::getInstance()->getBackgroundMusicVolume() * 100);
    if (_sfxSlider)
        _sfxSlider->setPercent(AudioManager::getInstance()->getSoundEffectsVolume() * 100);
    if (_muteCheckBox)
        _muteCheckBox->setSelected(AudioManager::getInstance()->isMuted());

    updateVolumeLabels();

    // 弹出动画
    this->setScale(0.5f);
    auto scaleAction = ScaleTo::create(0.2f, 1.0f);
    auto easeAction = EaseBackOut::create(scaleAction);
    this->runAction(easeAction);
}

void SettingsLayer::hide()
{
    log("SettingsLayer::hide() called");

    // 移除触摸事件监听器
    removeTouchListener();

    // 缩小动画
    auto scaleAction = ScaleTo::create(0.2f, 0.5f);
    auto easeAction = EaseBackIn::create(scaleAction);
    auto hideAction = CallFunc::create([this]() {
        this->setVisible(false);
        this->setScale(1.0f); // 恢复原始大小
        });

    this->runAction(Sequence::create(easeAction, hideAction, nullptr));
}

void SettingsLayer::onCloseButtonClicked(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
    if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        this->hide();
    }
}

void SettingsLayer::onBgmSliderChanged(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
    if (type == cocos2d::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        cocos2d::ui::Slider* slider = static_cast<cocos2d::ui::Slider*>(sender);
        float volume = slider->getPercent() / 100.0f;

        // 更新音量
        AudioManager::getInstance()->setBackgroundMusicVolume(volume);

        // 如果当前不是静音状态，立即应用音量变化
        if (!AudioManager::getInstance()->isMuted())
        {
            showSliderChangeEffect(_bgmValueLabel, 1.0f, 1.2f);
        }

        // 更新显示
        updateVolumeLabels();
    }
}

void SettingsLayer::onSfxSliderChanged(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
    if (type == cocos2d::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        cocos2d::ui::Slider* slider = static_cast<cocos2d::ui::Slider*>(sender);
        float volume = slider->getPercent() / 100.0f;

        // 更新音量
        AudioManager::getInstance()->setSoundEffectsVolume(volume);

        // 播放一个测试声音来让用户感受音量变化
        if (!AudioManager::getInstance()->isMuted())
        {
            showSliderChangeEffect(_sfxValueLabel, 1.0f, 1.2f);
        }

        // 更新显示
        updateVolumeLabels();
    }
}

void SettingsLayer::onMuteCheckBoxChanged(cocos2d::Ref* sender, cocos2d::ui::CheckBox::EventType type)
{
    bool isMuted = (type == cocos2d::ui::CheckBox::EventType::SELECTED);

    // 设置静音
    AudioManager::getInstance()->setMuted(isMuted);

    // 更新UI状态
    updateVolumeLabels();
}

void SettingsLayer::updateVolumeLabels()
{
    bool isMuted = AudioManager::getInstance()->isMuted();

    // 更新BGM音量显示
    int bgmPercent = (int)(AudioManager::getInstance()->getBackgroundMusicVolume() * 100);
    if (_bgmValueLabel)
    {
        _bgmValueLabel->setString(StringUtils::format("%d%%", bgmPercent));
    }

    // 更新音效音量显示
    int sfxPercent = (int)(AudioManager::getInstance()->getSoundEffectsVolume() * 100);
    if (_sfxValueLabel)
    {
        _sfxValueLabel->setString(StringUtils::format("%d%%", sfxPercent));
    }

    // 根据静音状态改变颜色
    if (isMuted)
    {
        if (_bgmValueLabel)
            _bgmValueLabel->setColor(Color3B::GRAY);
        if (_sfxValueLabel)
            _sfxValueLabel->setColor(Color3B::GRAY);
        if (_bgmSlider)
            _bgmSlider->setEnabled(false);
        if (_sfxSlider)
            _sfxSlider->setEnabled(false);
    }
    else
    {
        if (_bgmValueLabel)
            _bgmValueLabel->setColor(Color3B::GREEN);
        if (_sfxValueLabel)
            _sfxValueLabel->setColor(Color3B::GREEN);
        if (_bgmSlider)
            _bgmSlider->setEnabled(true);
        if (_sfxSlider)
            _sfxSlider->setEnabled(true);
    }
}

void SettingsLayer::showSliderChangeEffect(cocos2d::Node* target, float fromScale, float toScale)
{
    if (target)
    {
        auto scaleUp = ScaleTo::create(0.1f, toScale);
        auto scaleDown = ScaleTo::create(0.1f, fromScale);
        target->runAction(Sequence::create(scaleUp, scaleDown, nullptr));
    }
}