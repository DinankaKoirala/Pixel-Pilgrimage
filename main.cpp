// Pixel Winter — SFML 3.x
// SPACE = jump, avoid rocks, 3 hearts, game over on death, SPACE = restart

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <filesystem>

static void box(sf::RenderWindow& w, float x, float y, float ww, float hh, sf::Color c) {
    sf::RectangleShape r({ww,hh}); r.setFillColor(c); r.setPosition({x,y}); w.draw(r);
}
static void box(sf::RenderTarget& w, float x, float y, float ww, float hh, sf::Color c) {
    sf::RectangleShape r({ww,hh}); r.setFillColor(c); r.setPosition({x,y}); w.draw(r);
}

// ── generate textures at startup ──────────────────────────────────────────────
static sf::Texture makeGroundTex() {
    sf::RenderTexture rt; rt.resize({512,96});
    rt.clear(sf::Color::Transparent);
    sf::Color d1(115,68,36), d2(138,84,48), snow(220,232,242), snowS(175,194,210);
    box(rt,0,12,512,84,d1);
    for(float x=0;x<512;x+=16){ box(rt,x,12,14,10,d2); box(rt,x+4,22,10,8,d1); }
    for(float x=8;x<512;x+=16){ box(rt,x,16,12,9,d2);  box(rt,x+2,25,8,7,d1); }
    box(rt,0,0,512,12,snow); box(rt,0,8,512,4,snowS);
    for(float x=0;x<512;x+=8) box(rt,x,0,6,4,{238,248,254,255});
    rt.display();
    sf::Texture t(rt.getTexture()); t.setRepeated(true); return t;
}

static sf::Texture makeRockTex() {
    sf::RenderTexture rt; rt.resize({20,18});
    rt.clear(sf::Color::Transparent);
    sf::Color b(120,110,100), hi(152,142,132), dk(80,72,65), sn(225,235,245);
    box(rt,2,8,16,9,b); box(rt,1,5,18,7,b); box(rt,4,2,12,5,b);
    box(rt,2,5,5,4,hi); box(rt,3,3,4,2,hi);
    box(rt,16,8,3,9,dk); box(rt,1,14,3,3,dk);
    box(rt,4,0,11,4,sn); box(rt,5,0,9,2,{240,248,255,255});
    rt.display(); return sf::Texture(rt.getTexture());
}

static sf::Texture makeHeartTex() {
    sf::RenderTexture rt; rt.resize({14,12});
    rt.clear(sf::Color::Transparent);
    sf::Color r(210,42,68), hi(248,110,130);
    const char* rows[]={"_XX_XX_","XXXXXXX","XXXXXXX","_XXXXX_","__XXX__","___X___"};
    for(int row=0;row<6;++row)
        for(int col=0;col<7;++col)
            if(rows[row][col]=='X') box(rt,(float)col*2,(float)row*2,2,2,r);
    box(rt,2,0,2,2,hi); box(rt,8,0,2,2,hi);
    rt.display(); return sf::Texture(rt.getTexture());
}

// ── 7-segment score ───────────────────────────────────────────────────────────
static void drawDigit(sf::RenderWindow& w, int d, float x, float y, sf::Color c) {
    const bool S[10][7]={{1,1,1,0,1,1,1},{0,0,1,0,0,1,0},{1,0,1,1,1,0,1},
        {1,0,1,1,0,1,1},{0,1,1,1,0,1,0},{1,1,0,1,0,1,1},{1,1,0,1,1,1,1},
        {1,0,1,0,0,1,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};
    sf::RectangleShape h({8.f,2.f}),v({2.f,6.f}); h.setFillColor(c); v.setFillColor(c);
    if(S[d][0]){h.setPosition({x,y});      w.draw(h);} if(S[d][1]){v.setPosition({x,y+2});   w.draw(v);}
    if(S[d][2]){v.setPosition({x+8,y+2});  w.draw(v);} if(S[d][3]){h.setPosition({x,y+8});   w.draw(h);}
    if(S[d][4]){v.setPosition({x,y+10});   w.draw(v);} if(S[d][5]){v.setPosition({x+8,y+10});w.draw(v);}
    if(S[d][6]){h.setPosition({x,y+16});   w.draw(h);}
}
static void drawNumber(sf::RenderWindow& w, int n, float x, float y, sf::Color c) {
    for(char ch:std::to_string(n)){ drawDigit(w,ch-'0',x,y,c); x+=14.f; }
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {
    constexpr unsigned W=768, H=480;
    constexpr float GROUND_Y  = H - 96.f;   // top of ground strip
    constexpr float STAND_Y   = GROUND_Y + 10.f; // player feet
    constexpr float SPEED     = 150.f;
    constexpr float JUMP_VEL  = -410.f;
    constexpr float GRAVITY   =  900.f;

    sf::RenderWindow win(sf::VideoMode({W,H}), "Pixel Winter");
    win.setFramerateLimit(60);

    // textures
    sf::Texture groundTex = makeGroundTex();
    sf::Texture rockTex   = makeRockTex();
    sf::Texture heartTex  = makeHeartTex();

    // sky (plain gradient drawn each frame — no texture needed)
    // ground sprite (tiled)
    sf::Sprite groundSpr(groundTex);
    groundSpr.setPosition({0.f, GROUND_Y});
    float groundOff = 0.f;  // scrolling offset

    // ── game state ────────────────────────────────────────────────────────────
    // player
    float py      = STAND_Y;   // feet Y
    float pvy     = 0.f;       // vertical velocity
    bool  onGnd   = true;
    float flashT  = 0.f;

    // rocks
    struct Rock { float x; };
    std::vector<Rock> rocks;
    std::mt19937 rng(42);
    float rockTimer = 1.5f, rockNext = 2.5f;

    // score / lives
    int   lives    = 3;
    int   score    = 0;
    float dist     = 0.f;
    float hitCD    = 0.f;
    bool  gameOver = false;
    float goTimer  = 0.f;

    auto reset = [&]() {
        py=STAND_Y; pvy=0.f; onGnd=true; flashT=0.f;
        rocks.clear(); lives=3; score=0; dist=0.f;
        hitCD=0.f; gameOver=false; goTimer=0.f;
        rockTimer=1.5f; rockNext=2.5f; groundOff=0.f;
    };

    sf::Clock clock;

    while(win.isOpen()) {
        // ── events ────────────────────────────────────────────────────────
        while(auto ev = win.pollEvent()) {
            if(ev->is<sf::Event::Closed>()) win.close();
            if(auto* kp = ev->getIf<sf::Event::KeyPressed>()) {
                if(kp->code == sf::Keyboard::Key::Escape) win.close();
                if(kp->code == sf::Keyboard::Key::Space) {
                    if(gameOver) reset();
                    else if(onGnd) { pvy = JUMP_VEL; onGnd = false; }
                }
            }
        }

        float dt = std::min(clock.restart().asSeconds(), 0.05f);

        // ── update ────────────────────────────────────────────────────────
        if(!gameOver) {
            dist += SPEED * dt;
            score = (int)(dist / 50.f);

            // scroll ground
            groundOff += SPEED * dt;
            if(groundOff >= 512.f) groundOff -= 512.f;

            // player physics
            if(!onGnd) {
                pvy += GRAVITY * dt;
                py  += pvy * dt;
                if(py >= STAND_Y) { py = STAND_Y; pvy = 0.f; onGnd = true; }
            }
            if(flashT > 0.f) flashT -= dt;
            hitCD = std::max(0.f, hitCD - dt);

            // spawn rocks
            rockTimer += dt;
            if(rockTimer >= rockNext) {
                rockTimer = 0.f;
                rockNext  = std::uniform_real_distribution<float>(2.0f,4.5f)(rng);
                rocks.push_back({(float)W + 40.f});
            }
            for(auto& r : rocks) r.x -= SPEED * dt;
            rocks.erase(std::remove_if(rocks.begin(),rocks.end(),
                [](const Rock& r){ return r.x < -80.f; }), rocks.end());

            // collision  (player hitbox: narrow column)
            float px = (float)W * 0.22f;
            float ptop = py - 28.f + 6.f;
            if(hitCD <= 0.f) {
                for(auto& r : rocks) {
                    // rock visual: 20*3=60px wide, 18*3=54px tall, origin bottom-centre
                    float rl = r.x - 30.f + 10.f;   // shrink 10px each side
                    float rr = r.x + 30.f - 10.f;
                    float rt2 = STAND_Y - 54.f + 5.f;
                    if(px+4.f > rl && px-4.f < rr && py > rt2) {
                        lives--;
                        flashT = 0.5f;
                        hitCD  = 1.0f;
                        if(lives <= 0) { gameOver = true; goTimer = 0.f; }
                        break;
                    }
                }
            }
        } else {
            goTimer += dt;
        }

        // ── draw ──────────────────────────────────────────────────────────
        win.clear({145,135,165});

        // sky gradient
        for(int y=0;y<(int)GROUND_Y;++y) {
            float t=(float)y/GROUND_Y;
            box(win,0,(float)y,(float)W,1,{(uint8_t)(145+t*30),(uint8_t)(135+t*28),(uint8_t)(165+t*22),255});
        }

        // clouds (static, simple)
        sf::Color cl(108,98,128), clhi(130,120,150);
        float cxs[]={30,200,370,530}, cws[]={88,100,82,90};
        for(int i=0;i<4;++i) {
            box(win,cxs[i],30,cws[i],14,cl);
            box(win,cxs[i]+8,20,cws[i]-16,12,cl);
            box(win,cxs[i]+14,12,cws[i]-30,8,clhi);
        }

        // ground (tiled scroll)
        float gx = -groundOff;
        while(gx < (float)W) { groundSpr.setPosition({gx, GROUND_Y}); win.draw(groundSpr); gx+=512.f; }

        // rocks
        for(auto& r : rocks) {
            sf::Sprite rs(rockTex);
            rs.setScale({3.f,3.f});
            rs.setOrigin({10.f,18.f});
            rs.setPosition({r.x, STAND_Y});
            win.draw(rs);
        }

        // player
        float bx = (float)W*0.22f - 7.f;
        float by = py - 28.f;
        bool  fl = flashT>0.f && (int)(flashT*12)%2==0;
        sf::Color body = fl ? sf::Color(255,80,80) : sf::Color(65,105,175);
        sf::Color hat  = fl ? sf::Color(255,80,80) : sf::Color(190,35,35);
        // shadow
        float rise = STAND_Y - py;
        float ss   = std::max(0.3f, 1.f - rise/130.f);
        box(win, (float)W*0.22f - 8.f*ss, STAND_Y+1.f, 16.f*ss, 3.f, {0,0,0,45});
        box(win, bx,    py-4,  6,4, {38,28,55});
        box(win, bx+8,  py-4,  6,4, {38,28,55});
        box(win, bx,    by+18, 6,6, {52,44,85});
        box(win, bx+8,  by+18, 6,6, {52,44,85});
        box(win, bx,    by+9, 14,10, body);
        box(win, bx,    by+9, 14, 3, {42,148,58});
        box(win, bx+1,  by+2, 12, 8, {220,175,130});
        box(win, bx+3,  by+4,  2, 2, {25,18,18});
        box(win, bx+9,  by+4,  2, 2, {25,18,18});
        box(win, bx-1,  by+1, 16, 3, {215,215,215});
        box(win, bx+2,  by-5, 10, 8, hat);
        box(win, bx+5,  by-8,  4, 4, {255,255,255});

        // score panel
        box(win, 8,8, 90,28, {30,25,45,210});
        drawNumber(win, score, 14, 14, {255,215,70});

        // hearts
        sf::Sprite hs(heartTex); hs.setScale({2.f,2.f});
        for(int i=0;i<3;++i) {
            hs.setColor(i<lives ? sf::Color(225,65,85) : sf::Color(70,55,75,130));
            hs.setPosition({12.f+i*34.f,(float)H-36.f}); win.draw(hs);
        }

        // game over screen
        if(gameOver) {
            box(win,0,0,(float)W,(float)H,{15,10,30,210});
            float px2=W/2.f-140.f, py2=H/2.f-85.f;
            box(win,px2,py2,280,170,{42,35,60,250});
            box(win,px2+10,py2+14,260,26,{175,35,35,245});
            box(win,px2+10,py2+62,260,38,{28,22,45,240});
            drawNumber(win,score,px2+20,py2+72,{255,215,70});
            if((int)(goTimer*2)%2==0)
                box(win,px2+10,py2+120,260,20,{50,120,58,210});
        }

        win.display();
    }
    return 0;
