#include "EquationSolver.hpp"
std::optional<std::pair<float,float>> solve2x2(
    float a1, float b1, float c1,
    float a2, float b2, float c2
){
    //a1x + b1y = c1
    //a2x + b2y = c2
    float D  = (float)a1*b2 - a2*b1;
    if (D == 0) {
        return std::nullopt; // vô nghiệm hoặc vô số nghiệm
    }
    float Dx = c1*b2 - c2*b1;
    float Dy = a1*c2 - a2*c1;
    return std::make_pair((float)Dx/D,(float)Dy/D);
}
