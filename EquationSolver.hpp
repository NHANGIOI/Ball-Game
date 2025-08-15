#pragma once
#include <utility>
#include<optional>
// Trả về std::optional<std::pair<float,float>>
std::optional<std::pair<float,float>> solve2x2(
    float a1, float b1, float c1,
    float a2, float b2, float c2
);
