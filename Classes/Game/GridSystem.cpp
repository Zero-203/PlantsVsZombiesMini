#include "GridSystem.h"
#include "./Classes/Entities/Plants/Plant.h"

USING_NS_CC;

GridSystem* GridSystem::_instance = nullptr;

GridSystem* GridSystem::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new GridSystem();
    }
    return _instance;
}

GridSystem::GridSystem()
    : _rows(0)
    , _cols(0)
    , _debugNode(nullptr)
{
}

GridSystem::~GridSystem()
    {
    _instance = nullptr;
    }

bool GridSystem::init(int rows, int cols, const Size& cellSize, const Vec2& gridOrigin)
{
    if (rows <= 0 || cols <= 0 || cellSize.width <= 0 || cellSize.height <= 0)
    {
        log("GridSystem: Invalid parameters");
        return false;
    }

    _rows = rows;
    _cols = cols;
    _cellSize = cellSize;
    _gridOrigin = gridOrigin;

    // 初始化格子
    _grid.resize(rows);
    for (int i = 0; i < rows; i++)
    {
        _grid[i].resize(cols);
        for (int j = 0; j < cols; j++)
        {
            _grid[i][j].row = i;
            _grid[i][j].col = j;
            _grid[i][j].isOccupied = false;
            _grid[i][j].plant = nullptr;

            // 计算格子矩形区域
            float x = _gridOrigin.x + j * _cellSize.width;
            float y = _gridOrigin.y + i * _cellSize.height;
            _grid[i][j].rect = Rect(x, y, _cellSize.width, _cellSize.height);
        }
    }

    log("GridSystem: Initialized %d rows x %d cols", rows, cols);
    return true;
}

bool GridSystem::isValidGrid(int row, int col) const
{
    return row >= 0 && row < _rows&& col >= 0 && col < _cols;
}

bool GridSystem::worldToGrid(const Vec2& worldPos, int& outRow, int& outCol) const
{
    // 计算相对于网格原点的位置
    float relX = worldPos.x - _gridOrigin.x;
    float relY = worldPos.y - _gridOrigin.y;

    // 检查是否在网格范围内
    if (relX < 0 || relX >= _cols * _cellSize.width ||
        relY < 0 || relY >= _rows * _cellSize.height)
    {
        return false;
    }

    // 计算行列
    outCol = static_cast<int>(relX / _cellSize.width);
    outRow = static_cast<int>(relY / _cellSize.height);

    // 确保在有效范围内
    if (!isValidGrid(outRow, outCol))
    {
        return false;
    }

    return true;
}

Vec2 GridSystem::gridToWorld(int row, int col) const
{
    if (!isValidGrid(row, col))
    {
        return Vec2::ZERO;
    }

    float x = _gridOrigin.x + col * _cellSize.width;
    float y = _gridOrigin.y + row * _cellSize.height;

    return Vec2(x, y);
}

Vec2 GridSystem::gridToWorldCenter(int row, int col) const
{
    Vec2 worldPos = gridToWorld(row, col);
    if (worldPos == Vec2::ZERO)
    {
        return Vec2::ZERO;
    }

    // 返回格子中心点
    return Vec2(worldPos.x + _cellSize.width / 2,
        worldPos.y + _cellSize.height * 1 / 3);
}

bool GridSystem::isGridOccupied(int row, int col) const
{
    if (!isValidGrid(row, col))
    {
        return true; // 无效格子视为已占用
    }

    return _grid[row][col].isOccupied;
}

bool GridSystem::canPlantAt(int row, int col) const
{
    return isValidGrid(row, col) && !isGridOccupied(row, col);
}

bool GridSystem::plantAt(Plant* plant, int row, int col)
{
    if (!plant || !canPlantAt(row, col))
    {
        log("GridSystem: Cannot plant at row %d, col %d", row, col);
        return false;
    }

    _grid[row][col].isOccupied = true;
    _grid[row][col].plant = plant;

    log("GridSystem: Plant placed at row %d, col %d", row, col);
    return true;
}

void GridSystem::removePlant(int row, int col)
{
    if (!isValidGrid(row, col))
    {
        return;
    }

    _grid[row][col].isOccupied = false;
    _grid[row][col].plant = nullptr;

    log("GridSystem: Plant removed from row %d, col %d", row, col);
}

Plant* GridSystem::getPlantAt(int row, int col) const
{
    if (!isValidGrid(row, col))
    {
        return nullptr;
    }

    return _grid[row][col].plant;
}

void GridSystem::handleTouch(const Vec2& touchPos)
{
    int row, col;
    if (worldToGrid(touchPos, row, col))
    {
        // 调用回调函数
        if (_clickCallback)
        {
            _clickCallback(row, col, gridToWorldCenter(row, col));
        }

        log("GridSystem: Grid clicked at row %d, col %d", row, col);
    }
}

void GridSystem::reset()
{
    // 移除调试节点
    if (_debugNode)
    {
        _debugNode->removeFromParent();
        _debugNode = nullptr;
    }

    // 重置网格数据
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            removePlant(i, j);
        }
    }

    // 重置点击回调
    _clickCallback = nullptr;

    log("GridSystem: Reset completed");
}

void GridSystem::drawDebugGrid(Node* parent)
{
    if (!parent)
    {
        return;
    }

    
    /*
    // 移除旧的调试节点
    if (_debugNode && _debugNode->getParent())
    {
        _debugNode->removeFromParent();
    }
    
    _debugNode = DrawNode::create();
    parent->addChild(_debugNode, 9999); // 最高层级显示

    // 设置网格颜色
    Color4F gridColor(1.0f, 1.0f, 0.0f, 0.5f); // 黄色半透明

    // 绘制所有格子
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
{
            const Rect& rect = _grid[i][j].rect;

            // 绘制格子边框
            Vec2 vertices[] = {
                Vec2(rect.origin.x, rect.origin.y),
                Vec2(rect.origin.x + rect.size.width, rect.origin.y),
                Vec2(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height),
                Vec2(rect.origin.x, rect.origin.y + rect.size.height)
            };

            _debugNode->drawPoly(vertices, 4, true, gridColor);

            // 如果格子被占用，绘制填充
            if (_grid[i][j].isOccupied)
    {
                Color4F occupiedColor(1.0f, 0.0f, 0.0f, 0.2f); // 红色半透明
                _debugNode->drawSolidRect(rect.origin,
                    Vec2(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height),
                    occupiedColor);
            }

            // 显示行列编号
            std::string gridText = StringUtils::format("%d,%d", i, j);
            auto label = Label::createWithTTF(gridText, "fonts/Marker Felt.ttf", 12);
            label->setPosition(rect.origin.x + rect.size.width / 2,
                rect.origin.y + rect.size.height / 2);
            label->setColor(Color3B::WHITE);
            _debugNode->addChild(label);
        }
    }

    log("GridSystem: Debug grid drawn");
    */
}

void GridSystem::clearAll()
{
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
    {
            removePlant(i, j);
        }
    }

    log("GridSystem: All grids cleared");
}