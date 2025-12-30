#include "PotatoMine.h"
#include "./Game/GameManager.h"
#include "./Resources/AudioManager.h"
#include "./Resources/ResourceLoader.h"
#include <Entities/Zombie/Zombie.h>
#include <Game/WaveManager.h>

USING_NS_CC;

bool PotatoMine::init()
{
    if (!Plant::init())
    {
        return false;
    }

    // 设置地雷属性
    _armingTime = 20.0f;          // 20秒准备时间
    _armingTimer = 0.0f;          // 准备计时器
    _isArmed = false;             // 初始未激活
    _isTriggered = false;         // 初始未触发

    _triggerRange = 30.0f;        // 触发范围（僵尸进入此范围即触发）
    _explosionRange = 80.0f;     // 爆炸范围（实际伤害范围，大于触发范围）
    _explosionDamage = 200;      // 爆炸伤害，秒杀普通僵尸

    // 设置动画名称
    setIdleAnimation("potatomine_idle");      // 未激活状态
    setDyingAnimation("potatomine_explode");  // 爆炸动画

    // 播放初始动画（未激活状态）
    checkAndPlayIdleAnimation();

    log("PotatoMine: Initialized with arming time: %.1f, trigger range: %.1f, explosion range: %.1f, damage: %d",
        _armingTime, _triggerRange, _explosionRange, _explosionDamage);

    return true;
}

void PotatoMine::onIdle(float delta)
{
    // 如果已触发或死亡，不处理
    if (_isTriggered || !isAlive() || _state == PlantState::DYING)
    {
        return;
    }

    // 如果还未激活，进行准备计时
    if (!_isArmed)
    {
        _armingTimer += delta;

        // 检查是否完成准备
        if (_armingTimer >= _armingTime)
        {
            arm();  // 激活地雷
        }
        else
        {
            // 准备过程中闪烁提示（最后5秒）
            float timeLeft = _armingTime - _armingTimer;
            if (timeLeft <= 5.0f)
            {
                // 每0.5秒闪烁一次
                static float flashTimer = 0.0f;
                flashTimer += delta;
                if (flashTimer >= 0.5f)
                {
                    flashTimer = 0.0f;
                    auto fadeOut = FadeTo::create(0.2f, 150);
                    auto fadeIn = FadeTo::create(0.2f, 255);
                    this->runAction(Sequence::create(fadeOut, fadeIn, nullptr));
                }
            }
        }
    }
    else
    {
        // 已激活，检查是否有僵尸在触发范围内
        if (PotatoMine::checkZombieInTriggerRange())
        {
            trigger();  // 触发爆炸
        }
    }
}

void PotatoMine::arm()
{
    if (_isArmed) return;

    _isArmed = true;

    // 停止当前idle动画
    this->stopAllActions();
    stopCurrentAnimation();

    // 设置完全可见（确保不透明）
    this->setOpacity(255);

    // 播放激活动画
    playAnimation("potatomine_armed", true);

    // 播放激活音效
    playArmingSound();

    log("PotatoMine: Armed and ready!");
}

void PotatoMine::trigger()
{
    if (_isTriggered) return;

    _isTriggered = true;
    _state = PlantState::DYING;

    log("PotatoMine: Triggered! Exploding...");

    // 停止所有动作
    this->stopAllActions();

    // 播放爆炸动画
    playAnimation("potatomine_explode", false, [this]() {
        this->onExplosionComplete();
        });

    // 播放爆炸音效
    playExplosionSound();

    // 创建爆炸特效
    createMineEffect();

    // 计算爆炸范围 - 只影响同一行的相邻僵尸
    Vec2 minePos = this->getPosition();
    float rangeSquared = _explosionRange * _explosionRange;

    // 对爆炸范围内的僵尸造成伤害
    auto waveManager = WaveManager::getInstance();
    if (waveManager)
    {
        // 只获取同一行的僵尸
        auto zombiesInRow = waveManager->getZombiesInRow(_row);
        for (auto zombie : zombiesInRow)
        {
            if (zombie && zombie->isAlive())
            {
                Vec2 zombiePos = zombie->getPosition();
                float distanceSquared = minePos.distanceSquared(zombiePos);

                if (distanceSquared <= rangeSquared)
                {
                    // 对僵尸造成伤害
                    zombie->takeDamage(_explosionDamage);
                    log("PotatoMine: Zombie at distance %.1f hit with explosion damage: %d",
                        sqrt(distanceSquared), _explosionDamage);
                }
            }
        }
    }
}

bool PotatoMine::checkZombieInTriggerRange()
{
    if (!_isArmed || !isAlive()) return false;

    auto waveManager = WaveManager::getInstance();
    if (!waveManager) return false;

    // 只检查同一行的僵尸
    auto zombiesInRow = waveManager->getZombiesInRow(_row);

    Vec2 minePos = this->getPosition();
    float range = _triggerRange;  // 使用触发范围进行检测

    for (auto zombie : zombiesInRow)
    {
        if (!zombie || !zombie->isAlive()) continue;

        Vec2 zombiePos = zombie->getPosition();
        float distance = minePos.distance(zombiePos);

        // 检查僵尸是否在地雷的触发范围内
        if (distance <= range)
        {
            log("PotatoMine: Zombie detected in trigger range at distance %.1f (range: %.1f)", distance, range);
            return true;
        }
    }

    return false;
}

void PotatoMine::createMineEffect()
{
    // 创建爆炸粒子效果
    auto particle = ParticleExplosion::create();
    if (particle)
    {
        particle->setPosition(this->getPosition());
        particle->setDuration(0.4f);
        particle->setSpeed(200.0f);
        particle->setSpeedVar(50.0f);
        particle->setLife(0.4f);
        particle->setLifeVar(0.1f);
        particle->setStartColor(Color4F(1.0f, 0.6f, 0.1f, 1.0f));
        particle->setEndColor(Color4F(1.0f, 0.9f, 0.2f, 0.0f));
        particle->setTotalParticles(40);

        this->getParent()->addChild(particle, 10);

        particle->runAction(Sequence::create(
            DelayTime::create(0.8f),
            RemoveSelf::create(),
            nullptr
        ));
    }

    // 创建冲击波，基于爆炸范围
    auto shockwave = Sprite::create();
    if (shockwave)
    {
        shockwave->setPosition(this->getPosition());
        shockwave->setScale(0.1f);
        shockwave->setOpacity(200);

        // 创建圆形纹理
        shockwave->setTextureRect(Rect(0, 0, 100, 100));
        shockwave->setColor(Color3B(255, 150, 0));

        this->getParent()->addChild(shockwave, 9);

        // 冲击波大小基于爆炸范围
        auto scaleAction = ScaleTo::create(0.3f, _explosionRange / 50.0f * 2.0f);
        auto fadeAction = FadeOut::create(0.3f);

        shockwave->runAction(Sequence::create(
            Spawn::create(scaleAction, fadeAction, nullptr),
            RemoveSelf::create(),
            nullptr
        ));
    }
}

void PotatoMine::playArmingSound()
{
    auto audioManager = AudioManager::getInstance();
    if (audioManager)
    {
        audioManager->playSoundEffect("Sounds/SFX/plant_planted.mp3");
    }
}

void PotatoMine::playExplosionSound()
{
    auto audioManager = AudioManager::getInstance();
    if (audioManager)
    {
        audioManager->playSoundEffect("Sounds/SFX/explosion.mp3");
    }
}

void PotatoMine::onExplosionComplete()
{
    // 爆炸动画完成后移除植物
    //this->removeFromParent();
    this->setVisible(false);
    _state = PlantState::DEAD;
    onDead();
}

void PotatoMine::onDead()
{
    this->_state = PlantState::DEAD;
    log("PotatoMine: Explosion complete, plant removed");
}