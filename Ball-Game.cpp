#include <iostream>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>
#include "EquationSolver.hpp"
#include "Vector2DCalc.hpp"

#define fi first
#define se second
#define pff pair<float,float>
using Vecf = sf::Vector2f;

const float INF = 1e9 + 7;
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
    res.setPosition(I - Vecf(R,R));
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

struct pt_doan_t{
    Vecf A,B;
    float a,b,c;
    void cre(){
        a = (B - A).y;
        b = (A - B).x;
        c = (float)-a * A.x - b * A.y;
    }
    float khoang_cach(Vecf C){
        Vecf AC,AB;
        AB = B - A;
        AC = C - A;
        if(len(AB) == 0.0)  return len(AC);
        float t = (float)dot(AB,AC) / (float)(dot(AB,AB));
        if(t < 0.0)   return    len(AC);
        if(t > 1.0) return len(C - B);
        float diss = a * C.x + b * C.y + c;
        if(diss < 0.0)  diss = -diss;
        diss = (float)diss / len(Vecf(a,b));
        return diss;
    }
    bool huong_vao(float x0,float y0,Vecf u){
        Vecf AB = B - A;
        //A.x + AB.x * t = x0 + u.x * t' <=> AB.x * t - u.x * t' = x0 - A.x;
        //A.y + AB.y * t = y0 + u.y * t' <=> AB.y * t - u.y * t' = y0 - A.y;
        std::optional<std::pair<float,float>> res = solve2x2(AB.x,-u.x,x0 - A.x,
                                                            AB.y,-u.y,y0 - A.y);
        if(res->se > 0.0){
            return true;
        }
        return false;
    }
};
struct Ball{
    float x,y;
    float R;
    Vecf u;
    std::vector<pt_doan_t> lines;
    private:
        void dieu_chinh_u(int id){
            Vecf n = sf::Vector2f(lines[id].B.y - lines[id].A.y, lines[id].A.x - lines[id].B.x);
            n = n / (float)len(n);
            u = u - 2.f * dot(u,n) * n;
        }
        void reflex(){
            for(int i = 0;i < (int)lines.size();++i){
                if((float)lines[i].khoang_cach(Vecf(x,y)) - (float)R > 0 || lines[i].huong_vao(x,y,u) == false){
                    continue;
                }
                dieu_chinh_u(i);
            }
        }
    public:
        void main_process(sf::RenderWindow &windows){
            windows.draw(draw::Circle(Vecf(x,y),R,3000,sf::Color::Red));
            reflex();
            //std::cout << "BALL : " << x << " " << y << " : " << u.x << " " << u.y << std::endl;
            std::cout << len(u) << std::endl;
            x += u.x;
            y += u.y;
        }
};

unsigned int n = 800,m = 600;
int limit_frame = 60;
void pre_process(){
    if(Consolas.openFromFile("Fonts/Consolas-Regular.ttf") == false){
        exit(-1);
    }
}
signed main()
{
    pre_process();
    sf::RenderWindow windows(sf::VideoMode({n, m}), "Ball-Game");
    windows.setFramerateLimit(limit_frame);

    sf::View view;
    view.setSize(sf::Vector2f(n,m));
    view.setCenter({n / 2.f,m / 2.f});
    windows.setView(view);

    Ball bong;
    bong.x = 100.f;
    bong.y = 100.f;
    bong.u = Vecf(125.f,155.f) / limit_frame;//chuẩn hóa
    bong.R = 10;

    pt_doan_t tmp;
    tmp.A = sf::Vector2f(0.0,0.f);
    tmp.B = sf::Vector2f(800.f,0.f);
    tmp.cre();
    bong.lines.push_back(tmp);

    tmp.A = sf::Vector2f(800.f,0.f);
    tmp.B = sf::Vector2f(800.f,600.f);
    tmp.cre();
    bong.lines.push_back(tmp);

    tmp.A = sf::Vector2f(0.f,600.f);
    tmp.B = sf::Vector2f(800.f,600.f);
    tmp.cre();
    bong.lines.push_back(tmp);

    tmp.A = sf::Vector2f(0.f,600.f);
    tmp.B = sf::Vector2f(0.f,0.f);
    tmp.cre();
    bong.lines.push_back(tmp);

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
            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) == true){
                float Mx = sf::Mouse::getPosition(windows).x;
                float My = sf::Mouse::getPosition(windows).y;
                std::cout << Mx << " " << My << std::endl;
            }

        }
        windows.clear(sf::Color::Black);
        windows.setView(view);
        
        bong.main_process(windows);
        
        windows.display();
    }
}