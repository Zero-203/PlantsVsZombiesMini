#include "PlantCard.h"
#include "./Entities/Plants/PlantFactory.h"
#include <Resources/AudioManager.h>
#include "Resources/ResourceLoader.h"

USING_NS_CC;

PlantCard* PlantCard::create(PlantType plantType)
{
    PlantCard* card = new PlantCard();
    if (card && card->init(plantType))
    {
        card->autorelease();
        return card;
    }
    delete card;
    return nullptr;
}

bool PlantCard::init(PlantType plantType)
{
    if (!ui::Button::init())
    {
        return false;
    }

    _plantType = plantType;
    _sunCost = PlantFactory::getSunCost(plantType);
    _cooldown = PlantFactory::getCooldown(plantType);
    _cooldownTimer = 0.0f;
    _isAvailable = false;
    _isCoolingDown = false;

    // 设置卡牌大小
    this->setContentSize(Size(80, 80));
    this->setScale9Enabled(true);
    this->setCapInsets(Rect(10, 10, 60, 80));

    // 设置卡牌背景颜色（根据植物类型）
    Color3B cardColor;
    switch (plantType) {
    case PlantType::SUNFLOWER:
        cardColor = Color3B(255, 255, 0); // 黄色
        break;
    case PlantType::PEASHOOTER:
        cardColor = Color3B(0, 200, 0); // 绿色
        break;
    case PlantType::WALLNUT:
        cardColor = Color3B(139, 69, 19); // 棕色
        break;
    default:
        cardColor = Color3B(200, 200, 200); // 灰色
        break;
    }

    this->setColor(cardColor);
    this->setOpacity(200);

    // 添加植物名称标签
    std::string plantName = PlantFactory::getPlantName(plantType);
    auto nameLabel = Label::createWithTTF(plantName, "fonts/Marker Felt.ttf", 12);
    nameLabel->setPosition(Vec2(this->getContentSize().width / 2, 85));
    nameLabel->setColor(Color3B::YELLOW);
    nameLabel->setAlignment(TextHAlignment::CENTER);
    this->addChild(nameLabel);

    /*
    // 添加阳光消耗标签
    _sunCostLabel = Label::createWithTTF(StringUtils::toString(_sunCost),
        "fonts/Marker Felt.ttf", 18);
    _sunCostLabel->setPosition(Vec2(this->getContentSize().width / 2, 20));
    _sunCostLabel->setColor(Color3B::BLACK);
    _sunCostLabel->enableOutline(Color4B::YELLOW, 2);
    this->addChild(_sunCostLabel);
    */

    // 使用资源加载器加载植物第一张图片作为卡牌背景
    auto resourceLoader = ResourceLoader::getInstance();

    // 根据植物类型获取对应的第一张图片
    std::string imagePath;

    switch (this->getPlantType()) {
    case PlantType::SUNFLOWER:
        imagePath = "Images/Plants/Sunflower/sunflower_idle_01.png";
        break;
    case PlantType::PEASHOOTER:
        imagePath = "Images/Plants/Peashooter/peashooter_idle_01.png";
        break;
    case PlantType::WALLNUT:
        imagePath = "Images/Plants/WallNut/wallnut_idle_01.png";
        break;
    default:
        imagePath = "Images/UI/card_default.png";
    }

    // 预加载图片
    resourceLoader->loadPNGFramesToTextureCache({ imagePath });

    // 创建植物图片精灵作为卡牌背景
    auto plantSprite = Sprite::create(imagePath);
    if (plantSprite) {
        // 调整大小以适应卡牌
        plantSprite->setScale(0.8f);
        plantSprite->setPosition(this->getContentSize().width / 2,
            this->getContentSize().height / 2);
        plantSprite->setOpacity(200); // 半透明
        this->addChild(plantSprite, -1); // 放在最底层作为背景
    }

    /*
    // 添加阳光图标
    auto sunIcon = Sprite::create();
    sunIcon->setTextureRect(Rect(0, 0, 20, 20));
    sunIcon->setColor(Color3B(255, 255, 0));
    sunIcon->setPosition(Vec2(this->getContentSize().width / 2, 45));
    this->addChild(sunIcon);

    // 添加阳光图标动画
    auto rotateAction = RepeatForever::create(RotateBy::create(2.0f, 360));
    sunIcon->runAction(rotateAction);
    */

    // 初始化冷却遮罩
    _cooldownOverlay = DrawNode::create();
    _cooldownOverlay->drawSolidRect(
        Vec2::ZERO,
        Vec2(this->getContentSize().width, this->getContentSize().height),
        Color4F(0, 0, 0, 0.7f)
    );
    _cooldownOverlay->setVisible(false);
    this->addChild(_cooldownOverlay, 1);

    // 设置初始状态
    updateCardState(0);

    // 设置触摸事件
    this->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            if (_isAvailable && !_isCoolingDown)
            {
                // 播放选择音效
                auto audioManager = AudioManager::getInstance();
                if (audioManager)
                {
                    audioManager->playSoundEffect("Sounds/SFX/seed_select.mp3");
                }

                log("PlantCard: Selected %s", PlantFactory::getPlantName(_plantType).c_str());
            }
            else
            {
                // 播放不可用音效
                auto audioManager = AudioManager::getInstance();
                if (audioManager)
                {
                    audioManager->playSoundEffect("Sounds/SFX/seed_unavailable.mp3");
                }

                log("PlantCard: Card unavailable or cooling down");
            }
        }
        });

    return true;
}

void PlantCard::updateCardState(int currentSun)
{
    bool canAfford = (currentSun >= _sunCost);

    if (canAfford && !_isCoolingDown)
    {
        _isAvailable = true;
        this->setOpacity(255); // 完全显示
        //_sunCostLabel->setColor(Color3B::BLACK);
    }
    else
    {
        _isAvailable = false;
        this->setOpacity(150); // 半透明
        //_sunCostLabel->setColor(Color3B::GRAY);
    }
}

void PlantCard::startCooldown()
{
    if (_isCoolingDown)
    {
        return;
    }

    _isCoolingDown = true;
    _cooldownTimer = _cooldown;
    _isAvailable = false;

    // 显示冷却效果
    _cooldownOverlay->setVisible(true);

    this->setOpacity(150);
    //_sunCostLabel->setColor(Color3B::GRAY);

    // 开始冷却计时
    this->schedule(CC_SCHEDULE_SELECTOR(PlantCard::updateCoolingDown), 0.1f);

    log("PlantCard: Started cooldown for %s (%.1f seconds)",
        PlantFactory::getPlantName(_plantType).c_str(), _cooldown);
}

void PlantCard::resetCooldown()
{
    _isCoolingDown = false;
    _cooldownTimer = 0.0f;

    // 隐藏冷却效果
    _cooldownOverlay->setVisible(false);

    // 停止计时
    this->unschedule(CC_SCHEDULE_SELECTOR(PlantCard::updateCoolingDown));

    log("PlantCard: Cooldown reset for %s", PlantFactory::getPlantName(_plantType).c_str());
}

void PlantCard::updateCoolingDown(float delta)
{
    if (!_isCoolingDown)
    {
        return;
    }

    _cooldownTimer -= delta;

    // 显示剩余时间
    if (_cooldownTimer <= 0.0f)
    {
        // 冷却完成
        _isCoolingDown = false;
        _cooldownOverlay->setVisible(false);

        // 播放冷却完成音效
        auto audioManager = AudioManager::getInstance();
        if (audioManager)
        {
            audioManager->playSoundEffect("Sounds/SFX/seed_ready.mp3");
        }

        // 停止计时
        this->unschedule(CC_SCHEDULE_SELECTOR(PlantCard::updateCoolingDown));

        log("PlantCard: Cooldown finished for %s", PlantFactory::getPlantName(_plantType).c_str());
    }
}