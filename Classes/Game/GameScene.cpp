#include "GameScene.h"
#include "./Game/GameManager.h"
#include "./Resources/AudioManager.h"
#include "./Entities/Plants/PlantFactory.h"
#include "./Resources/ResourceLoader.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    auto layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

bool GameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // 获取可视区域大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 初始化背景
    //auto background = LayerColor::create(Color4B(100, 180, 100, 255), visibleSize.width, visibleSize.height);
    //this->addChild(background, 0);

    auto background = Sprite::create(
        ResourceLoader::getInstance()->getTexture("game_background") ?
        ResourceLoader::getInstance()->getTexture("game_background")->getPath() :
        "Images/Backgrounds/game_bg_1.png"
    );

    background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    background->setScale(visibleSize.width / background->getContentSize().width,
        visibleSize.height / background->getContentSize().height);
    this->addChild(background, 0);


    // 初始化网格系统
    initGrid();

    // 初始化UI
    initUI();

    // 初始化植物卡牌
    initPlantCards();

    // 初始化触摸处理器
    initTouchHandlers();

    // 初始化游戏状态
    _selectedPlantType = PlantType::SUNFLOWER;
    _hasSelectedPlant = false;
    _plantPreview = nullptr;

    // 设置初始阳光
    auto gameManager = GameManager::getInstance();
    if (gameManager)
    {
        gameManager->setSunCount(50); // 初始50阳光
        updateSunDisplay();
    }

    // 播放游戏背景音乐
    auto audioManager = AudioManager::getInstance();
    if (audioManager)
    {
        audioManager->playBackgroundMusic("Sounds/BGM/game_bgm.mp3", true);
    }

    // 设置更新调度
    this->scheduleUpdate();

    log("GameScene: Initialized");
    return true;
}

void GameScene::update(float delta)
{
    // 更新植物卡牌状态
    updatePlantCards();

    // 更新植物预览位置（如果正在选择植物）
    if (_hasSelectedPlant && _plantPreview)
    {
        auto touchPos = Director::getInstance()->getVisibleSize() / 2; // 临时位置
        updatePlantPreviewPosition(touchPos);
    }

    // 更新植物行为
    for (auto plant : _plants)
    {
        if (plant && plant->isAlive())
        {
            plant->update(delta);
        }
    }
}

void GameScene::initUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 游戏标题
    auto titleLabel = Label::createWithTTF("LEVEL 1", "fonts/Marker Felt.ttf", 24);
    titleLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height - 30 + origin.y));
    titleLabel->setColor(Color3B::YELLOW);
    titleLabel->enableOutline(Color4B::BLACK, 2);
    this->addChild(titleLabel, 10);
    _levelLabel = titleLabel;

    // 阳光显示
    _sunLabel = Label::createWithTTF("50", "fonts/Marker Felt.ttf", 32);
    _sunLabel->setPosition(Vec2(70 + origin.x, visibleSize.height - 40 + origin.y));
    _sunLabel->setColor(Color3B::YELLOW);
    _sunLabel->enableOutline(Color4B::BLACK, 3);
    this->addChild(_sunLabel, 10);

    // 阳光图标
    auto sunIcon = Sprite::create();
    sunIcon->setTextureRect(Rect(0, 0, 30, 30));
    sunIcon->setColor(Color3B(255, 255, 0));
    sunIcon->setPosition(Vec2(30 + origin.x, visibleSize.height - 40 + origin.y));
    this->addChild(sunIcon, 10);

    // 阳光图标动画
    auto rotateAction = RepeatForever::create(RotateBy::create(2.0f, 360));
    sunIcon->runAction(rotateAction);

    // 暂停按钮
    _pauseButton = ui::Button::create();
    _pauseButton->setTitleText("Pause");
    _pauseButton->setTitleFontName("fonts/Marker Felt.ttf");
    _pauseButton->setTitleFontSize(20);
    _pauseButton->setTitleColor(Color3B::WHITE);
    _pauseButton->setContentSize(Size(80, 40));
    _pauseButton->setScale9Enabled(true);
    _pauseButton->setCapInsets(Rect(5, 5, 5, 5));
    _pauseButton->setColor(Color3B(100, 100, 200));
    _pauseButton->setPosition(Vec2(visibleSize.width - 50 + origin.x, visibleSize.height - 30 + origin.y));
    _pauseButton->addTouchEventListener(CC_CALLBACK_2(GameScene::onPauseButtonClicked, this));
    this->addChild(_pauseButton, 10);

    // 返回菜单按钮
    auto backButton = ui::Button::create();
    backButton->setTitleText("Menu");
    backButton->setTitleFontName("fonts/Marker Felt.ttf");
    backButton->setTitleFontSize(20);
    backButton->setTitleColor(Color3B::WHITE);
    backButton->setContentSize(Size(80, 40));
    backButton->setScale9Enabled(true);
    backButton->setCapInsets(Rect(5, 5, 5, 5));
    backButton->setColor(Color3B(200, 100, 100));
    backButton->setPosition(Vec2(visibleSize.width - 140 + origin.x, visibleSize.height - 30 + origin.y));
    backButton->addTouchEventListener(CC_CALLBACK_2(GameScene::onBackButtonClicked, this));
    this->addChild(backButton, 10);

    /*
    // 添加草坪纹理（视觉增强）
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            auto grassPatch = Sprite::create();
            grassPatch->setTextureRect(Rect(0, 0, 100, 100));
            grassPatch->setColor(Color3B(80 + (i + j) % 2 * 20,
                140 + (i + j) % 2 * 20,
                80 + (i + j) % 2 * 20));
            grassPatch->setPosition(Vec2(60 + i * 100 + origin.x,
                100 + j * 100 + origin.y));
            grassPatch->setOpacity(100);
            this->addChild(grassPatch, 1);
        }
    }
    */
}

void GameScene::initGrid()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 初始化网格系统：5行9列，每格80x100像素
    auto gridSystem = GridSystem::getInstance();
    gridSystem->init(5, 9, Size(90, 130), Vec2(145 + origin.x, 25 + origin.y));

    // 设置网格点击回调
    gridSystem->setClickCallback([this](int row, int col, const Vec2& worldPos) {
        this->onGridClicked(row, col, worldPos);
        });

    // 绘制调试网格（开发时使用，发布时可注释掉）
    gridSystem->drawDebugGrid(this);
}

void GameScene::initPlantCards()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 植物卡牌类型
    std::vector<PlantType> plantTypes = {
        PlantType::SUNFLOWER,
        PlantType::PEASHOOTER,
        PlantType::WALLNUT
    };

    // 创建植物卡牌
    float startX = 50 + origin.x;
    float yPos = 50 + origin.y;
    float spacing = 90;

    for (size_t i = 0; i < plantTypes.size(); i++)
    {
        PlantCard* card = PlantCard::create(plantTypes[i]);
        if (card)
        {
            card->setPosition(Vec2(startX + i * spacing, yPos));
            this->addChild(card, 5);
            _plantCards.push_back(card);

            // 设置卡牌点击事件
            card->addTouchEventListener([this, plantType = plantTypes[i]](Ref* sender, ui::Widget::TouchEventType type) {
                if (type == ui::Widget::TouchEventType::ENDED)
                {
                    this->onPlantCardSelected(plantType);
                }
            });
        }
    }
}

void GameScene::initTouchHandlers()
{
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);

    touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
        // 如果正在选择植物，更新预览位置
        if (_hasSelectedPlant)
        {
            updatePlantPreviewPosition(touch->getLocation());
        }
        return true;
    };

    touchListener->onTouchMoved = [this](Touch* touch, Event* event) {
        // 如果正在选择植物，更新预览位置
        if (_hasSelectedPlant)
        {
            updatePlantPreviewPosition(touch->getLocation());
        }
    };

    touchListener->onTouchEnded = [this](Touch* touch, Event* event) {
        // 处理网格点击
        auto gridSystem = GridSystem::getInstance();
        gridSystem->handleTouch(touch->getLocation());

        // 如果正在选择植物，点击后取消选择
        if (_hasSelectedPlant)
        {
            hidePlantPreview();
            _hasSelectedPlant = false;
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void GameScene::updateSunDisplay()
{
    auto gameManager = GameManager::getInstance();
    if (gameManager && _sunLabel)
    {
        int sunCount = gameManager->getSunCount();
        _sunLabel->setString(StringUtils::toString(sunCount));
    }
}

void GameScene::updatePlantCards()
{
    auto gameManager = GameManager::getInstance();
    if (!gameManager)
    {
        return;
    }

    int currentSun = gameManager->getSunCount();

    for (auto card : _plantCards)
    {
        if (card)
        {
            card->updateCardState(currentSun);
        }
    }
}

void GameScene::onPlantCardSelected(PlantType plantType)
{
    auto gameManager = GameManager::getInstance();
    if (!gameManager)
    {
        return;
    }

    // 检查阳光是否足够
    int sunCost = PlantFactory::getSunCost(plantType);
    if (gameManager->getSunCount() < sunCost)
    {
        log("GameScene: Not enough sun to select plant");
        return;
    }

    // 设置选中的植物类型
    _selectedPlantType = plantType;
    _hasSelectedPlant = true;

    // 显示植物预览
    showPlantPreview(plantType, Director::getInstance()->getVisibleSize() / 2);

    log("GameScene: Selected plant type %d", (int)plantType);
}

void GameScene::onGridClicked(int row, int col, const Vec2& worldPos)
{
    log("GameScene: Grid clicked at row %d, col %d", row, col);

    // 如果正在选择植物，则尝试放置植物
    if (_hasSelectedPlant)
    {
        placePlant(_selectedPlantType, row, col);
        hidePlantPreview();
        _hasSelectedPlant = false;
    }
}

void GameScene::onPauseButtonClicked(Ref* sender, ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        pauseGame();
        showPauseMenu();
    }
}

void GameScene::onBackButtonClicked(Ref* sender, ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        exitToMenu();
    }
}

void GameScene::placePlant(PlantType plantType, int row, int col)
{
    auto gridSystem = GridSystem::getInstance();
    auto gameManager = GameManager::getInstance();

    if (!gridSystem || !gameManager)
    {
        return;
    }

    // 检查格子是否可用
    if (!gridSystem->canPlantAt(row, col))
    {
        // 播放错误音效
        auto audioManager = AudioManager::getInstance();
        if (audioManager)
        {
            audioManager->playSoundEffect("Sounds/SFX/plant_fail.mp3");
        }

        log("GameScene: Cannot plant at row %d, col %d", row, col);
        return;
    }

    // 检查阳光是否足够
    int sunCost = PlantFactory::getSunCost(plantType);
    if (!gameManager->spendSun(sunCost))
    {
        log("GameScene: Not enough sun to plant");
        return;
    }

    // 创建植物
    Plant* plant = PlantFactory::createPlant(plantType);
    if (!plant)
    {
        log("GameScene: Failed to create plant");
        return;
    }

    // 设置植物位置
    Vec2 plantPos = gridSystem->gridToWorldCenter(row, col);
    plant->setPosition(plantPos);
    plant->setGridPosition(row, col);

    // 添加到场景
    this->addChild(plant, 3); // 在草坪之上

    // 添加到网格系统
    gridSystem->plantAt(plant, row, col);

    // 添加到植物列表
    _plants.push_back(plant);

    // 播放种植音效
    auto audioManager = AudioManager::getInstance();
    if (audioManager)
    {
        audioManager->playSoundEffect("Sounds/SFX/plant_plant.mp3");
    }

    // 更新阳光显示
    updateSunDisplay();

    // 种植动画
    plant->setScale(0.1f);
    auto scaleAction = ScaleTo::create(0.3f, 1.0f);
    plant->runAction(scaleAction);

    log("GameScene: Plant placed at row %d, col %d", row, col);
}

void GameScene::showPlantPreview(PlantType plantType, const Vec2& position)
{
    // 如果已有预览，先移除
    hidePlantPreview();

    // 创建预览植物
    _plantPreview = PlantFactory::createPlant(plantType);
    if (!_plantPreview)
    {
        return;
    }

    // 设置预览属性
    _plantPreview->setPosition(position);
    _plantPreview->setOpacity(150); // 半透明

    // 添加到场景
    this->addChild(_plantPreview, 4);

    log("GameScene: Plant preview shown");
}

void GameScene::hidePlantPreview()
{
    if (_plantPreview)
    {
        _plantPreview->removeFromParent();
        _plantPreview = nullptr;
    }
}

void GameScene::updatePlantPreviewPosition(const Vec2& position)
{
    if (_plantPreview)
    {
        _plantPreview->setPosition(position);

        // 检查当前位置是否可以放置
        int row, col;
        auto gridSystem = GridSystem::getInstance();
        if (gridSystem && gridSystem->worldToGrid(position, row, col))
        {
            if (gridSystem->canPlantAt(row, col))
            {
                _plantPreview->setColor(Color3B::WHITE); // 可以放置：白色
            }
            else
            {
                _plantPreview->setColor(Color3B::RED); // 不能放置：红色
            }
        }
    }
}

void GameScene::addSun(int amount)
{
    auto gameManager = GameManager::getInstance();
    if (gameManager)
    {
        gameManager->addSun(amount);
        updateSunDisplay();
    }
}

void GameScene::spendSun(int amount)
{
    auto gameManager = GameManager::getInstance();
    if (gameManager)
    {
        gameManager->spendSun(amount);
        updateSunDisplay();
    }
}

void GameScene::pauseGame()
{
    Director::getInstance()->pause();

    auto audioManager = AudioManager::getInstance();
    if (audioManager)
    {
        audioManager->pauseBackgroundMusic();
    }

    log("GameScene: Game paused");
}

void GameScene::resumeGame()
{
    Director::getInstance()->resume();

    auto audioManager = AudioManager::getInstance();
    if (audioManager)
    {
        audioManager->resumeBackgroundMusic();
    }

    log("GameScene: Game resumed");
}

void GameScene::showPauseMenu()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建半透明背景
    auto overlay = LayerColor::create(Color4B(0, 0, 0, 150), visibleSize.width, visibleSize.height);
    overlay->setPosition(origin);
    overlay->setTag(100); // 用于标识
    this->addChild(overlay, 100);

    // 创建暂停菜单面板
    auto panel = ui::Button::create();
    panel->setContentSize(Size(300, 200));
    panel->setScale9Enabled(true);
    panel->setCapInsets(Rect(10, 10, 10, 10));
    panel->setColor(Color3B(50, 50, 100));
    panel->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    panel->setEnabled(false); // 仅作为背景
    overlay->addChild(panel);

    // 暂停标题
    auto pauseLabel = Label::createWithTTF("PAUSED", "fonts/Marker Felt.ttf", 36);
    pauseLabel->setPosition(Vec2(panel->getContentSize().width / 2, 150));
    pauseLabel->setColor(Color3B::YELLOW);
    panel->addChild(pauseLabel);

    // 继续按钮
    auto resumeButton = ui::Button::create();
    resumeButton->setTitleText("Resume");
    resumeButton->setTitleFontName("fonts/Marker Felt.ttf");
    resumeButton->setTitleFontSize(24);
    resumeButton->setTitleColor(Color3B::WHITE);
    resumeButton->setContentSize(Size(200, 50));
    resumeButton->setScale9Enabled(true);
    resumeButton->setCapInsets(Rect(5, 5, 5, 5));
    resumeButton->setColor(Color3B(100, 200, 100));
    resumeButton->setPosition(Vec2(panel->getContentSize().width / 2, 100));
    resumeButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            hidePauseMenu();
            resumeGame();
        }
        });
    panel->addChild(resumeButton);

    // 重新开始按钮
    auto restartButton = ui::Button::create();
    restartButton->setTitleText("Restart");
    restartButton->setTitleFontName("fonts/Marker Felt.ttf");
    restartButton->setTitleFontSize(24);
    restartButton->setTitleColor(Color3B::WHITE);
    restartButton->setContentSize(Size(200, 50));
    restartButton->setScale9Enabled(true);
    restartButton->setCapInsets(Rect(5, 5, 5, 5));
    restartButton->setColor(Color3B(200, 200, 100));
    restartButton->setPosition(Vec2(panel->getContentSize().width / 2, 50));
    restartButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            restartGame();
        }
        });
    panel->addChild(restartButton);

    log("GameScene: Pause menu shown");
}

void GameScene::hidePauseMenu()
{
    // 移除暂停菜单
    auto overlay = this->getChildByTag(100);
    if (overlay)
    {
        overlay->removeFromParent();
    }

    log("GameScene: Pause menu hidden");
}

void GameScene::restartGame()
{
    log("GameScene: Restarting game");

    // 重新开始场景
    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
}

void GameScene::exitToMenu()
{
    log("GameScene: Exiting to menu");

    auto gameManager = GameManager::getInstance();
    if (gameManager)
    {
        gameManager->goToMenuScene();
    }
}