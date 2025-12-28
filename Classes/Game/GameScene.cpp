#include "GameScene.h"
#include "./Game/GameManager.h"
#include "./Resources/AudioManager.h"
#include "./Entities/Plants/PlantFactory.h"
#include "./Resources/ResourceLoader.h"
#include "./UI/PlantCard.h" 
#include "WaveManager.h"
#include "./Entities/Zombie/ZombieNormal.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    auto layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

GameScene::~GameScene()
{
    log("GameScene: Destructor called");

    // 注意：不要调用 release()，因为 Cocos2d-x 使用自动引用计数
    // 植物节点在 removeAllChildrenWithCleanup(true) 时已经被释放

    if (_waveManager)
    {
        _waveManager->clearAllZombies();
    }

    // 只需清空指针向量

    for (auto plant : _plants)
    {
        if (plant && plant->getParent())
        {
            plant->removeFromParent();
        }
    }

    // 清理随机阳光
    for (auto sun : _randomSuns)
    {
        if (sun && sun->getParent())
        {
            sun->removeFromParent();
        }
    }
    _randomSuns.clear();

    _plants.clear();
    _plantCards.clear();
    _cardBarBackground = nullptr;
    _plantPreview = nullptr;
    _pauseButton = nullptr;
    _sunLabel = nullptr;
    _levelLabel = nullptr;

    // 清理 WaveManager 中的殭屍
    if (_waveManager)
    {
        _waveManager->clearAllZombies(); // 使用 reset() 而不是 clearAllZombies()
    }

    // 清理其他資源
    if (_plantPreview && _plantPreview->getParent())
    {
        _plantPreview->removeFromParent();
    }

    // 清理 GameManager 中的子彈
    auto gameManager = GameManager::getInstance();
    if (gameManager)
    {
        gameManager->clearAllProjectiles();
    }
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


    // 获取ResourceLoader并检查动画
    ResourceLoader* resourceLoader = ResourceLoader::getInstance();
    if (resourceLoader)
    {
        resourceLoader->printCachedAnimations();

        // 检查特定动画
        if (!resourceLoader->hasAnimation("pea_fly"))
        {
            log("ERROR: pea_fly animation not loaded!");

            // 尝试重新加载
            std::vector<std::string> peaFlyFrames = {
                "Images/Projectiles/Pea/pea_01.png",
                "Images/Projectiles/Pea/pea_02.png",
                "Images/Projectiles/Pea/pea_03.png",
                "Images/Projectiles/Pea/pea_04.png"
            };
            resourceLoader->loadAnimationFrames("pea_fly", peaFlyFrames, 0.1f);

            // 再次检查
            if (resourceLoader->hasAnimation("pea_fly"))
            {
                log("Successfully reloaded pea_fly animation");
            }
        }
        else
        {
            log("pea_fly animation found in cache");
        }
    }

    // 预加载游戏资源
    resourceLoader = ResourceLoader::getInstance();
    if (resourceLoader)
    {
        log("GameScene: Preloading game resources...");
        resourceLoader->preloadResources(ResourceLoader::LoadingPhase::GAME_RESOURCES);
        log("GameScene: Game resources preloaded");

        // 检查关键动画是否加载成功
        if (resourceLoader->getCachedAnimation("sunflower_idle")) {
            CCLOG("GameScene: Sunflower idle animation loaded successfully");
        }
        else {
            CCLOG("GameScene: WARNING: Sunflower idle animation NOT loaded");
        }

        if (resourceLoader->getCachedAnimation("peashooter_idle")) {
            CCLOG("GameScene: Peashooter idle animation loaded successfully");
        }
        else {
            CCLOG("GameScene: WARNING: Peashooter idle animation NOT loaded");
        }
    }
    else
    {
        CCLOG("GameScene: ERROR: ResourceLoader is null!");
    }

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
        gameManager->setSunCount(500); // 初始50阳光
        updateSunDisplay();
    }

    // 播放背景音乐
    AudioManager::getInstance()->playBackgroundMusic(
        ResourceLoader::getInstance()->getBackgroundMusicPath("sound_game_bgm"),
        true
    );

    // 设置更新调度
    this->scheduleUpdate();
    _waveManager = WaveManager::getInstance();
    initZombieSystem();

    // 初始化随机阳光系统
    initRandomSunSystem();

    log("GameScene: Initialized");
    log("=== GAME SCENE INITIALIZATION COMPLETE ===");
    log("WaveManager instance: %p", _waveManager);
    log("=== GAME SCENE INITIALIZATION REPORT ===");
    log("1. ResourceLoader: %s", resourceLoader ? "OK" : "NULL");
    log("2. GameManager: %s", gameManager ? "OK" : "NULL");
    log("3. WaveManager: %s", _waveManager ? "OK" : "NULL");
    //log("4. AudioManager: %s", audioManager ? "OK" : "NULL");
    log("======================================");

    return true;

}

void GameScene::initZombieSystem()
{
    log("GameScene: Initializing zombie system...");

    // 1. 預加載僵屍資源
    auto resourceLoader = ResourceLoader::getInstance();
    if (resourceLoader)
    {
        log("Preloading zombie resources...");
        resourceLoader->preloadZombieResources();
    }

    // 2. 獲取 WaveManager
    _waveManager = WaveManager::getInstance();
    if (!_waveManager)
    {
        log("ERROR: WaveManager::getInstance() returned null!");
        return;
    }

    // 3. 先重置（不要重新 init，避免多次初始化）
    _waveManager->reset();

    // 4. 設置回調
    _waveManager->setWaveStartedCallback([this](int waveNumber) {
        log("GameScene: Wave %d started", waveNumber);

        if (_levelLabel)
        {
            _levelLabel->setString(StringUtils::format("WAVE %d", waveNumber));

            // 添加淡入淡出效果
            auto fadeOut = FadeOut::create(0.3f);
            auto fadeIn = FadeIn::create(0.3f);
            _levelLabel->runAction(Sequence::create(fadeOut, fadeIn, nullptr));
        }

        // 播放波次開始音效
        auto audioManager = AudioManager::getInstance();
        if (audioManager)
        {
            audioManager->playSoundEffect("Sounds/SFX/wave_start.mp3");
        }
        });

    _waveManager->setWaveCompletedCallback([this](int waveNumber) {
        log("GameScene: Wave %d completed", waveNumber);

        // 不要在这里清理僵尸，让 WaveManager 自己管理
        // 直接延迟后开始下一波
        this->runAction(Sequence::create(
            DelayTime::create(3.0f),
            CallFunc::create([this]() {
                if (_waveManager) {
                    // 检查是否还有更多波次
                    if (_waveManager->getCurrentWave() < _waveManager->getTotalWaves()) {
                        log("GameScene: Starting next wave...");
                        _waveManager->startNextWave();
                    }
                    else {
                        log("GameScene: All waves completed!");
                        // 触发胜利条件
                        auto gameManager = GameManager::getInstance();
                        if (gameManager) {
                            gameManager->gameOver(true);
                        }
                    }
                }
                }),
            nullptr
        ));
        });

    _waveManager->setWaveAllCompletedCallback([this]() {
        log("GameScene: All waves completed!");

        // 延迟后触发胜利
        this->runAction(Sequence::create(
            DelayTime::create(2.0f),
            CallFunc::create([this]() {
                auto gameManager = GameManager::getInstance();
                if (gameManager) {
                    gameManager->gameOver(true);
                }
                }),
            nullptr
        ));
        });

    _waveManager->setGameOverCallback([this]() {
        log("GameScene: Game Over!");

        // 延遲 2 秒後顯示失敗
        this->runAction(Sequence::create(
            DelayTime::create(2.0f),
            CallFunc::create([this]() {
                auto gameManager = GameManager::getInstance();
                if (gameManager)
                {
                    gameManager->gameOver(false);
                }
                }),
            nullptr
        ));
        });

    // 5. 啟動第一波（延遲 3 秒開始）
    this->runAction(Sequence::create(
        DelayTime::create(5.0f),
        CallFunc::create([this]() {
            log("=== Starting first wave ===");
            if (_waveManager) {
                _waveManager->startNextWave(); // 開始第一波
            }
            }),
        nullptr
    ));

    log("GameScene: Zombie system initialized successfully");
}

void GameScene::update(float delta)
{
    // 只在 PLAYING 狀態更新遊戲邏輯
    auto gameManager = GameManager::getInstance();
    if (gameManager && gameManager->getCurrentState() != GameManager::GameState::PLAYING)
    {
        return;
    }

    // 更新植物卡牌狀態
    updatePlantCards();

    // 更新陽光數量
    updateSunDisplay();

    // 更新随机阳光
    updateRandomSuns(delta);

    // 更新植物預覽位置
    if (_hasSelectedPlant && _plantPreview)
    {
        auto touchPos = Director::getInstance()->getVisibleSize() / 2;
        updatePlantPreviewPosition(touchPos);
    }

    // 更新植物行为 - 使用安全的迭代器
    auto it = _plants.begin(); int i = 0;
    while (it != _plants.end())
    {
        if (!*it) {
            it = _plants.erase(it);
            continue;
        }
            
        Plant* plant = *it; i++;
        //log("Plants: %d Check plant %s at %d row %d col",i, (*it)->getName(),(*it)->getRow(), (*it)->getRow());
        if (plant && plant->isAlive())  // 检查植物是否仍然在场景中
        {
            plant->update(delta);
            ++it;
        }
        else
        {
            // 植物已被移除，从列表中删除
            log("Remove plant at %d row %d col", (*it)->getRow(), (*it)->getCol());
            auto gridsystem = GridSystem::getInstance();
            gridsystem->removePlant((*it)->getRow(), (*it)->getCol());
            plant->removeFromParent();
            it = _plants.erase(it);

        }
    }

    // 更新子彈
    if (gameManager)
    {
        gameManager->updateProjectiles(delta);
    }

    // 更新 WaveManager
    if (_waveManager)
    {
        _waveManager->update(delta);
    }
}

void GameScene::initUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 游戏标题
    auto titleLabel = Label::createWithTTF("WAVE 0", "fonts/Marker Felt.ttf", 24);
    titleLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height - 30 + origin.y));
    titleLabel->setColor(Color3B::YELLOW);
    titleLabel->enableOutline(Color4B::BLACK, 2);
    this->addChild(titleLabel, 10);
    _levelLabel = titleLabel;

    // 创建卡牌栏背景
    _cardBarBackground = Sprite::create(
        ResourceLoader::getInstance()->getTexture("card_bar_bg") ?
        ResourceLoader::getInstance()->getTexture("card_bar_bg")->getPath() :
        "Images/UI/card_bar_bg.png"
    );

    if (_cardBarBackground) {
        // 将背景放在屏幕左上角
        _cardBarBackground->setAnchorPoint(Vec2(0, 1));
        _cardBarBackground->setPosition(Vec2(origin.x, origin.y + visibleSize.height));
        //this->addChild(_cardBarBackground, 0);

        
        // 调整大小以容纳阳光数量和卡牌
        _cardBarBackground->setScaleX(1.2f);
        _cardBarBackground->setScaleY(1.2f);
        this->addChild(_cardBarBackground, 0);
        
    }

    // 阳光显示
    _sunLabel = Label::createWithTTF("50", "fonts/Marker Felt.ttf", 24);
    _sunLabel->setPosition(Vec2(115 + origin.x, visibleSize.height - 85 + origin.y));
    _sunLabel->setColor(Color3B::YELLOW);
    _sunLabel->enableOutline(Color4B::BLACK, 3);
    this->addChild(_sunLabel, 10);

    /*
    // 阳光图标
    auto sunIcon = Sprite::create();
    sunIcon->setTextureRect(Rect(0, 0, 30, 30));
    sunIcon->setColor(Color3B(255, 255, 0));
    sunIcon->setPosition(Vec2(30 + origin.x, visibleSize.height - 40 + origin.y));
    this->addChild(sunIcon, 10);

    // 阳光图标动画
    auto rotateAction = RepeatForever::create(RotateBy::create(2.0f, 360));
    sunIcon->runAction(rotateAction);
    */

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

    // 添加測試按鈕
    auto testButton = ui::Button::create();
    testButton->setTitleText("TEST: Spawn Zombie");
    testButton->setTitleFontName("fonts/Marker Felt.ttf");
    testButton->setTitleFontSize(16);
    testButton->setTitleColor(Color3B::WHITE);
    testButton->setContentSize(Size(150, 40));
    testButton->setScale9Enabled(true);
    testButton->setCapInsets(Rect(5, 5, 5, 5));
    testButton->setColor(Color3B(200, 100, 100));
    testButton->setPosition(Vec2(visibleSize.width - 230 + origin.x, visibleSize.height - 80 + origin.y));
    testButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            log("=== MANUAL ZOMBIE SPAWN TEST ===");

            // 手動生成一個殭屍
            auto waveManager = WaveManager::getInstance();
            if (waveManager) {
                Zombie* zombie = waveManager->spawnRandomZombie();
                if (zombie) {
                    log("Manual spawn SUCCESS!");
                }
                else {
                    log("Manual spawn FAILED!");
                }
            }
            else {
                log("ERROR: WaveManager is null!");
            }
        }
        });
    this->addChild(testButton, 10);
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
        PlantType::WALLNUT,
        PlantType::CHERRY_BOMB,
        PlantType::SNOW_PEA
    };

    // 创建植物卡牌
     // 计算起始位置：阳光数量标签的右侧
    float startX = _sunLabel->getPosition().x + _sunLabel->getContentSize().width + 50;
    float cardY = _sunLabel->getPosition().y + 30;
    float cardSpacing = 60; // 卡牌间距

    for (size_t i = 0; i < plantTypes.size(); i++)
    {
        PlantCard* card = PlantCard::create(plantTypes[i]);
        if (card)
        {
            card->setPosition(Vec2(startX + i * cardSpacing, cardY));
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

    updatePlantCards();
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

    // 检查植物冷却
    for (auto plantcard : _plantCards) {
        if (_selectedPlantType == plantcard->getPlantType()) {
            if (plantcard->isCoolingDown()) {
                log("GameScene: Plant is cooling");
                return;
            }
        }
    }

    // 检查阳光是否足够
    int sunCost = PlantFactory::getSunCost(plantType);
    if (!gameManager->spendSun(sunCost))
    {
        log("GameScene: Not enough sun to plant");
        return;
    }

    // 创建植物 - 确保使用自动释放
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

    // 添加到场景 - 使用 addChild，Cocos2d-x 会自动管理内存
    this->addChild(plant, 3);

    // 添加到网格系统
    gridSystem->plantAt(plant, row, col);

    // 添加到植物列表 - 不要 retain，因为父节点已经持有引用
    _plants.push_back(plant);

    // 播放种植音效
    AudioManager::getInstance()->playSoundEffect(
        ResourceLoader::getInstance()->getSoundEffectPath("sound_plant_planted")
    );

    // 更新阳光显示
    updateSunDisplay();

    // 开始冷却
    for (auto plantcard : _plantCards) {
        if (_selectedPlantType == plantcard->getPlantType())
            plantcard->startCooldown();
    }

    // 种植动画 Tofix
    /*
    plant->setScale(0.1f);
    auto scaleAction = ScaleTo::create(0.3f, 1.0f);
    plant->runAction(scaleAction);
    */
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
    _plantPreview->unscheduleUpdate();

    // 添加到场景 - 使用 addChild，Cocos2d-x 会自动管理内存
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

    // 创建半透明覆盖层
    auto overlay = LayerColor::create(Color4B(0, 0, 0, 150), visibleSize.width, visibleSize.height);
    overlay->setPosition(origin);
    overlay->setTag(100);
    this->addChild(overlay, 100);

    // 创建暂停菜单面板（保持原始代码）
    auto panel = ui::Button::create();
    panel->setContentSize(Size(300, 200));
    panel->setScale9Enabled(true);
    panel->setCapInsets(Rect(10, 10, 10, 10));
    panel->setColor(Color3B(50, 50, 100));
    panel->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    panel->setEnabled(true); // 作为背景，不接收事件
    overlay->addChild(panel);

    // 暂停标题
    auto pauseLabel = Label::createWithTTF("PAUSED", "fonts/Marker Felt.ttf", 36);
    pauseLabel->setPosition(Vec2(panel->getContentSize().width / 2, 150));
    pauseLabel->setColor(Color3B::YELLOW);
    panel->addChild(pauseLabel);

    // 继续按钮 - 确保事件可以传递
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

    // 关键：确保按钮可以接收触摸事件
    resumeButton->setTouchEnabled(true);
    resumeButton->setEnabled(true);
    resumeButton->setSwallowTouches(true); // 阻止事件传递到父节点

    resumeButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        log("GameScene: Resume button event type: %d", (int)type);
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            log("GameScene: Resume button clicked");
            hidePauseMenu();
            resumeGame();
        }
        });
    panel->addChild(resumeButton);

    // 重新开始按钮 - 确保事件可以传递
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

    // 关键：确保按钮可以接收触摸事件
    restartButton->setTouchEnabled(true);
    restartButton->setEnabled(true);
    restartButton->setSwallowTouches(true); // 阻止事件传递到父节点

    restartButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        log("GameScene: Restart button event type: %d", (int)type);
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            log("GameScene: Restart button clicked");
            hidePauseMenu();
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

void GameScene::exitToMenu()
{
    log("GameScene: Exiting to menu");

    auto gameManager = GameManager::getInstance();
    if (gameManager)
    {
        gameManager->goToMenuScene();
    }
}

void GameScene::restartGame()
{
    log("GameScene: Restarting game via GameManager");

    // 恢复游戏状态（如果之前是暂停的）
    resumeGame();

    // 隐藏暂停菜单
    hidePauseMenu();

    // 停止所有调度器
    this->unscheduleAllCallbacks();

    // 停止所有动作
    this->stopAllActions();

    // 清理植物列表（不要调用 release，因为 Cocos2d-x 会自动管理）
    for (auto plant : _plants)
    {
        if (plant)
        {
            plant->removeFromParent();
        }
    }
    _plants.clear();

    // 清理植物卡片列表
    _plantCards.clear();

    // 清理预览
    if (_plantPreview)
    {
        _plantPreview->removeFromParent();
        _plantPreview = nullptr;
    }

    // 重置变量
    _selectedPlantType = PlantType::SUNFLOWER;
    _hasSelectedPlant = false;
    _pauseButton = nullptr;
    _sunLabel = nullptr;
    _levelLabel = nullptr;

    // 重置网格系统
    auto gridSystem = GridSystem::getInstance();
    if (gridSystem)
    {
        gridSystem->clearAll();
    }

    // 使用 GameManager 重启游戏
    auto gameManager = GameManager::getInstance();
    if (gameManager)
    {
        gameManager->restartGame();
    }
    else
    {
        // 备用方案：直接替换场景
        auto scene = GameScene::createScene();
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
    }

    // 重置WaveManager
    auto waveManager = WaveManager::getInstance();
    if (waveManager)
    {
        waveManager->reset();
    }
}

// 初始化随机阳光系统
void GameScene::initRandomSunSystem()
{
    log("GameScene: Initializing random sun system...");

    // 初始化参数
    _randomSunTimer = 0.0f;
    _randomSunInterval = 8.0f;  // 初始8秒生成一个随机阳光
    _randomSunSpeed = 60.0f;    // 下落速度

    _randomSuns.clear();

    log("GameScene: Random sun system initialized");
}

// 更新随机阳光
void GameScene::updateRandomSuns(float delta)
{
    // 更新计时器
    _randomSunTimer += delta;

    // 检查是否需要生成新的随机阳光
    if (_randomSunTimer >= _randomSunInterval)
    {
        _randomSunTimer = 0.0f;
        spawnRandomSun();

        // 随机化下一次生成间隔 (5-12秒)
        _randomSunInterval = 5.0f + CCRANDOM_0_1() * 7.0f;
    }

    // 更新所有随机阳光的位置
    auto it = _randomSuns.begin();
    while (it != _randomSuns.end())
    {
        if (!*it)
        {
            it = _randomSuns.erase(it);
            continue;
        }

        cocos2d::Sprite* sun = *it;

        // 检查阳光是否被收集（已经移除了）
        if (!sun->getParent())
        {
            it = _randomSuns.erase(it);
            continue;
        }

        // 继续下一项
        ++it;
    }
}

// 生成随机阳光
void GameScene::spawnRandomSun()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 随机生成位置（从屏幕顶部随机位置落下）
    float randomX = origin.x + 50.0f + CCRANDOM_0_1() * (visibleSize.width - 100.0f);
    float startY = origin.y + visibleSize.height + 30.0f;  // 从屏幕顶部上方开始

    createRandomSunAtPosition(cocos2d::Vec2(randomX, startY));

    log("GameScene: Random sun spawned at (%.0f, %.0f)", randomX, startY);
}

// 在指定位置创建随机阳光
void GameScene::createRandomSunAtPosition(const cocos2d::Vec2& startPos)
{
    log("GameScene: Creating random sun at position (%.0f, %.0f)", startPos.x, startPos.y);

    // 获取ResourceLoader实例
    auto resourceLoader = ResourceLoader::getInstance();

    // 创建太阳精灵
    cocos2d::Sprite* sun = nullptr;

    if (resourceLoader && resourceLoader->hasAnimation("sun_floating"))
    {
        // 使用动画创建太阳
        log("GameScene: Creating sun with sun_floating animation");

        // 获取动画
        auto animation = resourceLoader->getCachedAnimation("sun_floating");
        if (animation)
        {
            // 创建一个精灵来播放动画
            sun = cocos2d::Sprite::create();

            // 创建一个Animate动作
            auto animate = cocos2d::Animate::create(animation);
            if (animate)
            {
                // 循环播放动画
                auto repeatAnimation = cocos2d::RepeatForever::create(animate);
                sun->runAction(repeatAnimation);
            }
        }
    }

    // 如果动画创建失败，使用简单的图形作为后备
    if (!sun)
    {
        log("GameScene: Using fallback sun graphic for random sun");
        sun = cocos2d::Sprite::create();
        sun->setTextureRect(cocos2d::Rect(0, 0, 30, 30));
        sun->setColor(cocos2d::Color3B(255, 255, 0));
    }

    // 设置阳光位置
    sun->setPosition(startPos);

    // 添加到场景
    this->addChild(sun, 10); // 最高层级

    // 添加到阳光列表
    _randomSuns.push_back(sun);

    // 阳光浮动动画
    auto floatAction = cocos2d::RepeatForever::create(
        cocos2d::Sequence::create(
            cocos2d::MoveBy::create(0.5f, cocos2d::Vec2(0, 10)),
            cocos2d::MoveBy::create(0.5f, cocos2d::Vec2(0, -10)),
            nullptr
        )
    );
    sun->runAction(floatAction);

    // 阳光旋转动画
    auto rotateAction = cocos2d::RepeatForever::create(
        cocos2d::RotateBy::create(4.0f, 360)
    );
    sun->runAction(rotateAction);

    // 添加点击收集功能
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [sun, this](cocos2d::Touch* touch, cocos2d::Event* event) {
        cocos2d::Vec2 locationInNode = sun->convertToNodeSpace(touch->getLocation());
        cocos2d::Size s = sun->getContentSize();
        cocos2d::Rect rect = cocos2d::Rect(0, 0, s.width, s.height);

        if (rect.containsPoint(locationInNode))
        {
            collectRandomSun(sun);
            return true;
        }
        return false;
    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, sun);

    // 自动消失（15秒后）
    sun->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(15.0f),
        cocos2d::FadeOut::create(0.5f),
        cocos2d::CallFunc::create([sun, this]() {
            // 从列表中移除
            auto it = std::find(_randomSuns.begin(), _randomSuns.end(), sun);
            if (it != _randomSuns.end())
            {
                _randomSuns.erase(it);
            }
            }),
        cocos2d::RemoveSelf::create(),
                nullptr
                ));

    // 下落动画
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 计算落点位置（草坪区域）
    float endY = origin.y + 100.0f + CCRANDOM_0_1() * 300.0f;
    float endX = startPos.x + (CCRANDOM_MINUS1_1() * 50.0f); // 轻微左右偏移
    cocos2d::Vec2 endPos = cocos2d::Vec2(endX, endY);

    // 抛物线下落效果
    float fallDuration = 3.0f + CCRANDOM_0_1() * 0.5f; // 2.5-3.5秒

    auto fallAction = cocos2d::Sequence::create(
        cocos2d::Spawn::create(
            cocos2d::MoveTo::create(fallDuration, endPos),
            cocos2d::ScaleTo::create(fallDuration * 0.3f, 1.2f),  // 先放大
            cocos2d::ScaleTo::create(fallDuration * 0.7f, 1.0f),  // 再恢复
            nullptr
        ),
        nullptr
    );

    sun->runAction(fallAction);
}

// 收集随机阳光
void GameScene::collectRandomSun(cocos2d::Sprite* sun)
{
    if (!sun) return;

    // 播放收集音效
    auto audioManager = AudioManager::getInstance();
    if (audioManager)
    {
        audioManager->playSoundEffect("Sounds/SFX/sun_collected.mp3");
    }

    // 增加阳光数量
    auto gameManager = GameManager::getInstance();
    if (gameManager)
    {
        gameManager->addSun(25);
        updateSunDisplay();
    }

    // 收集动画
    auto scaleUp = cocos2d::ScaleTo::create(0.1f, 1.5f);
    auto fadeOut = cocos2d::FadeOut::create(0.1f);
    auto remove = cocos2d::RemoveSelf::create();

    auto sequence = cocos2d::Sequence::create(
        cocos2d::Spawn::create(scaleUp, fadeOut, nullptr),
        cocos2d::CallFunc::create([sun, this]() {
            // 从列表中移除
            auto it = std::find(_randomSuns.begin(), _randomSuns.end(), sun);
            if (it != _randomSuns.end())
            {
                _randomSuns.erase(it);
            }
            }),
        remove,
                nullptr
                );

    sun->runAction(sequence);
}
