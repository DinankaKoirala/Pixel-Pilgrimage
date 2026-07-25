#ifndef LEVEL4_H
#define LEVEL4_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <algorithm>
#include <optional>

// ── constants ──────────────────────────────────────────────────────────────────
static constexpr float SW=800.f,SH=480.f;
static constexpr float BASE_SPEED=140.f;
static constexpr float BASE_Y=430.f, SLOPE=0.20f;

inline float groundYat(float x){return BASE_Y-SLOPE*x;}

// ── utility draw helpers ──────────────────────────────────────────────────────
inline void R(sf::RenderTarget& t,float x,float y,float w,float h,sf::Color c,
              const sf::RenderStates& st=sf::RenderStates::Default){
    sf::RectangleShape s({w,h});s.setFillColor(c);s.setPosition({x,y});t.draw(s,st);}
inline void R(sf::RenderWindow& t,float x,float y,float w,float h,sf::Color c,
              const sf::RenderStates& st=sf::RenderStates::Default){
    sf::RectangleShape s({w,h});s.setFillColor(c);s.setPosition({x,y});t.draw(s,st);}
inline void C(sf::RenderWindow& w,float cx,float cy,float r,sf::Color c,int pts=20,
              const sf::RenderStates& st=sf::RenderStates::Default){
    sf::CircleShape s(r,pts);s.setFillColor(c);s.setOrigin({r,r});s.setPosition({cx,cy});w.draw(s,st);}
inline float smooth01(float t){t=std::clamp(t,0.f,1.f);return t*t*(3.f-2.f*t);}
inline sf::RenderStates scaleAbout(float ax,float ay,float s){
    sf::Transform tr;tr.translate({ax,ay}).scale({s,s}).translate({-ax,-ay});
    sf::RenderStates st;st.transform=tr;return st;}

// ── SKY ───────────────────────────────────────────────────────────────────────
class Sky{
public:
    struct Cloud{float x,y,w;};
    std::vector<Cloud> clouds;
    Sky();
    void update(float dt);
    void draw(sf::RenderWindow& w);
private:
    void drawCloud(sf::RenderWindow& w,float x,float y,float ww);
};

// ── SNOWFALL ──────────────────────────────────────────────────────────────────
class Snowfall{
public:
    struct Flake{float x,y,spd,drift,phase,sz,shine,shineT;};
    std::vector<Flake> flakes;
    std::mt19937 rng{99};
    Snowfall();
    void update(float dt);
    void draw(sf::RenderWindow& w);
private:
    void px(sf::RenderWindow& w,float x,float y,float s,sf::Color c);
    void drawFlake(sf::RenderWindow& w,const Flake& f);
};

// ── GROUND ────────────────────────────────────────────────────────────────────
class Ground{
public:
    float offset=0;
    Ground();
    void update(float dt,float spd);
    void draw(sf::RenderWindow& w);
private:
    sf::RenderTexture rt; sf::Texture tex; std::optional<sf::Sprite> spr;
};

// ── TREE ──────────────────────────────────────────────────────────────────────
class Tree{
public:
    float x; float scale;
    Tree(float sx,float sc=1.f);
    void update(float dt,float spd);
    void draw(sf::RenderWindow& w);
};

// ── COIN ──────────────────────────────────────────────────────────────────────
class Coin{
public:
    float x,y; bool collected=false; float sparkT=0; float ageT=0;
    Coin(float sx,float sy);
    void update(float dt,float spd);
    float drawY()const;
    void draw(sf::RenderWindow& w);
    sf::FloatRect bounds()const;
};

// ── ROCK ──────────────────────────────────────────────────────────────────────
class Rock{
public:
    float x; bool counted=false;
    static constexpr float RW=44,RH=36;
    Rock(float sx);
    void update(float dt,float spd);
    void draw(sf::RenderWindow& w);
    sf::FloatRect bounds()const;
};

// ── DEER ENEMY ────────────────────────────────────────────────────────────────
class DeerEnemy{
public:
    float x; bool active=false,passed=false,dead=false;
    float runT=0,spawnT=0;
    static constexpr float SPD_MULT=1.8f;
    static constexpr float W=70,H=80;
    void spawn();
    void update(float dt,float gameSpd);
    void draw(sf::RenderWindow& w);
    sf::FloatRect bounds()const;
private:
    sf::Color body{155,102,60},belly{195,162,120},dark{108,68,35},antl{130,88,45};
    sf::Color nose{200,100,80},scarf{200,30,30},scarfD{155,18,18},spot{185,152,115};
    void drawDeer(sf::RenderWindow& w,float cx,float base,bool flip,const sf::RenderStates& st);
};

// ── GIANT ENEMY ───────────────────────────────────────────────────────────────
class Giant{
public:
    float x; bool active=false,passed=false; float walkT=0,spawnT=0;
    static constexpr float W=32,H=36;
    void spawn();
    void update(float dt,float gameSpd);
    void draw(sf::RenderWindow& w);
    sf::FloatRect bounds()const;
private:
    void drawGiant(sf::RenderWindow& w,float cx,float base,const sf::RenderStates& st);
};

// ── PLAYER ────────────────────────────────────────────────────────────────────
class Player{
public:
    static constexpr float PX=170.f;
    float y,vy=0; int jumpsLeft=2; float flashT=0;
    bool yetiMode=false;
    Player();
    void setYetiMode(bool on);
    int maxJumps()const;
    void jump();
    void update(float dt);
    void hit();
    void draw(sf::RenderWindow& w);
    sf::FloatRect bounds()const;
};

// ── HUD ───────────────────────────────────────────────────────────────────────
class HUD{
public:
    HUD();
    void draw(sf::RenderWindow& w,int score,int coins,int lives);
private:
    sf::Texture htex,ctex; sf::RenderTexture hrt,crt;
    void buildHeart();
    void buildCoin();
    void panel(sf::RenderWindow& w,float x,float y,float ww,float hh,sf::Color f,sf::Color o);
    void digit(sf::RenderWindow& w,int d,float x,float y,sf::Color c);
    void drawNum(sf::RenderWindow& w,int n,float x,float y,sf::Color c);
};

// ── GAME OVER ─────────────────────────────────────────────────────────────────
class GameOverScreen{
public:
    bool active=false; float timer=0;
    void show();
    void update(float dt);
    bool wantsRestart(sf::Keyboard::Key k);
    void draw(sf::RenderWindow& w,int score,int coins);
private:
    void digit(sf::RenderWindow& w,int d,float x,float y,sf::Color c);
    void drawNum(sf::RenderWindow& w,int n,float x,float y,sf::Color c);
};

#endif
