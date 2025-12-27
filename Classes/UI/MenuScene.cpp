#include "MenuScene.h"
#include "./Game/GameManager.h"
#include "./Resources/AudioManager.h"
#include "./Resources/ResourceLoader.h"

USING_NS_CC;
using namespace ui;

Scene* MenuScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MenuScene::create();
    scene->addChild(layer);
    return scene;
}

bool MenuScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // 获取可视区域大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 预加载资源
    preloadResources();

    // 初始化UI
    initUI();

    // 播放背景音乐
    AudioManager::getInstance()->playBackgroundMusic(
        ResourceLoader::getInstance()->getBackgroundMusicPath("sound_menu_bgm"),
        true
    );

    // 设置更新调度
    this->scheduleUpdate();

    return true;
}

void MenuScene::update(float delta)
{
    // 菜单场景的更新逻辑
}

void MenuScene::initUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 1. 背景
    _background = Sprite::create(
        ResourceLoader::getInstance()->getTexture("menu_background") ?
        ResourceLoader::getInstance()->getTexture("menu_background")->getPath() :
        "Images/Backgrounds/menu_bg.png"
    );
       
    _background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));   
    _background->setScale(visibleSize.width / _background->getContentSize().width,
        visibleSize.height / _background->getContentSize().height);
    this->addChild(_background, 0);

    // 2. 开始按钮
    _startButton = Button::create(
        ResourceLoader::getInstance()->getTexture("start_button_normal") ?
        ResourceLoader::getInstance()->getTexture("start_button_normal")->getPath() :
        "Images/UI/start_btn_normal.png",
        ResourceLoader::getInstance()->getTexture("start_button_pressed") ?
        ResourceLoader::getInstance()->getTexture("start_button_pressed")->getPath() :
        "Images/UI/start_btn_pressed.png"
    );

    if (_startButton)
    {
        _startButton->setPosition(Vec2(visibleSize.width * 23 / 32 + origin.x, visibleSize.height * 0.75f + origin.y));
        _startButton->setScale(1.2f);
        // 添加点击事件
        _startButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onStartButtonClicked, this));
        this->addChild(_startButton, 1);
    }

    /*
    // 3. 设置按钮 - 修改：添加背景和调整位置
    _settingsButton = Button::create();

    // 设置按钮背景（使用与开始按钮相同的背景资源）
    std::string settingsNormal = ResourceLoader::getInstance()->getTexture("start_button_normal") ?
        ResourceLoader::getInstance()->getTexture("start_button_normal")->getPath() :
        "Images/UI/start_btn_normal.png";

    std::string settingsPressed = ResourceLoader::getInstance()->getTexture("start_button_pressed") ?
        ResourceLoader::getInstance()->getTexture("start_button_pressed")->getPath() :
        "Images/UI/start_btn_pressed.png";

    // 尝试加载按钮背景图片
    if (FileUtils::getInstance()->isFileExist(settingsNormal)) {
        _settingsButton->loadTextures(settingsNormal, settingsPressed, "");
    }

    // 设置按钮文本属性
    _settingsButton->setTitleText("设置");
    _settingsButton->setTitleFontName(ResourceLoader::getInstance()->getFontPath("font_main"));
    _settingsButton->setTitleFontSize(24);  // 稍微减小字体大小
    _settingsButton->setTitleColor(Color3B::BLACK);  // 改为黑色以在浅色背景上更明显

    // 调整按钮大小
    _settingsButton->setContentSize(Size(150, 60));

    // 调整位置：放在开始按钮下方
    _settingsButton->setPosition(Vec2(visibleSize.width * 23 / 32 + origin.x,
        visibleSize.height * 0.55f + origin.y));

    // 添加点击事件
    _settingsButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onSettingsButtonClicked, this));
    this->addChild(_settingsButton, 1);

    // 4. 退出按钮 - 修改：添加背景和调整位置
    _exitButton = Button::create();

    // 设置退出按钮背景
    if (FileUtils::getInstance()->isFileExist(settingsNormal)) {
        _exitButton->loadTextures(settingsNormal, settingsPressed, "");
    }

    // 设置退出按钮文本属性
    _exitButton->setTitleText("退出游戏");
    _exitButton->setTitleFontName(ResourceLoader::getInstance()->getFontPath("font_main"));
    _exitButton->setTitleFontSize(24);
    _exitButton->setTitleColor(Color3B::BLACK);  // 改为黑色以在浅色背景上更明显

    // 调整按钮大小
    _exitButton->setContentSize(Size(200, 60));

    // 调整位置：放在设置按钮下方
    _exitButton->setPosition(Vec2(visibleSize.width * 23 / 32 + origin.x,
        visibleSize.height * 0.45f + origin.y));

    // 添加点击事件
    _exitButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onExitButtonClicked, this));
    this->addChild(_exitButton, 1);
    */
   
}

void MenuScene::onStartButtonClicked(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        // 播放按钮点击音效
        AudioManager::getInstance()->playSoundEffect(
            ResourceLoader::getInstance()->getSoundEffectPath("sound_button_click")
        );

        // 播放按钮动画
        playButtonAnimation(static_cast<Button*>(sender));

        // 延迟执行游戏开始，让动画完成
        this->runAction(Sequence::create(
            DelayTime::create(0.3f),
            CallFunc::create([]() {
                GameManager::getInstance()->startNewGame();
                }),
            nullptr
                    ));
    }
}

void MenuScene::onSettingsButtonClicked(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        // 播放按钮点击音效
        AudioManager::getInstance()->playSoundEffect(
            ResourceLoader::getInstance()->getSoundEffectPath("sound_button_click")//""
        );

        // 播放按钮动画
        playButtonAnimation(static_cast<Button*>(sender));

        // TODO: 打开设置界面（在后续阶段实现）
        log("Settings button clicked");
    }
}

void MenuScene::onExitButtonClicked(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        // 播放按钮点击音效
        AudioManager::getInstance()->playSoundEffect(
            ResourceLoader::getInstance()->getSoundEffectPath("sound_button_click")
        );

        // 播放按钮动画
        playButtonAnimation(static_cast<Button*>(sender));

        // 延迟退出，让动画完成
        this->runAction(Sequence::create(
            DelayTime::create(0.3f),
            CallFunc::create([]() {
                Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                exit(0);
#endif
                }),
            nullptr
                    ));
    }
}

void MenuScene::playButtonAnimation(Button* button)
{
    if (!button) return;

    // 创建缩放动画
    auto scaleDown = ScaleTo::create(0.1f, 0.9f);
    auto scaleUp = ScaleTo::create(0.1f, 1.0f);

    button->runAction(Sequence::create(scaleDown, scaleUp, nullptr));
}

void MenuScene::preloadResources()
{
    // 预加载菜单所需资源
    ResourceLoader::getInstance()->preloadResources(ResourceLoader::LoadingPhase::MENU_RESOURCES);
}
