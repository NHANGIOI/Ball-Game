#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>
#include "EquationSolver.hpp"
#include "Vector2DCalc.hpp"

#define fi first
#define se second
#define pff pair<float,float>
#define ll long long
using Vecf = sf::Vector2f;
std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
ll rnd(ll l,ll r){
    return l + rng() % (r - l + 1);
}
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
struct Picture{
    sf::Texture pic;
    void Load(std::string file_target,bool smooth_ok){
        if(pic.loadFromFile(file_target) == false){
            throw std::runtime_error("Cannot Load file main_menu_background");
        }
        pic.setSmooth(smooth_ok);
    }
    sf::Sprite draw(Vecf pos){
        sf::Sprite sp(pic);
        sp.setPosition(pos);
        return sp;
    }
};
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
    sf::Color Color_code;
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
            windows.draw(draw::Circle(Vecf(x,y),R,3000,Color_code));
            reflex();
            //std::cout << "BALL : " << x << " " << y << " : " << u.x << " " << u.y << std::endl;
            //std::cout << len(u) << std::endl;
            x += u.x;
            y += u.y;
        }
};
//---------------------------//
unsigned int n = 800,m = 600;
int limit_frame = 60;
sf::View view;
namespace main_menu{
    Picture pic_main_menu;
    Ball menu_ball[5];

    sf::Texture tx_single_button;
    sf::Texture tx_duel_button;
    sf::Texture tx_setting_button;

    void pre_process(){
        pic_main_menu.Load("Texture/main_menu_background.jpg",true);
        if(tx_single_button.loadFromFile("Texture/Khung_Single_Button.png") == false){//nạp ảnh khung single button
            throw std::runtime_error("Cannot Load file Khung_Single_Button");
        }
        tx_single_button.setSmooth(true);

        if(tx_duel_button.loadFromFile("Texture/Khung_Duel_Button.png") == false){//nạp ảnh khung duel button
            throw std::runtime_error("Cannot Load file Khung_Duel_Button");
        }
        tx_duel_button.setSmooth(true);

        if(tx_setting_button.loadFromFile("Texture/Khung_Setting_Button.png") == false){//nạp ảnh khung Setting button
            throw std::runtime_error("Cannot Load file Khung_Setting_Button");
        }
        tx_setting_button.setSmooth(true);

        //pre_process menu_ball
        for(int i = 0;i < 5;++i){
            menu_ball[i].x = rnd(0.f,(float)n);
            menu_ball[i].y = rnd(0.f,(float)m);
            menu_ball[i].R = rnd(10,20);
            menu_ball[i].u = Vecf((float)(rnd(0,1) == 1 ? 1 : -1) * rnd(200,300),(float)(rnd(0,1) == 1 ? 1 : -1) * rnd(200,300)) / limit_frame;
            menu_ball[i].Color_code = sf::Color(rnd(200,255),rnd(10,255),rnd(20,255));
            
            pt_doan_t tmp;

            tmp.A = Vecf((float)n,0.0);
            tmp.B = Vecf(0.0,0.0);
            tmp.cre();
            menu_ball[i].lines.push_back(tmp);

            tmp.A = Vecf((float)n,0.0);
            tmp.B = Vecf((float)n,(float)m);
            tmp.cre();
            menu_ball[i].lines.push_back(tmp);

            tmp.A = Vecf(0.0,(float)m);
            tmp.B = Vecf((float)n,(float)m);
            tmp.cre();
            menu_ball[i].lines.push_back(tmp);
        
            tmp.A = Vecf(0.0,(float)m);
            tmp.B = Vecf(0.0,0.0);
            tmp.cre();
            menu_ball[i].lines.push_back(tmp);
            //--------------------------------//
        }
    }
    bool limit_single_button(float mx,float my){
        if(mx < 280 || mx > 520 || my < 295 || my > 345)    return false;
        return true;
    }
    void draw_single_button(float mx,float my,sf::RenderWindow &windows){
        sf::Sprite button(tx_single_button);
        button.scale(Vecf(0.1138245033f,0.095087163f));
        button.setPosition(Vecf(262.5f,290));
        windows.draw(button);

        sf::Text docu(Consolas);
        docu.setString("Single Play");
        docu.setCharacterSize(35);
        docu.setFillColor((limit_single_button(mx,my) ? sf::Color(0,128,0) : sf::Color::Red));
        docu.setPosition(Vecf(295,295));
        windows.draw(docu);
    }
    bool limit_duel_button(float mx,float my){
        if(mx < 280 || mx > 520 || my < 395 || my > 445)    return false;
        return true;
    }
    void draw_duel_button(float mx,float my,sf::RenderWindow &windows){
        sf::Sprite button(tx_duel_button);
        button.scale(Vecf(0.1135425268f,0.095087163f));
        button.setPosition(Vecf(262.5f,390));
        windows.draw(button);

        sf::Text docu(Consolas);
        docu.setString("Duel Play");
        docu.setCharacterSize(35);
        docu.setFillColor((limit_duel_button(mx,my) ? sf::Color(0,128,0) : sf::Color::Blue));
        docu.setPosition(Vecf(315,396));
        windows.draw(docu);
    }
    bool limit_setting_button(float mx,float my){
        if(mx < 280 || mx > 520 || my < 495 || my > 540)    return false;
        return true;
    }
    void draw_setting_button(float mx,float my,sf::RenderWindow &windows){
        sf::Sprite button(tx_setting_button);
        button.scale(Vecf(0.11377741f,0.094488188f));
        button.setPosition(Vecf(262.5f,490));
        windows.draw(button);

        sf::Text docu(Consolas);
        docu.setString("Setting");
        docu.setCharacterSize(35);
        docu.setFillColor((limit_setting_button(mx,my) ? sf::Color(0,128,0) : sf::Color::White));
        docu.setPosition(Vecf(333,495));
        windows.draw(docu);
    }
    void main_process(sf::RenderWindow &windows){
        windows.draw(pic_main_menu.draw(Vecf(0.f,0.f)));

        windows.draw(draw::text("Ball Game",Vecf(215,100),Consolas,75,sf::Color(103, 199, 99),""));
        for(int i = 0;i < 5;++i){
            menu_ball[i].main_process(windows);
        }
        Vecf rpos = windows.mapPixelToCoords(sf::Mouse::getPosition(windows),view);
        float mx = rpos.x;
        float my = rpos.y; 
        draw_single_button(mx,my,windows);
        draw_duel_button(mx,my,windows);
        draw_setting_button(mx,my,windows);
    }
}
void pre_process(){
    if(Consolas.openFromFile("Fonts/Consolas-Regular.ttf") == false){
        exit(-1);
    }
    main_menu::pre_process();
}
signed main()
{
    sf::RenderWindow windows(sf::VideoMode({n, m}), "Ball-Game");
    windows.setFramerateLimit(limit_frame);

    view.setSize(sf::Vector2f(n,m));
    view.setCenter({n / 2.f,m / 2.f});
    windows.setView(view);

    pre_process();
    
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
            if(auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()){
                float Mx = mousePressed->position.x;
                float My = mousePressed->position.y;
                Vecf rpos = windows.mapPixelToCoords(sf::Mouse::getPosition(windows),view);
                float mx = rpos.x;
                float my = rpos.y; 
                std::cout << Mx << " " << My << std::endl;
            }

        }
        windows.clear(sf::Color::Black);
        windows.setView(view);

        main_menu::main_process(windows);

        windows.display();
    }
}