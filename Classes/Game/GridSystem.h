#ifndef GRID_SYSTEM_H
#define GRID_SYSTEM_H
#include "cocos2d.h"
#include "Plant/Plant.h"
USING_NS_CC;

class GridSystem : public Node
{
public:
    static GridSystem* create();
    bool init() override;

    // 网格配置（5行9列，适配屏幕）
    static const int ROW_COUNT = 5;
    static const int COL_COUNT = 9;
    static const float GRID_WIDTH;
    static const float GRID_HEIGHT;

    // 种植植物
    bool plantAt(Plant* plant, int row, int col);
    // 获取格子上的植物
    Plant* getPlantAt(int row, int col);
    // 移除植物
    void removePlantAt(int row, int col);

private:
    void drawGrid(); // 绘制网格（调试用）
    std::vector<std::vector<Plant*>> _grid; // 二维网格存储植物
    Vec2 _gridOrigin; // 网格起始位置
};

#endif // GRID_SYSTEM_H