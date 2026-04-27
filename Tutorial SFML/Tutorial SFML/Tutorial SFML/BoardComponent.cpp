#include "BoardComponent.h"

BoardComponent::BoardComponent()
{
    _cells.fill(CellState::EMPTY);
}

CellState BoardComponent::GetCell(int row, int col) const
{
    return _cells[row * COLS + col];
}

void BoardComponent::SetCell(int row, int col, CellState state)
{
    _cells[row * COLS + col] = state;
}

bool BoardComponent::IsCellEmpty(int row, int col) const
{
    return _cells[row * COLS + col] == CellState::EMPTY;
}

bool BoardComponent::IsFull() const
{
    for (const auto& cell : _cells)
    {
        if (cell == CellState::EMPTY) return false;
    }
    return true;
}

const std::type_index BoardComponent::GetType()
{
    return typeid(BoardComponent);
}
