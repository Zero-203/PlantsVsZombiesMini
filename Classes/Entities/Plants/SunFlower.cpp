#include "Sunflower.h"
#include "./Game/GameManager.h"
#include "./Resources/AudioManager.h"

USING_NS_CC;

bool Sunflower::init()
{
    if (!Plant::init())
    {
        return false;
    }

    // 初始化向日葵属性
    initPlant(PlantType::SUNFLOWER, 50, 7.5f, 100);

    // 设置向日葵特有属性
    _produceInterval = 24.0f; // 每24秒生产一次

    // 设置颜色和大小
    this->setColor(Color3B(255, 255, 0)); // 黄色
    this->setContentSize(Size(60, 80));

    // 添加呼吸动画
    auto idleAction = RepeatForever::create(
        Sequence::create(
            ScaleTo::create(1.0f, 1.05f),
            ScaleTo::create(1.0f, 1.0f),
            nullptr
        )
    );
    this->runAction(idleAction);

    // 添加颜色波动动画
    auto colorAction = RepeatForever::create(
        Sequence::create(
            TintTo::create(1.0f, Color3B(255, 255, 100)),
            TintTo::create(1.0f, Color3B(255, 255, 0)),
            nullptr
        )
    );
    this->runAction(colorAction);

    return true;
}

void Sunflower::produceSun()
{
    createSun();
}

void Sunflower::createSun()
{
    log("Sunflower: Producing sun!");

    // 播放生产音效
    auto audioManager = AudioManager::getInstance();
    if (audioManager)
    {
        audioManager->playSoundEffect("Sounds/SFX/sun_produced.mp3");
    }

    // 创建阳光
    auto sun = Sprite::create();
    sun->setTextureRect(Rect(0, 0, 30, 30));
    sun->setColor(Color3B(255, 255, 0));

    // 设置阳光位置（在向日葵上方）
    auto sunflowerPos = this->getPosition();
    sun->setPosition(sunflowerPos + Vec2(0, 80));

    // 获取父节点并添加阳光
    auto parent = this->getParent();
    if (parent)
    {
        parent->addChild(sun, 10); // 最高层级

        // 阳光浮动动画
        auto floatAction = RepeatForever::create(
            Sequence::create(
                MoveBy::create(0.5f, Vec2(0, 10)),
                MoveBy::create(0.5f, Vec2(0, -10)),
                nullptr
            )
        );
        sun->runAction(floatAction);

        // 阳光旋转动画
        auto rotateAction = RepeatForever::create(
            RotateBy::create(2.0f, 360)
        );
        sun->runAction(rotateAction);

        // 添加点击收集功能
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [sun, parent](Touch* touch, Event* event) {
            Vec2 locationInNode = sun->convertToNodeSpace(touch->getLocation());
            Size s = sun->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);

            if (rect.containsPoint(locationInNode))
            {
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
                }

                // 收集动画
                auto scaleUp = ScaleTo::create(0.1f, 1.5f);
                auto fadeOut = FadeOut::create(0.1f);
                auto remove = RemoveSelf::create();

                sun->runAction(Sequence::create(
                    scaleUp,
                    Spawn::create(fadeOut, scaleUp->clone(), nullptr),
                    remove,
                    nullptr
                ));

                return true;
            }
            return false;
        };

        parent->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, sun);

        // 自动消失（10秒后）
        sun->runAction(Sequence::create(
            DelayTime::create(10.0f),
            FadeOut::create(0.5f),
            RemoveSelf::create(),
            nullptr
        ));
    }
}