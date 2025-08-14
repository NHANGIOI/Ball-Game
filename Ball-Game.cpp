#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#define fi first
#define se second
#define pff pair<float,float>
using Vecf = sf::Vector2f;

const int N = 100;
sf::Font Consolas;

namespace draw{
sf::RectangleShape reg(Vecf A,Vecf B,sf::Color Color_code){
    sf::RectangleShape res;
    res.setPosition(A);
    res.setSize(B - A);
    res.setFillColor(Color_code);
    return res;
}
sf::CircleShape Circle(Vecf I,float R,int pointcount,sf::Color Color_code){
    sf::CircleShape res;
    res.setPosition(sf::Vector2f(I.x + R,I.y + R));
    res.setRadius(R);
    res.setFillColor(Color_code);
    res.setPointCount(pointcount);
    return res;
}
sf::VertexArray line(Vecf A,Vecf B,float do_day,sf::Color Color_code){
    sf::Vector2f Vn = sf::Vector2f(B.y - A.y,A.x - B.x);
    float Q = (do_day / (float)std::sqrt(Vn.x * Vn.x + Vn.y * Vn.y)) / 2;
    sf::VertexArray res(sf::PrimitiveType::TriangleStrip,4);
    res[2].position = {A.x + (Vn * Q).x,A.y + (Vn * Q).y};
    res[0].position = {B.x + (Vn * Q).x,B.y + (Vn * Q).y};
    res[1].position = {B.x - (Vn * Q).x,B.y - (Vn * Q).y};
    res[3].position = {A.x - (Vn * Q).x,A.y - (Vn * Q).y};
    for(int i = 0;i < 4;++i)    res[i].color = Color_code;
    return res;
}
sf::Text text(std::string x,Vecf pos,const sf::Font &font,float sz,sf::Color Color_code,std::string type){
    sf::Text res(font);
    res.setString(x);
    res.setPosition(sf::Vector2f(pos.x,pos.y));
    res.setCharacterSize(sz);
    res.setFillColor(Color_code);
    if(type == "Bold")   res.setStyle(sf::Text::Bold);
    else if(type == "Underlined")  res.setStyle(sf::Text::Underlined);
    return res;
}
}
struct show{
void interative(std::optional<sf::Event> *event){

}
void draw(sf::RenderWindow &windows){

}
};
show screen[N];
int par[N];
std::vector<int> child[N];

unsigned int n = 800,m = 600;
void pre_process(){
    if(Consolas.openFromFile("Fonts/Consolas-Regular.ttf") == false){
        exit(-1);
    }
}
signed main()
{
    pre_process();
    sf::RenderWindow windows(sf::VideoMode({n, m}), "Ball-Game");
    sf::View view;
    view.setSize(sf::Vector2f(n,m));
    view.setCenter({n / 2.f,m / 2.f});
    windows.setView(view);
    while (windows.isOpen())
    {
        while (const std::optional event = windows.pollEvent())
        {
            if (event->is<sf::Event::Closed>()){
                windows.close();
            }
            else if(const sf::Event::Resized *resize = event->getIf<sf::Event::Resized>()){
                float windowsRatio = (float)resize->size.x / resize->size.y;
                float targetRatio = (float)n / m;
                if(windowsRatio > targetRatio){
                    const float scale = targetRatio / windowsRatio;
                    view.setViewport(sf::FloatRect({(1.f - scale) / 2.f,0.f},{scale,1.f}));
                }
                else{
                    const float scale = windowsRatio / targetRatio;
                    view.setViewport(sf::FloatRect({0.f,(1.f - scale) / 2.f},{1.f,scale}));
                }
            }

        }
        windows.clear(sf::Color::Black);
        windows.setView(view);

        
        windows.display();
    }
}