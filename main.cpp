#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <algorithm>

// ── utils ─────────────────────────────────────────────────────────────────────
static void R(sf::RenderTarget& t,float x,float y,float w,float h,sf::Color c){
    sf::RectangleShape s({w,h});s.setFillColor(c);s.setPosition({x,y});t.draw(s);}
static void R(sf::RenderWindow& t,float x,float y,float w,float h,sf::Color c){
    sf::RectangleShape s({w,h});s.setFillColor(c);s.setPosition({x,y});t.draw(s);}
static void C(sf::RenderWindow& w,float cx,float cy,float r,sf::Color c,int pts=20){
    sf::CircleShape s(r,pts);s.setFillColor(c);s.setOrigin({r,r});s.setPosition({cx,cy});w.draw(s);}

static constexpr float SW=800.f,SH=480.f;
static constexpr float BASE_SPEED=140.f;
static constexpr float BASE_Y=430.f, SLOPE=0.20f;
static float groundYat(float x){return BASE_Y-SLOPE*x;}

// ── SKY ───────────────────────────────────────────────────────────────────────
class Sky{
public:
    struct Cloud{float x,y,w;};
    std::vector<Cloud> clouds;
    Sky(){clouds={{60,45,90},{230,28,110},{420,50,85},{600,32,100},{740,55,72}};}
    void update(float dt){for(auto&c:clouds){c.x-=22.f*dt;if(c.x+c.w<0)c.x=SW+20.f;}}
    void draw(sf::RenderWindow& w){
        R(w,0,0,SW,SH*0.55f,{95,155,215}); R(w,0,SH*0.55f,SW,SH*0.45f,{120,175,230});
        for(auto&c:clouds)drawCloud(w,c.x,c.y,c.w);}
private:
    void drawCloud(sf::RenderWindow&w,float x,float y,float ww){
        sf::Color b(245,250,255),m(210,225,240),hi(255,255,255);
        R(w,x,y+14,ww,8,m); R(w,x+6,y+8,ww-12,10,b);
        R(w,x+14,y+3,ww-28,10,b); R(w,x+20,y,ww-40,6,b);
        R(w,x+16,y+1,10,4,hi); R(w,x+8,y+8,8,4,hi); R(w,x+4,y+18,ww-8,4,m);}
};

// ── SNOWFALL ──────────────────────────────────────────────────────────────────
class Snowfall{
public:
    struct Flake{float x,y,spd,drift,phase,sz,shine,shineT;};
    std::vector<Flake> flakes;
    std::mt19937 rng{99};
    Snowfall(){
        std::mt19937 r(77);
        std::uniform_real_distribution<float> rx(0,SW),ry(0,SH),rs(28,72),rd(4,16),rp(0,6.28f),rsz(2,5);
        for(int i=0;i<60;++i) flakes.push_back({rx(r),ry(r),rs(r),rd(r),rp(r),rsz(r),0,0});}
    void update(float dt){
        std::uniform_real_distribution<float> rx(0,SW);
        for(auto&f:flakes){
            f.phase+=dt*1.4f; f.x+=std::sin(f.phase)*f.drift*dt; f.y+=f.spd*dt;
            f.shineT+=dt; f.shine=0.5f+0.5f*std::sin(f.shineT*3.5f+f.phase);
            if(f.y>SH+8) {f.y=-8;f.x=rx(rng);}}}
    void draw(sf::RenderWindow&w){for(auto&f:flakes)drawFlake(w,f);}
private:
    void px(sf::RenderWindow&w,float x,float y,float s,sf::Color c){
        sf::RectangleShape r({s,s});r.setFillColor(c);r.setPosition({x-s/2,y-s/2});w.draw(r);}
    void drawFlake(sf::RenderWindow&w,const Flake&f){
        uint8_t br=(uint8_t)(155+f.shine*95);
        sf::Color core(br,218,255,215),arm(125,190,238,175),glow(255,255,255,(uint8_t)(f.shine*135));
        float s=f.sz; float cx=f.x,cy=f.y;
        px(w,cx,cy,s,core);
        float dirs[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
        for(auto&d:dirs){
            for(int i=1;i<=3;++i){
                float ax=cx+d[0]*i*s*1.4f,ay=cy+d[1]*i*s*1.4f;
                float sz=(i==1)?s:(i==2?s*0.8f:s*0.5f);
                px(w,ax,ay,sz,i==1?core:arm);
                if(i==2){px(w,ax+d[1]*s*1.2f,ay+d[0]*s*1.2f,s*0.5f,arm);
                         px(w,ax-d[1]*s*1.2f,ay-d[0]*s*1.2f,s*0.5f,arm);}}
        }
        float diags[4][2]={{-1,-1},{1,-1},{-1,1},{1,1}};
        for(auto&d:diags){px(w,cx+d[0]*s,cy+d[1]*s,s*0.7f,arm);px(w,cx+d[0]*s*2,cy+d[1]*s*2,s*0.5f,arm);}
        if(f.shine>0.7f)px(w,cx,cy,s*0.6f,glow);}
};

// ── GROUND ────────────────────────────────────────────────────────────────────
class Ground{
public:
    float offset=0;
    Ground(){
        rt.resize({512,120});rt.clear(sf::Color::Transparent);
        sf::Color d1(108,62,30),d2(130,78,44),snow(215,230,242),ss(170,190,210);
        R(rt,0,14,512,106,d1);
        for(float x=0;x<512;x+=16){R(rt,x,14,14,10,d2);R(rt,x+4,24,10,8,d1);}
        R(rt,0,0,512,14,snow);R(rt,0,10,512,5,ss);
        for(float x=0;x<512;x+=8)R(rt,x,0,6,4,{235,248,255,255});
        rt.display(); tex=sf::Texture(rt.getTexture());tex.setRepeated(true);spr.emplace(tex);}
    void update(float dt,float spd){offset+=spd*dt;if(offset>=512)offset-=512;}
    void draw(sf::RenderWindow&w){
        for(int sx=0;sx<(int)SW;sx+=2){float gy=groundYat((float)sx);R(w,(float)sx,gy+10,2,SH-gy,{100,58,28});}
        for(int sx=0;sx<(int)SW;sx+=2){float gy=groundYat((float)sx);R(w,(float)sx,gy,2,12,{215,230,242});R(w,(float)sx,gy+8,2,4,{170,190,210});}
        for(float bx=std::fmod(-offset,16.f);bx<SW;bx+=16.f){float gy=groundYat(bx);R(w,bx,gy-2,6,4,{235,248,255});}}
private:
    sf::RenderTexture rt; sf::Texture tex; std::optional<sf::Sprite> spr;
};

// ── TREE ──────────────────────────────────────────────────────────────────────
class Tree{
public:
    float x; float scale;
    Tree(float sx, float sc=1.f):x(sx),scale(sc){}

    void update(float dt,float spd){x-=spd*0.38f*dt; if(x<-100)x=SW+80.f;}

    void draw(sf::RenderWindow&w){
        float base=groundYat(x);
        float s=scale;

        // colours
        sf::Color dk (0,  72, 28);   // dark green
        sf::Color md (0,  98, 38);   // mid green
        sf::Color lt (18,128, 52);   // light green highlight
        sf::Color sn (210,238,252);  // snow white-blue
        sf::Color snS(170,205,228);  // snow shadow
        sf::Color trk(82, 52, 22);   // trunk
        sf::Color trkD(55, 34, 12);  // trunk dark

        // Trunk — two-tone for depth
        R(w, x-3*s, base-20*s, 6*s, 20*s, trk);
        R(w, x-1*s, base-20*s, 2*s, 20*s, trkD);

        // 5 layers bottom→top, each is a stepped triangle
        // layer: {half-width, height, y-offset from base}
        struct Layer{ float hw,h,yoff; };
        Layer layers[]={
            {34,18,20},{28,16,34},{22,15,46},{16,14,57},{10,12,67}
        };

        for(int i=0;i<5;++i){
            float hw=layers[i].hw*s;
            float lh=layers[i].h*s;
            float ly=base-layers[i].yoff*s;

            // Draw triangle as stepped pixel rows (gives hard 8-bit edge)
            int rows=(int)lh;
            for(int row=0;row<rows;++row){
                float t=(float)(row+1)/rows;
                float rowW=hw*2.f*t;
                float rx2=x-rowW/2.f;
                sf::Color rowC=(row<rows/3)?lt:(row<rows*2/3?md:dk);
                R(w,rx2,ly+row,rowW,1,dk);
                // centre highlight strip
                if(row>rows/3)
                    R(w,x-rowW*0.18f,ly+row,rowW*0.36f,1,md);
            }
            // Dark pixel outline on left/right edges (hard pixel look)
            for(int row=2;row<rows;++row){
                float t=(float)(row+1)/rows;
                float rowW=hw*2.f*t;
                R(w,x-rowW/2.f,ly+row,2,1,{0,40,15});
                R(w,x+rowW/2.f-2,ly+row,2,1,{0,40,15});
            }

            // Snow cap on tip of each layer
            int snowH=(int)(lh*0.30f);
            for(int row=0;row<snowH;++row){
                float t=(float)(row+1)/snowH;
                float sw2=hw*0.68f*t;
                R(w,x-sw2,ly+row,sw2*2,1, row<snowH/2 ? sn : snS);
            }
            // Snow blob on left branch edge
            float blobY=ly+lh*0.55f;
            float blobW=hw*0.42f;
            R(w,x-hw+2,blobY,    blobW,  3, sn);
            R(w,x-hw+2,blobY+2,  blobW*0.7f,2,snS);
            // Right side smaller snow
            R(w,x+hw*0.3f,blobY, hw*0.28f,2,sn);
        }

        // Very tip spike
        R(w,x-2*s,base-81*s, 4*s,12*s, dk);
        R(w,x-1*s,base-83*s, 2*s, 4*s, sn);
    }
};

// ── COIN ─────────────────────────────────────────────────────────────────────
class Coin{
public:
    float x,y; bool collected=false; float bob=0,sparkT=0;
    Coin(float sx,float sy):x(sx),y(sy){}
    void update(float dt,float spd){x-=spd*dt; bob+=dt*4.f; sparkT+=dt;}
    float drawY()const{return y+std::sin(bob)*4.f;}  // actual Y (moves with bob)
    void draw(sf::RenderWindow&w){
        if(collected)return;
        float cy=drawY();
        uint8_t ga=(uint8_t)(100+50*std::sin(sparkT*5.f));
        R(w,x-13,cy-13,26,26,{255,220,60,ga});
        sf::Color out(20,18,5),gd(240,185,20),sh(255,215,80),hi(255,240,140),dk(190,130,10);
        R(w,x-6,cy-10,12,2,out); R(w,x-8,cy-8,16,2,out);
        R(w,x-10,cy-6,20,4,out); R(w,x-10,cy-2,20,8,out);
        R(w,x-10,cy+2,20,4,out); R(w,x-8,cy+6,16,2,out);
        R(w,x-6,cy+8,12,2,out);
        R(w,x-4,cy-8,8,2,gd); R(w,x-6,cy-6,12,2,gd);
        R(w,x-8,cy-4,16,8,gd); R(w,x-6,cy+4,12,2,gd);
        R(w,x-4,cy+6,8,2,gd);
        R(w,x-6,cy-6,4,2,hi); R(w,x-8,cy-4,6,2,sh);
        R(w,x-8,cy-2,6,2,sh); R(w,x-4,cy-2,4,2,hi);
        R(w,x+2,cy+2,4,2,dk); R(w,x+4,cy,4,2,dk);}
    sf::FloatRect bounds()const{float cy=drawY();return{{x-14.f,cy-14.f},{28.f,28.f}};}
};

// ── ROCK ──────────────────────────────────────────────────────────────────────
class Rock{
public:
    float x; bool counted=false;
    static constexpr float RW=44,RH=36;
    Rock(float sx):x(sx){}
    void update(float dt,float spd){x-=spd*dt;}
    void draw(sf::RenderWindow&w){
        float y=groundYat(x)-RH;
        sf::Color b(118,108,98),hi(155,145,135),dk(72,65,58),sn(222,234,245);
        R(w,x+4,y+14,36,20,b);R(w,x,y+8,44,16,b);R(w,x+8,y+3,28,10,b);R(w,x+12,y,20,6,b);
        R(w,x+2,y+8,12,8,hi);R(w,x+8,y+3,8,5,hi);
        R(w,x+34,y+10,8,24,dk);R(w,x+2,y+28,10,8,dk);
        R(w,x+8,y-2,26,8,sn);R(w,x+12,y-5,18,6,{238,248,255,255});}
    sf::FloatRect bounds()const{float y=groundYat(x)-RH;return{{x+8,y+4},{28,RH-6}};}
};

// ── DEER ENEMY ────────────────────────────────────────────────────────────────
class DeerEnemy{
public:
    float x; bool active=false,passed=false,dead=false;
    float runT=0;
    static constexpr float SPD_MULT=1.8f; // runs toward player
    static constexpr float W=70,H=80;

    void spawn(){x=SW+20;active=true;passed=false;dead=false;runT=0;}
    void update(float dt,float gameSpd){
        if(!active)return;
        x-=(gameSpd*SPD_MULT)*dt; // comes FROM right toward left (player)
        runT+=dt*10.f;
        if(x<-100){active=false;}}
    void draw(sf::RenderWindow&w){
        if(!active)return;
        float base=groundYat(x);
        drawDeer(w,x,base,true);}
    sf::FloatRect bounds()const{float base=groundYat(x);return{{x-30,base-H},{60,H}};}

private:
    sf::Color body{155,102,60},belly{195,162,120},dark{108,68,35},antl{130,88,45};
    sf::Color nose{200,100,80},scarf{200,30,30},scarfD{155,18,18},spot{185,152,115};

    void drawDeer(sf::RenderWindow&w,float cx,float base,bool flip){
        float f=flip?-1.f:1.f; // flipped — facing left
        // legs running
        float ls=std::sin(runT)*12.f;
        R(w,cx+f*(-18),(int)(base-28+ls),5,28,dark); R(w,cx+f*(-6),(int)(base-28-ls),5,28,dark);
        R(w,cx+f*(6), (int)(base-28-ls),5,28,dark);  R(w,cx+f*(16),(int)(base-28+ls),5,28,dark);
        // body
        R(w,cx-22,base-70,48,42,body); R(w,cx-16,base-68,36,30,body);
        R(w,cx-12,base-58,28,18,belly);
        for(int i=0;i<4;++i)R(w,cx-10+i*8,base-62,4,4,spot);
        // tail (on right since flipped left)
        R(w,cx+20,base-68,8,10,belly);
        // neck + head (facing LEFT since coming toward player)
        R(w,cx+f*14,base-80,14,24,body);
        R(w,cx+f*12,base-98,22,20,body);
        R(w,cx+f*24,base-94,10,10,belly);
        R(w,cx+f*30,base-90,5,5,nose);
        // angry eye
        R(w,cx+f*16,base-96,4,4,{20,15,12});
        R(w,cx+f*16,base-100,8,3,dark); // angry brow
        // antlers
        R(w,cx+f*18,base-100,3,18,antl);
        R(w,cx+f*10,base-96,10,3,antl); R(w,cx+f*10,base-100,3,6,antl);
        R(w,cx+f*22,base-96,8,3,antl);  R(w,cx+f*26,base-100,3,6,antl);
        // scarf
        R(w,cx+f*12,base-78,16,7,scarf); R(w,cx+f*22,base-76,5,10,scarfD);}
};

// ── GIANT ENEMY ──────────────────────────────────────────────────────────────
class Giant{
public:
    float x; bool active=false,passed=false; float walkT=0;
    static constexpr float W=80,H=110;

    void spawn(){x=SW+20;active=true;passed=false;walkT=0;}
    void update(float dt,float gameSpd){
        if(!active)return;
        x-=gameSpd*1.2f*dt; walkT+=dt*6.f;
        if(x<-120)active=false;}
    void draw(sf::RenderWindow&w){
        if(!active)return;
        float base=groundYat(x);
        drawGiant(w,x,base);}
    sf::FloatRect bounds()const{float base=groundYat(x);return{{x-28,base-H},{56,H}};}

private:
    void drawGiant(sf::RenderWindow&w,float cx,float base){
        // Body colours — grey/white like ref image
        sf::Color bdy(185,195,205),blight(215,225,232),bdark(145,155,165);
        sf::Color skin(220,185,150),eye_c(200,80,20),pupil(20,12,8);
        sf::Color cloth(120,75,45),club(130,85,40),clubDk(90,55,25);
        sf::Color metal(100,110,120),mlight(140,150,160);

        // legs (wide, walking bob)
        float lb=std::sin(walkT)*8.f;
        R(w,cx-22,(int)(base-38+lb),18,38,bdy); R(w,cx+4,(int)(base-38-lb),18,38,bdy);
        R(w,cx-24,(int)(base-4+lb), 20,6,bdark);R(w,cx+2, (int)(base-4-lb), 20,6,bdark);

        // loincloth
        R(w,cx-28,base-45,56,14,cloth);

        // main body — round and fat like ref
        R(w,cx-32,base-100,64,58,bdy);
        R(w,cx-28,base-104,56,10,bdy);
        // belly circle (lighter)
        R(w,cx-20,base-90,40,36,blight);
        C(w,cx,base-72,18,{200,215,225});   // belly circle like ref
        C(w,cx,base-72,12,{185,200,215});

        // arms
        // left arm (holding club — at right of screen since facing left)
        R(w,cx+26,base-92,16,38,bdy);
        R(w,cx+28,base-56,14,22,bdy);
        // club
        R(w,cx+30,base-42,10,32,club);
        R(w,cx+24,base-76,22,14,clubDk); // club head
        R(w,cx+22,base-84,26,12,club);
        R(w,cx+20,base-88,28,8,blight);  // club highlight
        // metal brace on arm
        R(w,cx+25,base-70,18,8,metal); R(w,cx+26,base-69,16,6,mlight);

        // right arm
        R(w,cx-42,base-92,16,34,bdy);

        // neck
        R(w,cx-14,base-108,28,12,bdy);

        // head — big round like ref
        R(w,cx-28,base-140,56,36,bdy);
        R(w,cx-24,base-148,48,14,bdy);
        R(w,cx-20,base-150,40,8,bdy);

        // face details
        // angry brow ridges
        R(w,cx-22,base-136,18,5,bdark); R(w,cx+4,base-136,18,5,bdark);
        // eyes — orange-red like ref
        R(w,cx-20,base-130,12,8,eye_c); R(w,cx+8,base-130,12,8,eye_c);
        R(w,cx-16,base-130,6,6,{230,100,30}); R(w,cx+10,base-130,6,6,{230,100,30});
        R(w,cx-14,base-128,4,4,pupil); R(w,cx+12,base-128,4,4,pupil);

        // nose / snout
        R(w,cx-14,base-118,28,14,blight);
        R(w,cx-10,base-116,20,10,{210,185,170});
        // nostrils
        R(w,cx-8,base-112,5,4,bdark); R(w,cx+3,base-112,5,4,bdark);

        // ears
        R(w,cx-30,base-140,8,14,bdy); R(w,cx+22,base-140,8,14,bdy);
        R(w,cx-28,base-138,4,8,skin); R(w,cx+24,base-138,4,8,skin);

        // head top ridges (like ref's head bumps)
        R(w,cx-10,base-152,8,6,blight); R(w,cx+2,base-152,8,6,blight);}
};

// ── PLAYER ───────────────────────────────────────────────────────────────────
class Player{
public:
    static constexpr float PX=170.f;
    float y,vy=0; int jumpsLeft=2; float flashT=0;
    bool yetiMode=false,jumpHeld=false;
    Player(){y=groundYat(PX);}

    void setYetiMode(bool on){
        yetiMode=on;
        int maxJ=yetiMode?3:2;
        if(jumpsLeft>maxJ) jumpsLeft=maxJ;
    }
    int maxJumps()const{return yetiMode?3:2;}

    void jump(){
        if(jumpsLeft<=0)return;
        float vel=(jumpsLeft==maxJumps())?-460.f:(jumpsLeft==2?-400.f:-350.f);
        vy=vel; --jumpsLeft;jumpHeld=true;
    }
    void releaseJump(){if(vy<0)vy*=0.45f;jumpHeld=false;}
    void update(float dt){
        float grav=jumpHeld&&vy<0?780.f:860.f;
        if(jumpsLeft<maxJumps()||y<groundYat(PX)){vy+=grav*dt;y+=vy*dt;}
        float gy=groundYat(PX);
        if(y>=gy){y=gy;vy=0;jumpsLeft=maxJumps();jumpHeld=false;}
        if(flashT>0)flashT-=dt;}
    void hit(){flashT=0.55f;}

    void draw(sf::RenderWindow&w){
        bool fl=flashT>0&&(int)(flashT*12)%2==0;
        sf::Color body=fl?sf::Color(255,80,80):sf::Color(60,100,170);
        sf::Color hat =fl?sf::Color(255,80,80):sf::Color(180,32,32);
        float bx=PX-7.f,by=y-28.f;
        float rise=groundYat(PX)-y,ss=std::max(0.3f,1.f-rise/130.f);
        R(w,PX-8.f*ss,y+2,16.f*ss,3,{0,0,0,40});
        R(w,bx,y-4,6,4,{38,28,55});   R(w,bx+8,y-4,6,4,{38,28,55});
        R(w,bx,by+18,6,6,{52,44,85}); R(w,bx+8,by+18,6,6,{52,44,85});
        R(w,bx,by+9,14,10,body);       R(w,bx,by+9,14,3,{42,148,58});
        R(w,bx+1,by+2,12,8,{220,175,130});
        R(w,bx+3,by+4,2,2,{25,18,18}); R(w,bx+9,by+4,2,2,{25,18,18});
        R(w,bx-1,by+1,16,3,{215,215,215});
        R(w,bx+2,by-5,10,8,hat); R(w,bx+5,by-8,4,4,{255,255,255});}



    sf::FloatRect bounds()const{return{{PX-5.f,y-28.f+4.f},{10.f,24.f}};}
};



// ── HUD ───────────────────────────────────────────────────────────────────────
class HUD{
public:
    HUD(){buildHeart();buildCoin();}
    void draw(sf::RenderWindow&w,int score,int coins,int lives){
        R(w,0,0,SW,66,{15,12,30,200});
        R(w,0,66,SW,2,{120,100,180,160});
        // SCORE panel
        panel(w,10,10,54,20,{30,24,55,220},{80,65,120,200});
        drawNum(w,score,18,14,{255,215,70});
        // SCORE label
        for(int i=0;i<5;++i)R(w,18+i*14,32,10,2,{100,85,140,180});
        // COINS panel
        panel(w,10,38,54,20,{30,24,55,220},{180,155,30,200});
        sf::Sprite cs(ctex);cs.setScale({1.5f,1.5f});cs.setPosition({14,42});w.draw(cs);
        drawNum(w,coins,36,42,{255,210,50});
        // Hearts
        R(w,0,SH-32,SW,32,{15,12,30,180});
        sf::Sprite hs(htex);hs.setScale({2.f,2.f});
        for(int i=0;i<3;++i){
            hs.setColor(i<lives?sf::Color(225,60,85):sf::Color(65,50,72,130));
            hs.setPosition({12.f+i*34.f,SH-28.f});w.draw(hs);}}
private:
    sf::Texture htex,ctex; sf::RenderTexture hrt,crt;
    void buildHeart(){
        hrt.resize({14,12});hrt.clear(sf::Color::Transparent);
        sf::Color r(208,40,65),hi(245,108,128);
        const char*rows[]={"_XX_XX_","XXXXXXX","XXXXXXX","_XXXXX_","__XXX__","___X___"};
        for(int row=0;row<6;++row)for(int col=0;col<7;++col)
            if(rows[row][col]=='X')R(hrt,(float)col*2,(float)row*2,2,2,r);
        R(hrt,2,0,2,2,hi);R(hrt,8,0,2,2,hi);hrt.display();htex=sf::Texture(hrt.getTexture());}
    void buildCoin(){
        crt.resize({16,16});crt.clear(sf::Color::Transparent);
        R(crt,2,0,12,2,{20,18,5});R(crt,0,2,16,12,{20,18,5});R(crt,2,14,12,2,{20,18,5});
        R(crt,2,2,12,12,{240,185,20});R(crt,1,1,14,1,{240,185,20});R(crt,1,14,14,1,{240,185,20});
        R(crt,2,2,5,10,{255,215,80});R(crt,3,2,3,4,{255,240,140});
        R(crt,10,4,4,8,{190,130,10});
        crt.display();ctex=sf::Texture(crt.getTexture());}
    void panel(sf::RenderWindow&w,float x,float y,float ww,float hh,sf::Color f,sf::Color o){
        sf::RectangleShape s({ww,hh});s.setFillColor(f);s.setOutlineColor(o);s.setOutlineThickness(2.f);s.setPosition({x,y});w.draw(s);}
    void digit(sf::RenderWindow&w,int d,float x,float y,sf::Color c){
        const bool S[10][7]={{1,1,1,0,1,1,1},{0,0,1,0,0,1,0},{1,0,1,1,1,0,1},{1,0,1,1,0,1,1},{0,1,1,1,0,1,0},{1,1,0,1,0,1,1},{1,1,0,1,1,1,1},{1,0,1,0,0,1,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};
        sf::RectangleShape h({8.f,2.f}),v({2.f,6.f});h.setFillColor(c);v.setFillColor(c);
        if(S[d][0]){h.setPosition({x,y});w.draw(h);}     if(S[d][1]){v.setPosition({x,y+2});w.draw(v);}
        if(S[d][2]){v.setPosition({x+8,y+2});w.draw(v);} if(S[d][3]){h.setPosition({x,y+8});w.draw(h);}
        if(S[d][4]){v.setPosition({x,y+10});w.draw(v);}  if(S[d][5]){v.setPosition({x+8,y+10});w.draw(v);}
        if(S[d][6]){h.setPosition({x,y+16});w.draw(h);}}
    void drawNum(sf::RenderWindow&w,int n,float x,float y,sf::Color c){
        for(char ch:std::to_string(n)){digit(w,ch-'0',x,y,c);x+=14.f;}}
};

// ── GAME OVER ─────────────────────────────────────────────────────────────────
class GameOverScreen{
public:
    bool active=false,isWin=false; float timer=0,fadeIn=0;
    float nbx,nby,nbw=120,nbh=36;
    void show(){active=true;timer=0;fadeIn=0;}
    void update(float dt){if(active){timer+=dt;fadeIn=std::min(1.f,fadeIn+dt*2.5f);}}
    bool wantsRestart(sf::Keyboard::Key k){return active&&timer>1.f&&k==sf::Keyboard::Key::Space;}
    bool wantsNext(float mx,float my){return active&&isWin&&timer>1.f&&mx>=nbx&&mx<=nbx+nbw&&my>=nby&&my<=nby+nbh;}
    void draw(sf::RenderWindow&w,int score,int coins,int level){
        if(!active)return;
        uint8_t oa=(uint8_t)(fadeIn*205);
        R(w,0,0,SW,SH,{10,8,22,oa});
        if(fadeIn<0.25f)return;
        float px=SW/2-150.f,py=SH/2-100.f;
        nbx=px+90;nby=py+155;
        uint8_t pa=(uint8_t)(std::min(1.f,(fadeIn-0.25f)/0.75f)*248);
        sf::RectangleShape pn({300.f,200.f});
        if(isWin){
            pn.setFillColor({20,45,28,pa});
            pn.setOutlineColor({80,195,100,(uint8_t)(pa*.9f)});}
        else{
            pn.setFillColor({38,30,55,pa});
            pn.setOutlineColor({165,140,195,(uint8_t)(pa*.9f)});}
        pn.setOutlineThickness(3.f);pn.setPosition({px,py});w.draw(pn);
        if(isWin){
            R(w,px+10,py+14,280,26,{30,120,50,pa});
            R(w,px+14,py+18,272,18,{50,160,70,pa});
            if((int)(timer*4)%2==0){
                R(w,px+22,py+16,6,6,{255,235,50,pa});
                R(w,px+270,py+16,6,6,{255,235,50,pa});}
        } else {
            R(w,px+10,py+14,280,26,{165,30,30,pa});
            R(w,px+14,py+18,272,18,{200,50,50,pa});}
        R(w,px+20,py+55,120,32,{22,18,40,pa}); drawNum(w,score,px+30,py+62,{255,215,70});
        R(w,px+160,py+55,120,32,{22,18,40,pa}); drawNum(w,coins,px+182,py+62,{255,210,50});
        // Level indicator
        R(w,px+20,py+100,260,20,{22,18,40,pa});
        drawNum(w,level,px+130,py+102,{150,200,255});
        int cw=isWin?(int)timer%2:0;
        if(timer>1.f&&(int)(timer*2)%2==0&&!isWin){
            sf::Color hh=isWin?sf::Color(50,170,80,pa):sf::Color(40,105,48,pa);
            R(w,px+10,py+106,260,22,hh);}
        // Next button (win only)
        if(isWin&&timer>1.f){
            uint8_t ba=(uint8_t)(200+55*std::sin(timer*3.f));
            R(w,nbx,nby,nbw,nbh,{40,90,200,ba});
            sf::RectangleShape nb({nbw,nbh});nb.setFillColor(sf::Color::Transparent);
            nb.setOutlineColor({100,180,255,ba});nb.setOutlineThickness(2.f);
            nb.setPosition({nbx,nby});w.draw(nb);
            drawNum(w,level+1,nbx+40,nby+9,{255,255,255});
            R(w,nbx+58,nby+12,8,2,{255,255,255});R(w,nbx+62,nby+12,2,2,{255,255,255});R(w,nbx+62,nby+14,2,2,{255,255,255});R(w,nbx+62,nby+16,2,2,{255,255,255});R(w,nbx+62,nby+18,2,2,{255,255,255});R(w,nbx+58,nby+20,8,2,{255,255,255});R(w,nbx+62,nby+22,2,2,{255,255,255});}}
private:
    void digit(sf::RenderWindow&w,int d,float x,float y,sf::Color c){
        const bool S[10][7]={{1,1,1,0,1,1,1},{0,0,1,0,0,1,0},{1,0,1,1,1,0,1},{1,0,1,1,0,1,1},{0,1,1,1,0,1,0},{1,1,0,1,0,1,1},{1,1,0,1,1,1,1},{1,0,1,0,0,1,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};
        sf::RectangleShape h({8.f,2.f}),v({2.f,6.f});h.setFillColor(c);v.setFillColor(c);
        if(S[d][0]){h.setPosition({x,y});w.draw(h);}     if(S[d][1]){v.setPosition({x,y+2});w.draw(v);}
        if(S[d][2]){v.setPosition({x+8,y+2});w.draw(v);} if(S[d][3]){h.setPosition({x,y+8});w.draw(h);}
        if(S[d][4]){v.setPosition({x,y+10});w.draw(v);}  if(S[d][5]){v.setPosition({x+8,y+10});w.draw(v);}
        if(S[d][6]){h.setPosition({x,y+16});w.draw(h);}}
    void drawNum(sf::RenderWindow&w,int n,float x,float y,sf::Color c){
        for(char ch:std::to_string(n)){digit(w,ch-'0',x,y,c);x+=14.f;}}
};

// ── MAIN ──────────────────────────────────────────────────────────────────────
int main(){
    auto desktop=sf::VideoMode::getDesktopMode();
    sf::RenderWindow win(desktop,"Pixel Winter",sf::State::Fullscreen);
    win.setFramerateLimit(60);
    sf::View v(sf::FloatRect({0,0},{800,480}));
    v.setViewport(sf::FloatRect({0,0},{1,1}));
    win.setView(v);

    Sky    sky;  Ground ground; Snowfall snow;
    Player player;
    HUD hud; GameOverScreen gameOver;

    std::vector<Tree>      trees;
    std::vector<Rock>      rocks;
    std::vector<DeerEnemy> deerEnemies(1);
    std::vector<Giant>     giants(1);
    std::vector<Coin>      coins;

    // Trees with varied scales for depth
    float treeXs[]={80,200,350,500,650,140,420,570};
    float treeSc[]={1.0f,1.3f,0.8f,1.1f,1.4f,0.9f,1.2f,0.75f};
    for(int i=0;i<8;++i) trees.emplace_back(treeXs[i],treeSc[i]);

    std::mt19937 rng(42);
    int level=4;
    float rockTimer=1.8f,  rockNext=2.8f;
    float deerTimer=0,     deerNext=6.f;
    float yetiTimer=0,     yetiNext=20.f;
    float coinTimer=0,     coinNext=2.f;

    int   lives=3, score=0, coinCount=0;
    float dist=0, hitCD=0;
    bool spaceHeld=false;

    auto gameSpeed=[&]()->float{
        float lvlMult=1.f+(level-4)*0.25f;
        return BASE_SPEED*lvlMult*(1.f+std::min((float)score/150.f,1.2f));};

    auto lvlUp=[&](){
        level++;
        player=Player();
        lives=3;score=0;coinCount=0;dist=0;hitCD=0;
        rocks.clear();coins.clear();
        for(auto&d:deerEnemies)d.active=false;
        for(auto&g:giants)g.active=false;
        gameOver.active=false;gameOver.isWin=false;gameOver.fadeIn=0;
        rockTimer=1.8f;rockNext=2.8f;
        deerTimer=0;deerNext=6.f;
        yetiTimer=0;yetiNext=20.f;
        coinTimer=0;coinNext=2.f;};

    auto reset=[&](){
        level=4;
        player=Player();
        lives=3;score=0;coinCount=0;dist=0;hitCD=0;
        rocks.clear();coins.clear();
        for(auto&d:deerEnemies)d.active=false;
        for(auto&g:giants)g.active=false;
        gameOver.active=false;gameOver.isWin=false;gameOver.fadeIn=0;
        rockTimer=1.8f;rockNext=2.8f;
        deerTimer=0;deerNext=6.f;
        yetiTimer=0;yetiNext=20.f;
        coinTimer=0;coinNext=2.f;};

    sf::Clock clock;

    while(win.isOpen()){
        while(auto ev=win.pollEvent()){
            if(ev->is<sf::Event::Closed>())win.close();
            if(auto*kp=ev->getIf<sf::Event::KeyPressed>()){
                if(kp->code==sf::Keyboard::Key::Escape)win.close();
                if(kp->code==sf::Keyboard::Key::Space){
                    if(gameOver.wantsRestart(kp->code))reset();
                    else player.jump();}}
            if(auto*kr=ev->getIf<sf::Event::KeyReleased>()){
                if(kr->code==sf::Keyboard::Key::Space)player.releaseJump();}
            if(auto*mb=ev->getIf<sf::Event::MouseButtonPressed>()){
                if(mb->button==sf::Mouse::Button::Left){
                    sf::Vector2f mp=win.mapPixelToCoords(mb->position);
                    if(gameOver.wantsNext(mp.x,mp.y))lvlUp();}}}

        float dt=std::min(clock.restart().asSeconds(),0.05f);
        float spd=gameSpeed();
        gameOver.update(dt);

        if(!gameOver.active){
            dist+=spd*dt; score=(int)(dist/50.f);
            int winScore=400+level*50;
            if(score>=winScore&&!gameOver.active){gameOver.show();gameOver.isWin=true;}

            sky.update(dt); ground.update(dt,spd); snow.update(dt);
            for(auto&t:trees)t.update(dt,spd);
            player.update(dt);

            rockTimer+=dt;
            if(rockTimer>=rockNext){
                rockTimer=0;rockNext=std::uniform_real_distribution<float>(1.5f,3.5f)(rng);
                rocks.emplace_back(SW+40.f);}
            for(auto&r:rocks)r.update(dt,spd);
            rocks.erase(std::remove_if(rocks.begin(),rocks.end(),[](const Rock&r){return r.x<-80;}),rocks.end());

            deerTimer+=dt;
            if(deerTimer>=deerNext){
                deerTimer=0;
                deerNext=std::uniform_real_distribution<float>(5.f,10.f)(rng);
                for(auto&d:deerEnemies)if(!d.active){d.spawn();break;}}
            for(auto&d:deerEnemies)d.update(dt,spd);

            yetiTimer+=dt;
            if(yetiTimer>=yetiNext){
                yetiTimer=0;
                yetiNext=std::uniform_real_distribution<float>(18.f,28.f)(rng);
                for(auto&g:giants)if(!g.active){g.spawn();break;}}
            for(auto&g:giants)g.update(dt,spd);

            bool yetiOnScreen=false;
            for(auto&g:giants) if(g.active) yetiOnScreen=true;
            player.setYetiMode(yetiOnScreen);

            coinTimer+=dt;
            if(coinTimer>=coinNext){
                coinTimer=0;coinNext=std::uniform_real_distribution<float>(0.8f,2.2f)(rng);
                int count=std::uniform_int_distribution<int>(1,2+level/4)(rng);
                for(int i=0;i<count;++i){
                    float cx=SW+30.f+i*28.f;
                    float cy=groundYat(cx)-55.f-std::uniform_real_distribution<float>(0,50)(rng);
                    coins.emplace_back(cx,cy);}}
            for(auto&c:coins)c.update(dt,spd);
            coins.erase(std::remove_if(coins.begin(),coins.end(),[](const Coin&c){return c.x<-30||c.collected;}),coins.end());

            hitCD=std::max(0.f,hitCD-dt);
            auto pb=player.bounds();

            for(auto&c:coins)
                if(!c.collected&&c.bounds().findIntersection(pb)){c.collected=true;coinCount++;}

            if(hitCD<=0.f){
                for(auto&r:rocks)
                    if(r.bounds().findIntersection(pb)){lives--;player.hit();hitCD=1.f;if(lives<=0)gameOver.show();break;}
                for(auto&d:deerEnemies)
                    if(d.active&&d.bounds().findIntersection(pb)){lives--;player.hit();hitCD=1.f;if(lives<=0)gameOver.show();break;}
                for(auto&g:giants)
                    if(g.active&&g.bounds().findIntersection(pb)){lives--;player.hit();hitCD=1.f;if(lives<=0)gameOver.show();break;}}}

        // ── draw ──────────────────────────────────────────────────────────
        win.clear({95,155,215});
        sky.draw(win); snow.draw(win);
        for(auto&t:trees)t.draw(win);
        ground.draw(win);
        for(auto&r:rocks)r.draw(win);
        for(auto&c:coins)c.draw(win);
        for(auto&d:deerEnemies)d.draw(win);
        for(auto&g:giants)g.draw(win);
        player.draw(win);
        hud.draw(win,score,coinCount,lives);
        // Triple-jump hint banner when yeti is on screen
        if(player.yetiMode){
            R(win,SW/2-85,8,170,24,{40,30,60,210});
            sf::RectangleShape hb({170.f,24.f});
            hb.setFillColor(sf::Color::Transparent);
            hb.setOutlineColor({200,80,255,200});hb.setOutlineThickness(2.f);
            hb.setPosition({SW/2-85,8});win.draw(hb);
            for(int i=0;i<3;++i){
                sf::Color jc=i<player.jumpsLeft?sf::Color(180,100,255):sf::Color(60,40,80,150);
                R(win,SW/2-30.f+i*22.f,14,14,12,jc);}}
        gameOver.draw(win,score,coinCount,level);
        win.display();}
}
