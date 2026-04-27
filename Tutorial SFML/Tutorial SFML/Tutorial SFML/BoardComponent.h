#pragma once
#include "Component.h"
#include "Enums.hpp"
#include <array>

class BoardComponent : public Component
{
public:
    static constexpr int ROWS  = 6;
    static constexpr int COLS  = 6;
    static constexpr int TOTAL = ROWS * COLS;

    BoardComponent();

    CellState GetCell(int row, int col) const;
    void SetCell(int row, int col, CellState state);
    bool IsCellEmpty(int row, int col) const;
    bool IsFull() const;

    const std::type_index GetType() override;

private:
    std::array<CellState, TOTAL> _cells;
};
