#include "GridSystem.h"
USING_NS_CC;

// 初始化网格大小（适配屏幕）
const float GridSystem::GRID_WIDTH = Director::getInstance()->getVisibleSize().width / COL_COUNT;
const float GridSystem::GRID_HEIGHT = Director::getInstance()->getVisibleSize().height / (ROW_COUNT + 2); // 预留顶部HUD空间

GridSystem* GridSystem::create()
{
    auto ret = new GridSystem();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GridSystem::init()
{
    if (!Node::init()) return false;

    // 初始化网格
    _grid.resize(ROW_COUNT);
    for (int i = 0; i < ROW_COUNT; ++i)
    {
        _grid[i].resize(COL_COUNT, nullptr);
    }

    // 计算网格起始位置（底部居中）
    auto visibleSize = Director::getInstance()->getVisibleSize();
    _gridOrigin.x = (visibleSize.width - COL_COUNT * GRID_WIDTH) / 2;
    _gridOrigin.y = (visibleSize.height - ROW_COUNT * GRID_HEIGHT) / 2;

    // 绘制网格（调试用，发布时可删除）
    drawGrid();

    return true;
}

bool GridSystem::plantAt(Plant* plant, int row, int col)
{
    if (!plant || row < 0 || row >= ROW_COUNT || col < 0 || col >= COL_COUNT)
        return false;

    if (_grid[row][col] != nullptr)
        return false; // 格子已被占用

    _grid[row][col] = plant;
    // 设置植物位置为格子中心
    float x = _gridOrigin.x + col * GRID_WIDTH + GRID_WIDTH / 2;
    float y = _gridOrigin.y + row * GRID_HEIGHT + GRID_HEIGHT / 2;
    plant->setPosition(Vec2(x, y));
    this->addChild(plant, 2);
    return true;
}

Plant* GridSystem::getPlantAt(int row, int col)
{
    if (row < 0 || row >= ROW_COUNT || col < 0 || col >= COL_COUNT)
        return nullptr;
    return _grid[row][col];
}

void GridSystem::removePlantAt(int row, int col)
{
    if (row < 0 || row >= ROW_COUNT || col < 0 || col >= COL_COUNT)
        return;
    if (_grid[row][col])
    {
        _grid[row][col]->removeFromParent();
        _grid[row][col] = nullptr;
    }
}

void GridSystem::drawGrid()
{
    auto drawNode = DrawNode::create();
    Color4F gridColor(1.0f, 1.0f, 1.0f, 0.3f); // 白色半透明

    // 绘制竖线
    for (int col = 0; col <= COL_COUNT; ++col)
    {
        float x = _gridOrigin.x + col * GRID_WIDTH;
        drawNode->drawLine(Vec2(x, _gridOrigin.y),
            Vec2(x, _gridOrigin.y + ROW_COUNT * GRID_HEIGHT),
            gridColor);
    }

    // 绘制横线
    for (int row = 0; row <= ROW_COUNT; ++row)
    {
        float y = _gridOrigin.y + row * GRID_HEIGHT;
        drawNode->drawLine(Vec2(_gridOrigin.x, y),
            Vec2(_gridOrigin.x + COL_COUNT * GRID_WIDTH, y),
            gridColor);
    }

    this->addChild(drawNode, 1);
}