#include "Animation.h"
#include <algorithm>

Animation::Animation(float speed)
	: value(0.0f)
	, speed(speed)
{
}

void Animation::Update(bool active)
{
    if (active)
        value += speed;
    else
        value -= speed;

    value = std::clamp(value, 0.0f, 1.0f);
}

float Animation::Get() const
{
	return value;
}
