// Plant.h - 完整文件
#ifndef __PLANT_H__
#define __PLANT_H__

#include "cocos2d.h"

class AnimationHelper;
// 植物类型枚举
enum class PlantType {
    SUNFLOWER,      // 向日葵 - 资源型
    PEASHOOTER,     // 豌豆射手 - 攻击型
    WALLNUT,        // 坚果墙 - 防御型
    CHERRY_BOMB,    // 樱桃炸弹 - 攻击型
    SNOW_PEA,       // 寒冰射手 - 攻击型
    POTATO_MINE     // 土豆地雷 - 攻击型
};

// 植物状态
enum class PlantState {
    IDLE,           // 闲置
    ATTACKING,      // 攻击中
    PRODUCING,      // 生产中（向日葵）
    DAMAGED,        // 受伤
    DYING,          // 死亡中
    DEAD            // 死亡
};

class ResourceLoader;
class AnimationHelper;
class PNGAnimationHelper;

class Plant : public cocos2d::Sprite {
public:
    CREATE_FUNC(Plant);

    virtual bool init() override;
    void setupFallbackAnimation();
    virtual void update(float delta) override;

    // 初始化植物
    bool initPlant(PlantType type, int sunCost, float cooldown, int health);

    // 植物属性
    PlantType getType() const { return _type; }
    int getSunCost() const { return _sunCost; }
    float getCooldown() const { return _cooldown; }
    int getHealth() const { return _health; }
    int getMaxHealth() const { return _maxHealth; }
    PlantState getState() const { return _state; }
    int getRow() const { return _row; }
    int getCol() const { return _col; }

    // 设置位置
    void setGridPosition(int row, int col);

    // 植物行为
    virtual void takeDamage(int damage);
    virtual void attack(float delta);          // 攻击型植物使用
    virtual void produceSun();                 // 资源型植物使用
    virtual void die();

    // 状态检查
    bool isAlive() const { return _state != PlantState::DEAD && _state != PlantState::DYING; }
    bool canAttack() const { return _canAttack; }

    // 设置资源加载器
    void setResourceLoader(ResourceLoader* loader);

    // 设置动画辅助器
    void setAnimationHelper(AnimationHelper* helper);

    // 播放动画
    void playAnimation(const std::string& animationName, bool loop = false,
        const std::function<void()>& callback = nullptr);

    void checkAndPlayIdleAnimation();

    // 停止当前动画
    void stopCurrentAnimation();

    // 设置动画名称
    void setIdleAnimation(const std::string& name) { _idleAnimationName = name; }
    void setAttackAnimation(const std::string& name) { _attackAnimationName = name; }
    void setDamagedAnimation(const std::string& name) { _damagedAnimationName = name; }
    void setDyingAnimation(const std::string& name) { _dyingAnimationName = name; }
    void setProduceAnimation(const std::string& name) { _produceAnimationName = name; }

protected:
    // 子类可重写的函数
    virtual void onIdle(float delta) {}
    virtual void onAttack(float delta) {}
    virtual void onProduce(float delta) {}
    virtual void onDamaged() {}
    virtual void onDying() {}
    virtual void onDead() {}

    // 动画完成回调
    void onDyingComplete();

protected:
    PlantType _type;
    PlantState _state;

    // 基本属性
    int _sunCost;
    float _cooldown;
    int _health;
    int _maxHealth;

    // 攻击属性
    bool _canAttack;
    float _attackRange;
    float _attackSpeed;
    int _attackDamage;
    float _attackTimer;

    // 生产属性（向日葵）
    bool _canProduce;
    float _produceInterval;
    int _sunProduceAmount;
    float _produceTimer;

    // 位置
    int _row;
    int _col;

    // 动画相关
    std::string _idleAnimationName;
    std::string _attackAnimationName;
    std::string _damagedAnimationName;
    std::string _dyingAnimationName;
    std::string _produceAnimationName;

    // 动画动作
    cocos2d::Animate* _currentAnimate;

    // 资源加载器引用
    ResourceLoader* _resourceLoader;

    // 动画辅助类引用
    AnimationHelper* _animationHelper;
};

#endif // __PLANT_H__