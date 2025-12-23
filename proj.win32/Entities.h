#include <cocos/editor-support/cocostudio/CCSkin.h>
// Entity.h - 所有游戏实体的基类
class Entity : public cocos2d::Sprite {
public:
    virtual bool init() override;
    virtual void update(float delta) override;

    // 通用属性
    CC_SYNTHESIZE(int, _health, Health);
    CC_SYNTHESIZE(int, _maxHealth, MaxHealth);
    CC_SYNTHESIZE(float, _attackDamage, AttackDamage);
    CC_SYNTHESIZE(float, _attackSpeed, AttackSpeed);

protected:
    virtual void onDie();
};