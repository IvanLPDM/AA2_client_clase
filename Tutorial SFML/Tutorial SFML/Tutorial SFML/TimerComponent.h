#pragma once
#include "Component.h"

class TimerComponent : public Component
{
public:
    explicit TimerComponent(float durationSeconds);

    float GetRemaining() const;
    bool HasExpired() const;
    void Tick(float deltaTime);
    void Reset();

    const std::type_index GetType() override;

private:
    float _duration;
    float _remaining;
};
