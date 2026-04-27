#pragma once
#include "Component.h"

// Tag component — no data. Added to a player entity when they win and enter spectator mode.
class SpectatorComponent : public Component
{
public:
    const std::type_index GetType() override;
};
