#pragma once
#include <algorithm>

class Animation
{
public:
    Animation(float speed = 0.15f);

    void Update(bool active);

    float Get() const;

private:
    float value;
    float speed;
};
