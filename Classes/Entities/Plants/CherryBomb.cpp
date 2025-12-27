#include "CherryBomb.h"
#include "./Game/GameManager.h"
#include "./Resources/AudioManager.h"
#include "./Resources/ResourceLoader.h"
#include <Entities/Zombie/Zombie.h>  // 假设有僵尸类

USING_NS_CC;

bool CherryBomb::init()
{
    if (!Plant::init())
    {
        return false;
    }

    // 初始化樱桃炸弹特有属性
    initPlant(PlantType::CHERRY_BOMB, 150, 50.0f, 100);

    // 设置爆炸属性
    _explosionDelay = 1.5f;     // 1.5秒后爆炸
    _explosionRange = 150.0f;   // 爆炸范围150像素
    _explosionDamage = 1800;    // 爆炸伤害，足以秒杀所有僵尸
    _isExploding = false;
    _explosionTimer = 0.0f;

    // 设置动画名称
    setIdleAnimation("cherrybomb_idle");
    setDyingAnimation("cherrybomb_explode");

    // 播放初始动画
    checkAndPlayIdleAnimation();

    log("CherryBomb: Initialized with explosion delay: %.1f, damage: %d",
        _explosionDelay, _explosionDamage);

    return true;
}

void CherryBomb::onIdle(float delta)
{
    if (!_isExploding)
    {
        _explosionTimer += delta;

        // 检查是否需要爆炸
        if (_explosionTimer >= _explosionDelay)
        {
            _isExploding = true;
            _state = PlantState::DYING;
            explode();
        }
        else
        {
            // 闪烁效果提示即将爆炸（最后0.5秒闪烁）
            float timeLeft = _explosionDelay - _explosionTimer;
            if (timeLeft <= 0.5f)
            {
                // 每0.1秒闪烁一次
                static float flashTimer = 0.0f;
                flashTimer += delta;
                if (flashTimer >= 0.1f)
                {
                    flashTimer = 0.0f;
                    auto tintAction = TintTo::create(0.05f, Color3B::RED);
                    auto reverseAction = TintTo::create(0.05f, Color3B::WHITE);
                    this->runAction(Sequence::create(tintAction, reverseAction, nullptr));
                }
            }
        }
    }
}

void CherryBomb::explode()
{
    log("CherryBomb: Exploding!");

    // 播放爆炸动画
    playAnimation("cherrybomb_explode", false, [this]() {
        this->onExplodingComplete();
        });

    // 播放爆炸音效
    playExplosionSound();

    // 创建爆炸特效
    createExplosionEffect();

    // 计算爆炸范围
    Vec2 bombPos = this->getPosition();
    float rangeSquared = _explosionRange * _explosionRange;

    // 这里需要获取场景中的所有僵尸并检查距离
    // 注意：这需要与僵尸管理器或场景中的僵尸列表交互
    // 以下为示例代码，实际实现可能需要调整

    /*
    auto gameScene = dynamic_cast<GameScene*>(this->getParent()->getParent());
    if (gameScene)
    {
        auto& zombies = gameScene->getZombies(); // 假设GameScene有getZombies方法
        for (auto zombie : zombies)
        {
            if (zombie && zombie->isAlive())
            {
                Vec2 zombiePos = zombie->getPosition();
                float distanceSquared = bombPos.distanceSquared(zombiePos);

                if (distanceSquared <= rangeSquared)
                {
                    // 对僵尸造成伤害
                    zombie->takeDamage(_explosionDamage);
                    log("CherryBomb: Zombie hit with explosion damage: %d", _explosionDamage);
                }
            }
        }
    }
    */

    this->onDead();

    // 临时实现：输出日志
    log("CherryBomb: Explosion damage %d applied in range %.0f",
        _explosionDamage, _explosionRange);
}

void CherryBomb::createExplosionEffect()
{
    // 创建爆炸粒子效果
    auto particle = ParticleExplosion::create();
    if (particle)
    {
        particle->setPosition(this->getPosition());
        particle->setDuration(0.5f);
        particle->setSpeed(200.0f);
        particle->setSpeedVar(50.0f);
        particle->setLife(0.5f);
        particle->setLifeVar(0.2f);
        particle->setStartColor(Color4F(1.0f, 0.2f, 0.1f, 1.0f));
        particle->setEndColor(Color4F(1.0f, 0.8f, 0.1f, 0.0f));
        particle->setTotalParticles(50);

        this->getParent()->addChild(particle, 10);

        // 粒子效果自动移除
        particle->runAction(Sequence::create(
            DelayTime::create(1.0f),
            RemoveSelf::create(),
            nullptr
        ));
    }

    // 创建爆炸冲击波视觉效果
    auto shockwave = Sprite::create();
    if (shockwave)
    {
        shockwave->setPosition(this->getPosition());
        shockwave->setScale(0.1f);
        shockwave->setOpacity(150);

        // 使用简单的圆形纹理或颜色
        shockwave->setTextureRect(Rect(0, 0, 100, 100));
        shockwave->setColor(Color3B(255, 100, 0));

        this->getParent()->addChild(shockwave, 9);

        // 冲击波动画
        auto scaleAction = ScaleTo::create(0.3f, _explosionRange / 50.0f);
        auto fadeAction = FadeOut::create(0.3f);
        auto removeAction = RemoveSelf::create();

        shockwave->runAction(Sequence::create(
            Spawn::create(scaleAction, fadeAction, nullptr),
            removeAction,
            nullptr
        ));
    }
    
}

void CherryBomb::playExplosionSound()
{
    AudioManager::getInstance()->playSoundEffect(
        ResourceLoader::getInstance()->getSoundEffectPath("sound_cherrybomb")
    );
}

void CherryBomb::onExplodingComplete()
{
    // 爆炸动画完成后移除植物
    this->removeFromParent();
    _state = PlantState::DEAD;
    this->setVisible(false);
    onDead();
}

void  CherryBomb::onDead(){
    
    this->_state = PlantState::DEAD;
    log("CherryBomb: Explosion complete, plant removed");
}