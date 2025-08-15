#include "Vector2DCalc.hpp"
float dot(sf::Vector2f A,sf::Vector2f B){ return (float)A.x * B.x + A.y * B.y;}
float cross(sf::Vector2f A,sf::Vector2f B){   return (float)A.x * B.y - A.y * B.x;}
float len(sf::Vector2f A){   return hypot(A.x,A.y);}
float SQR(float x){  return (float)x * x;}
sf::Vector2f operator* (sf::Vector2f A,float x){
    return sf::Vector2f((float)A.x * x,(float)A.y * x);
}
sf::Vector2f operator* (float x,sf::Vector2f A){
    return sf::Vector2f((float)A.x * x,(float)A.y * x);
}
sf::Vector2f operator/ (sf::Vector2f A,float x){
    return sf::Vector2f((float)A.x / x,(float)A.y / x);
}
sf::Vector2f operator/ (float x,sf::Vector2f A){
    return sf::Vector2f((float)A.x / x,(float)A.y / x);
}