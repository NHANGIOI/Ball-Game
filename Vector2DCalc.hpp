#pragma once
#include<SFML/System/Vector2.hpp>
#include<cmath>
float dot(sf::Vector2f A,sf::Vector2f B);
float cross(sf::Vector2f A,sf::Vector2f B);
float len(sf::Vector2f A);
float SQR(float x);
sf::Vector2f operator* (sf::Vector2f A,float x);
sf::Vector2f operator* (float x,sf::Vector2f A);
sf::Vector2f operator/ (sf::Vector2f A,float x);
sf::Vector2f operator/ (float x,sf::Vector2f A);