#pragma once
#ifndef __GRID_SYSTEM_H__
#define __GRID_SYSTEM_H__

#include "cocos2d.h"
#include <vector>
#include <functional>

class Plant; // 前向声明

// 格子结构体
struct GridCell {
    int row;
    int col;
    bool isOccupied;
    Plant* plant;
    cocos2d::Rect rect;

    GridCell() : row(0), col(0), isOccupied(false), plant(nullptr) {}
};

// 格子点击回调类型
typedef std::function<void(int, int, const cocos2d::Vec2&)> GridClickCallback;

class GridSystem {
public:
    static GridSystem* getInstance();

    // 初始化格子系统
    bool init(int rows, int cols, const cocos2d::Size& cellSize,
        const cocos2d::Vec2& gridOrigin);

    // 获取格子信息
    int getRows() const { return _rows; }
    int getCols() const { return _cols; }
    cocos2d::Size getCellSize() const { return _cellSize; }
    cocos2d::Vec2 getGridOrigin() const { return _gridOrigin; }

    // 坐标转换
    bool worldToGrid(const cocos2d::Vec2& worldPos, int& outRow, int& outCol) const;
    cocos2d::Vec2 gridToWorld(int row, int col) const;
    cocos2d::Vec2 gridToWorldCenter(int row, int col) const;

    // 格子操作
    bool isGridOccupied(int row, int col) const;
    bool canPlantAt(int row, int col) const;
    bool plantAt(Plant* plant, int row, int col);
    void removePlant(int row, int col);
    Plant* getPlantAt(int row, int col) const;

    // 获取所有格子
    const std::vector<std::vector<GridCell>>& getGrid() const { return _grid; }

    // 点击检测
    void handleTouch(const cocos2d::Vec2& touchPos);

    // 设置点击回调
    void setClickCallback(const GridClickCallback& callback) { _clickCallback = callback; }

    // 绘制调试网格
    void drawDebugGrid(cocos2d::Node* parent);

    // 清除所有格子
    void clearAll();

    // 重置网格系统
    void reset();

    const static int GRID_WIDTH = 90;
    const static int GRID_HEIGHT = 90;

private:
    GridSystem();
    ~GridSystem();

    // 禁止拷贝
    GridSystem(const GridSystem&) = delete;
    GridSystem& operator=(const GridSystem&) = delete;

    bool isValidGrid(int row, int col) const;

private:
    static GridSystem* _instance;

    // 格子参数
    int _rows;
    int _cols;
    cocos2d::Size _cellSize;
    cocos2d::Vec2 _gridOrigin;  // 网格起始点（左下角）

    // 格子数据
    std::vector<std::vector<GridCell>> _grid;

    // 点击回调
    GridClickCallback _clickCallback;

    // 调试节点
    cocos2d::DrawNode* _debugNode;
};

#endif // __GRID_SYSTEM_H__