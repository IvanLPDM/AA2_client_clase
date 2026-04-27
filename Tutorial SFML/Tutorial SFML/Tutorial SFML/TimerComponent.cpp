#include "TimerComponent.h"

TimerComponent::TimerComponent(float durationSeconds) : _duration(durationSeconds), _remaining(durationSeconds)
{

}

float TimerComponent::GetRemaining() const { return _remaining; }

bool  TimerComponent::HasExpired() const   { return _remaining <= 0.f; }

void TimerComponent::Tick(float deltaTime)
{
    _remaining -= deltaTime;
    if (_remaining < 0.f) _remaining = 0.f;
}

void TimerComponent::Reset()
{
    _remaining = _duration;
}

const std::type_index TimerComponent::GetType()
{
    return typeid(TimerComponent);
}
