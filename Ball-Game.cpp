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
sf::Font Consolas, Agencyfb;
sf::Clock program_time;
float he_so_phan_xa = 3.5;
float hang_so_phan_xa = 0.5;
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
    bool pxtl = false;
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
    Vecf v;
    std::vector<pt_doan_t> lines;
    sf::Color Color_code;
    sf::Time pret; // độ lệch thời gian so với thời điểm trc đó
    private:
        bool ok = false;
        void dieu_chinh_u(int id){
            Vecf AB = sf::Vector2f(lines[id].B.x - lines[id].A.x,lines[id].B.y - lines[id].A.y);
            Vecf AI = sf::Vector2f(x - lines[id].A.x,y - lines[id].A.y);
            Vecf j = sf::Vector2f(AB.y,-AB.x) / len(sf::Vector2f(AB.y,-AB.x));
            if(lines[id].pxtl == true){
                float tile = std::min(dot(AI,AB) / len(AB),len(AB) - (float)dot(AI,AB) / len(AB));
                tile = tile / len(AB / 2.f);
                tile = he_so_phan_xa * tile + hang_so_phan_xa;
                Vecf v_ = v - (float)dot(v,j) * j - (float)tile * dot(v,j) * j;
                v = v_ * ((float)len(v) / len(v_));
            }
            else    v = v - 2.f * dot(v,j) * j;
        }
        void reflex(){
            for(int i = 0;i < (int)lines.size();++i){
                if((float)lines[i].khoang_cach(Vecf(x,y)) - (float)R > 0 || lines[i].huong_vao(x,y,v) == false){
                    continue;
                }
                dieu_chinh_u(i);
            }
        }
    public:
        void process(sf::RenderWindow &windows){
            windows.draw(draw::Circle(Vecf(x,y),R,3000,Color_code));
            if(ok == false){
                pret = program_time.getElapsedTime();
                ok = true;
            }
            else{
                reflex();
                x += (float)((double)v.x * (double)(program_time.getElapsedTime() - pret).asMilliseconds() / 1000.0);  
                y += (float)((double)v.y * (double)(program_time.getElapsedTime() - pret).asMilliseconds() / 1000.0);
                pret = program_time.getElapsedTime();
            }
        }
};
struct Player{
    std::string name;
    int point = 0;
    float x = 300,len = 200;
    float y = 0;
    int speed = 180;
    sf::Keyboard::Key trai,phai;
    sf::Time pret;
    private:
        bool ok = false;
    public:
        void enter_key_control(sf::Keyboard::Key lhs,sf::Keyboard::Key rhs){
            trai = lhs;
            phai = rhs;
        }
        void act(sf::Keyboard::Key inputs,int id_player,Ball &PlaysBall){
            float delta = (float)((double)(program_time.getElapsedTime() - pret).asMilliseconds() / (double)1000.0);
            if(inputs == trai)  x = std::max(160.f,x - (float)speed * delta) ;
            else if(inputs == phai) x = std::min(640.f - len,x + (float)speed * delta) ;
            PlaysBall.lines[id_player].A = sf::Vector2f(x,y);
            PlaysBall.lines[id_player].B = sf::Vector2f(x + len,y);
            PlaysBall.lines[id_player].cre();

            PlaysBall.lines[id_player + 1].A = sf::Vector2f(x,y);
            PlaysBall.lines[id_player + 1].B = sf::Vector2f(x,y + 25);
            PlaysBall.lines[id_player + 1].cre();

            PlaysBall.lines[id_player + 2].A = sf::Vector2f(x + len,y);
            PlaysBall.lines[id_player + 2].B = sf::Vector2f(x + len,y + 25);
            PlaysBall.lines[id_player + 2].cre();
            pret = program_time.getElapsedTime();
        }
        void process(sf::RenderWindow &windows){
            if(ok == false) pret = program_time.getElapsedTime();
            windows.draw(draw::reg(Vecf(x,y),Vecf(x + len,600),sf::Color::White));
        }

};
//---------------------------//
unsigned int n = 800,m = 600;
int fps_limit = 60;
sf::View view;
int id_screen = 1;
int par[N];
Player player1,player2;

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
            menu_ball[i].v = Vecf((float)(rnd(0,1) == 1 ? 1 : -1) * rnd(100,200),(float)(rnd(0,1) == 1 ? 1 : -1) * rnd(100,200));
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
    void interactive(const std::optional<sf::Event> &event,sf::RenderWindow &windows){
        if(auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()){
            sf::Vector2i ppos = sf::Vector2i(mousePressed->position.x,mousePressed->position.y);
            Vecf rpos = windows.mapPixelToCoords(ppos,view);
            float mx = rpos.x;
            float my = rpos.y; 
            std::cout << mx << " " << my << std::endl;

            if(main_menu::limit_setting_button(mx,my) == true){
                id_screen = 2;
                windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Arrow));
            }
            else if(main_menu::limit_single_button(mx,my) == true){
                id_screen = 3;
                windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Arrow));
            }
        }
    }
    void process(sf::RenderWindow &windows){
        windows.draw(pic_main_menu.draw(Vecf(0.f,0.f)));

        if(id_screen == 1)    windows.draw(draw::text("Ball Game",Vecf(215,100),Consolas,75,sf::Color(103, 199, 99),""));
        for(int i = 0;i < 5;++i){
            menu_ball[i].process(windows);
        }
        Vecf rpos = windows.mapPixelToCoords(sf::Mouse::getPosition(windows),view);
        float mx = rpos.x;
        float my = rpos.y; 
        if(id_screen == 1){
            if(limit_single_button(mx,my) || limit_duel_button(mx,my) || limit_setting_button(mx,my)){
                windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Hand));
            }
            else{
                windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Arrow));
            }
            draw_single_button(mx,my,windows);
            draw_duel_button(mx,my,windows);
            draw_setting_button(mx,my,windows);
        }
    }
}
namespace setting{
    sf::Texture tx_khung_setting;
    sf::Texture tx_Nut_Exit1, tx_Nut_Exit2;
    sf::Texture tx_khung_so_fps1, tx_khung_so_fps2;
    sf::Texture tx_khung_auto_fps1, tx_khung_auto_fps2;
    void pre_process(){
        if(tx_khung_setting.loadFromFile("Texture/Setting/Khung_setting.png") == false)     exit(-1);
        if(tx_Nut_Exit1.loadFromFile("Texture/Setting/Nut_Exit1.png") == false)     exit(-1);
        if(tx_Nut_Exit2.loadFromFile("Texture/Setting/Nut_Exit2.png") == false)     exit(-1);
        if(tx_khung_so_fps1.loadFromFile("Texture/Setting/Khung_so_fps1.png") == false)     exit(-1);
        if(tx_khung_so_fps2.loadFromFile("Texture/Setting/Khung_so_fps2.png") == false)     exit(-1);
        if(tx_khung_auto_fps1.loadFromFile("Texture/Setting/Khung_auto_fps1.png") == false)     exit(-1);
        if(tx_khung_auto_fps2.loadFromFile("Texture/Setting/Khung_auto_fps2.png") == false)     exit(-1);
        tx_khung_setting.setSmooth(true);
        tx_Nut_Exit1.setSmooth(true);
        tx_Nut_Exit2.setSmooth(true);
        tx_khung_so_fps1.setSmooth(true);
        tx_khung_so_fps2.setSmooth(true);
        tx_khung_auto_fps1.setSmooth(true);
        tx_khung_auto_fps2.setSmooth(true);
    }
    bool limit_Nut_Exit(float mx,float my){
        if(mx < 615.f || mx > 660.f || my < 110.f || my > 155.f)    return false;
        return true;
    }
    namespace fps_part{
        bool limit_enter_custom_fpt_reg(float mx,float my){
            if(mx < 205 || mx > 310 || my < 375 || my > 400)    return false;
            return true;
        }
        bool limit_45_fps_button(float mx,float my){
            if(mx < 180 || mx > 225 || my < 175 || my > 220)    return false;
            return true;
        }
        bool limit_60_fps_button(float mx,float my){
            if(mx < 285 || mx > 330 || my < 175 || my > 220)    return false;
            return true;
        }
        bool limit_75_fps_button(float mx,float my){
            if(mx < 175 || mx > 225 || my < 250 || my > 300)    return false;
            return true;
        }
        bool limit_auto_fps_button(float mx,float my){
            if(mx < 275 || mx > 340 || my < 260 || my > 300)    return false;
            return true;
        }
        void draw(float mx,float my,sf::RenderWindow &windows){
            //draw khung fps
            sf::Sprite khung_so_fps1(tx_khung_so_fps1);
            khung_so_fps1.scale(Vecf(0.1147959184f,0.1147959184f));
            sf::Sprite khung_so_fps2(tx_khung_so_fps2);
            khung_so_fps2.scale(Vecf(0.1147959184f,0.1147959184f));

            khung_so_fps1.setPosition(Vecf(179,176));
            khung_so_fps2.setPosition(Vecf(179,176));
            if(fps_limit == 45) windows.draw(khung_so_fps2);
            else    windows.draw(khung_so_fps1);

            khung_so_fps1.setPosition(Vecf(287,176));
            khung_so_fps2.setPosition(Vecf(287,176));
            if(fps_limit == 60) windows.draw(khung_so_fps2);
            else    windows.draw(khung_so_fps1);

            khung_so_fps1.setPosition(Vecf(179,256));
            khung_so_fps2.setPosition(Vecf(179,256));
            if(fps_limit == 75) windows.draw(khung_so_fps2);
            else    windows.draw(khung_so_fps1);

            sf::Sprite khung_auto_fps1(tx_khung_auto_fps1),khung_auto_fps2(tx_khung_auto_fps2);
            khung_auto_fps1.scale(Vecf(0.150462963f,0.2083333333f));
            khung_auto_fps2.scale(Vecf(0.150462963f,0.2083333333f));
            khung_auto_fps1.setPosition(Vecf(277,261));
            khung_auto_fps2.setPosition(Vecf(277,261));
            if(fps_limit == -1) windows.draw(khung_auto_fps2);
            else    windows.draw(khung_auto_fps1);
            //-------------------------------------------------------//
            windows.draw(draw::text("45",Vecf(187,180),Agencyfb,32,sf::Color::White,""));
            windows.draw(draw::text("60",Vecf(295,180),Agencyfb,32,sf::Color::White,""));
            windows.draw(draw::text("75",Vecf(187,260),Agencyfb,32,sf::Color::White,""));
            windows.draw(draw::text("auto",Vecf(285,258),Agencyfb,32,sf::Color::White,""));
        }
    }
    namespace enter_name_part{
        bool ok_enter = false;
        bool limit_enter_name(float mx,float my){
            if(mx < 442 || mx > 628 || my < 430 || my > 455)    return false;
            return true;
        }
        void draw(sf::RenderWindow &windows){
            windows.draw(draw::text(player1.name + (ok_enter ? "|" : ""),Vecf(448,429),Consolas,20,sf::Color::Black,""));
        }
    }
    void interactive(const std::optional<sf::Event> &event,sf::RenderWindow &windows){
        if(const sf::Event::MouseButtonPressed *mousePressed = event->getIf<sf::Event::MouseButtonPressed>()){
            sf::Vector2i ppos = sf::Vector2i(mousePressed->position.x,mousePressed->position.y);
            Vecf rpos = windows.mapPixelToCoords(ppos,view);
            float mx = rpos.x;
            float my = rpos.y; 
            std::cout << mx << " " << my << std::endl;
            if(setting::limit_Nut_Exit(mx,my) == true){
                id_screen = par[id_screen];
                windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Arrow));
            }
            else if(setting::fps_part::limit_45_fps_button(mx,my) == true){
                fps_limit = 45;
                windows.setFramerateLimit(fps_limit);
            }
            else if(setting::fps_part::limit_60_fps_button(mx,my) == true){
                fps_limit = 60;
                windows.setFramerateLimit(fps_limit);
            }
            else if(setting::fps_part::limit_75_fps_button(mx,my) == true){
                fps_limit = 75;
                windows.setFramerateLimit(fps_limit);
            }
            else if(setting::fps_part::limit_auto_fps_button(mx,my) == true){
                fps_limit = -1;
                windows.setVerticalSyncEnabled(true);
            }

            if(setting::enter_name_part::limit_enter_name(mx,my) == true){
                setting::enter_name_part::ok_enter = true;
            }
            else if(setting::enter_name_part::ok_enter == true) setting::enter_name_part::ok_enter = false;
        }
        else if(const sf::Event::KeyPressed *KeyPressed = event->getIf<sf::Event::KeyPressed>()){
            if(KeyPressed->code == sf::Keyboard::Key::Escape){
                id_screen = 1;
            }
        }
        if(const sf::Event::TextEntered *ex = event->getIf<sf::Event::TextEntered>()){
            int code_char = ex->unicode;
            if(code_char == 8){
                if(setting::enter_name_part::ok_enter == true){
                    if(player1.name.empty() == false)   player1.name.pop_back();
                }
            }
            else if(code_char == 13){
                setting::enter_name_part::ok_enter = false;
            }
            else if(36 <= code_char && code_char <= 122){
                if(setting::enter_name_part::ok_enter == true){
                    player1.name += static_cast<char>(code_char);
                }
            }
        }
    }
    void process(sf::RenderWindow &windows){
        sf::RectangleShape overlay;
        overlay.setSize(Vecf(n,m));
        overlay.setFillColor(sf::Color(0,0,0,128));
        windows.draw(overlay);

        sf::Sprite khung_setting(tx_khung_setting);
        khung_setting.setPosition(Vecf(125,93.75f));
        khung_setting.scale(Vecf(0.7189542484f,0.7161458333f));
        windows.draw(khung_setting);


        Vecf rpos = windows.mapPixelToCoords(sf::Mouse::getPosition(windows),view);
        float mx = rpos.x;
        float my = rpos.y;
        
        if(limit_Nut_Exit(mx,my) || fps_part::limit_45_fps_button(mx,my) || fps_part::limit_60_fps_button(mx,my)
        || fps_part::limit_75_fps_button(mx,my) || fps_part::limit_auto_fps_button(mx,my)){
            windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Hand));
        }
        else if(fps_part::limit_enter_custom_fpt_reg(mx,my) || enter_name_part::limit_enter_name(mx,my)){
            windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Text));
        }
        else    windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Arrow));

        sf::Sprite Nut_Exit(tx_Nut_Exit1);
        if(limit_Nut_Exit(mx,my) == true){
            Nut_Exit.setTexture(tx_Nut_Exit2);
        }
        Nut_Exit.setPosition(Vecf(615,110));
        Nut_Exit.scale(Vecf(0.703125f,0.703125f));
        windows.draw(Nut_Exit);

        fps_part::draw(mx,my,windows);
        enter_name_part::draw(windows);
    }
}
namespace single_play{
    bool Paused_ok = false;
    sf::Texture tx_Paused;
    Ball PlaysBall;
    void pre_process(){
        if(tx_Paused.loadFromFile("Texture/Single/Khung_Paused.png") == false){
            exit(-1);
        }
        player1.trai = sf::Keyboard::Key::A;
        player1.phai = sf::Keyboard::Key::D;
        player1.y = 575.f;
        
        PlaysBall.x = 400.f;
        PlaysBall.y = 300.f;
        PlaysBall.R = 20.f;
        PlaysBall.v = Vecf((float)(rnd(0,1) == 1 ? 1 : -1) * rnd(250,300),(float)(rnd(0,1) == 1 ? 1 : -1) * rnd(250,300));

        PlaysBall.Color_code = sf::Color(255,180,0);
        pt_doan_t tmp;
        tmp.A = Vecf((float)player1.x,570.f);
        tmp.B = Vecf((float)(player1.x + player1.len),570.f);
        tmp.cre();
        PlaysBall.lines.push_back(tmp);
        PlaysBall.lines[0].pxtl = true;

        tmp.A = Vecf((float)player1.x,player1.y);
        tmp.B = Vecf((float)player1.x,player1.y + 25);
        tmp.cre();
        PlaysBall.lines.push_back(tmp);

        tmp.A = Vecf((float)(player1.x + player1.len),player1.y);
        tmp.B = Vecf((float)(player1.x + player1.len),player1.y + 25);
        tmp.cre();
        PlaysBall.lines.push_back(tmp);

        tmp.A = Vecf(160.f,0.f);
        tmp.B = Vecf(160.f,600.f);
        tmp.cre();
        PlaysBall.lines.push_back(tmp);
        
        tmp.A = Vecf(640.f,0.f);
        tmp.B = Vecf(640.f,600.f);
        tmp.cre();
        PlaysBall.lines.push_back(tmp);

        tmp.A = Vecf(0.f,0.f);
        tmp.B = Vecf(640.f,0.f);
        tmp.cre();
        PlaysBall.lines.push_back(tmp);

    }
    namespace Paused{
        bool limit_Resume(float mx,float my){
            if(mx < 315 || mx > 485 || my < 240 || my > 290) return false;
            return true;
        }
        bool limit_mainmenu(float mx,float my){
            if(mx < 315 || mx > 485 || my < 310 || my > 360)    return false;
            return true;
        }
        void draw(float &mx,float &my,sf::RenderWindow &windows){
            windows.draw(draw::reg(Vecf(0,0),Vecf(800,600),sf::Color(0,0,0,128)));
            sf::Sprite Khung_Paused(tx_Paused);
            Khung_Paused.scale(Vecf(0.5197505f,0.5199307f));
            Khung_Paused.setPosition(Vecf(275,150));
            windows.draw(Khung_Paused);
            windows.draw(draw::text("Paused",Vecf(300,160),Agencyfb,55,sf::Color::White,""));
            sf::Sprite Button(main_menu::tx_single_button);
            Button.scale(Vecf(0.0827815f,0.0792393f));
            Button.setPosition(Vecf(300,240));
            windows.draw(Button);
            Button.setPosition(Vecf(300,310));
            windows.draw(Button);
            Button.setPosition(Vecf(300,380));
            windows.draw(Button);
            windows.draw(draw::text("RESUME",Vecf(360,247),Agencyfb,30,(limit_Resume(mx,my) ? sf::Color(0,128,0) : sf::Color::Black),""));
            windows.draw(draw::text("MAIN MENU",Vecf(345,317),Agencyfb,30,(limit_mainmenu(mx,my) ? sf::Color(0,128,0) : sf::Color::Black),""));
            windows.draw(draw::text("UNKNOWS",Vecf(350,387),Agencyfb,30,sf::Color::Black,""));

        }
    }
    int Goalllll_ok = 0;
    sf::Time pretime_goal;
    void congratulation_act(sf::RenderWindow &windows){
        std::cout << "OK" << std::endl;
    }
    void goal(sf::RenderWindow &windows,Player &ply){
        if(Goalllll_ok == false){
            Goalllll_ok = true;
            ply.point += 1;
            pretime_goal = program_time.getElapsedTime();
        }
        if((program_time.getElapsedTime() - pretime_goal).asSeconds() >= 2.0){
            Goalllll_ok = false;
            PlaysBall.pret = program_time.getElapsedTime();
            PlaysBall.x = 400.f;
            PlaysBall.y = 300.f;
        }
        congratulation_act(windows);
        
    }
    void reset(){
        Paused_ok = false;
    }
    void interactive(const std::optional<sf::Event> &event,sf::RenderWindow &windows){
        if(const sf::Event::MouseButtonPressed *MousePressed = event->getIf<sf::Event::MouseButtonPressed>()){
            sf::Vector2i ppos = sf::Vector2i(MousePressed->position.x,MousePressed->position.y);
            Vecf rpos = windows.mapPixelToCoords(ppos,view);
            float mx = rpos.x;
            float my = rpos.y; 
            std::cout << mx << " " << my << std::endl;
            if(Paused_ok == true){
                if(Paused::limit_Resume(mx,my) == true){
                    Paused_ok = false;
                }
                else if(Paused::limit_mainmenu(mx,my) == true){
                    for(int i = 0;i < 5;++i)    main_menu::menu_ball[i].pret = program_time.getElapsedTime();
                    reset();
                    id_screen = par[id_screen];
                }
            }
        }
        if(const sf::Event::KeyPressed *KeyPressed = event->getIf<sf::Event::KeyPressed>()){
            if(KeyPressed->code == sf::Keyboard::Key::Escape){
                Paused_ok = (Paused_ok ^ 1);
            }
        }
    }
    void process(sf::RenderWindow &windows){
        windows.draw(main_menu::pic_main_menu.draw(Vecf(0.f,0.f)));
        windows.draw(draw::reg(Vecf(0,0),Vecf(160,600),sf::Color::Red));
        windows.draw(draw::reg(Vecf(640,0),Vecf(800,600),sf::Color::Red));//2 thanh chắn 2 bên
        
        windows.draw(draw::text(std::to_string(player1.point),Vecf(680,300),Agencyfb,50,sf::Color::White,""));
        windows.draw(draw::text(player1.name,Vecf(680,200),Agencyfb,50,sf::Color::White,""));

        Vecf rpos = windows.mapPixelToCoords(sf::Mouse::getPosition(windows),view);
        float mx = rpos.x;
        float my = rpos.y;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))    player1.act(sf::Keyboard::Key::A,0,PlaysBall);
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))   player1.act(sf::Keyboard::Key::D,0,PlaysBall);
        player1.process(windows);
        
        if(PlaysBall.y > (float)m + PlaysBall.R)    goal(windows,player2);
        else if(PlaysBall.y < -PlaysBall.R) goal(windows,player1);
        else    PlaysBall.process(windows);

        if(Paused_ok && (Paused::limit_Resume(mx,my) || Paused::limit_mainmenu(mx,my))){
            windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Hand));
        }
        else    windows.setMouseCursor(sf::Cursor(sf::Cursor::Type::Arrow));
        if(Paused_ok == true)   Paused::draw(mx,my,windows);
    }
}
void pre_process(){
    if(Consolas.openFromFile("Fonts/Consolas-Regular.ttf") == false){
        exit(-1);
    }
    if(Agencyfb.openFromFile("Fonts/Agencyfb_bold.ttf") == false){
        exit(-1);
    }
    main_menu::pre_process();
    setting::pre_process();
    single_play::pre_process();

    //thiết lập screen cha của id
    par[2] = 1;
    par[3] = 1;
}
signed main()
{
    sf::RenderWindow windows(sf::VideoMode({n, m}), "Ball-Game");
    windows.setFramerateLimit(fps_limit);
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
            if(id_screen == 1)  main_menu::interactive(event,windows);
            else if(id_screen == 2) setting::interactive(event,windows);
            else if(id_screen == 3) single_play::interactive(event,windows);
        }
        
        windows.clear(sf::Color::Black);
        windows.setView(view);
        
        if(id_screen == 1) main_menu::process(windows);
        else if(id_screen == 2){
            main_menu::process(windows);
            setting::process(windows);
        }
        else if(id_screen == 3){
            single_play::process(windows);
        }

        windows.display();
    }
}