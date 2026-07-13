#include "Header/level4.h"

// ── SKY ───────────────────────────────────────────────────────────────────────
Sky::Sky(){clouds={{60,45,90},{230,28,110},{420,50,85},{600,32,100},{740,55,72}};}
void Sky::update(float dt){for(auto&c:clouds){c.x-=22.f*dt;if(c.x+c.w<0)c.x=SW+20.f;}}
void Sky::draw(sf::RenderWindow& w){
    R(w,0,0,SW,SH*0.55f,{95,155,215}); R(w,0,SH*0.55f,SW,SH*0.45f,{120,175,230});
    for(auto&c:clouds)drawCloud(w,c.x,c.y,c.w);}
void Sky::drawCloud(sf::RenderWindow&w,float x,float y,float ww){
    sf::Color b(245,250,255),m(210,225,240),hi(255,255,255);
    R(w,x,y+14,ww,8,m); R(w,x+6,y+8,ww-12,10,b);
    R(w,x+14,y+3,ww-28,10,b); R(w,x+20,y,ww-40,6,b);
    R(w,x+16,y+1,10,4,hi); R(w,x+8,y+8,8,4,hi);
    R(w,x+4,y+18,ww-8,4,m);}

// ── SNOWFALL ──────────────────────────────────────────────────────────────────
Snowfall::Snowfall(){
    std::mt19937 r(77);
    std::uniform_real_distribution<float> rx(0,SW),ry(0,SH),rs(28,72),rd(4,16),rp(0,6.28f),rsz(2,5);
    for(int i=0;i<60;++i)flakes.push_back({rx(r),ry(r),rs(r),rd(r),rp(r),rsz(r),0,0});}
void Snowfall::update(float dt){
    std::uniform_real_distribution<float> rx(0,SW);
    for(auto&f:flakes){
        f.phase+=dt*1.4f; f.x+=std::sin(f.phase)*f.drift*dt; f.y+=f.spd*dt;
        f.shineT+=dt; f.shine=0.5f+0.5f*std::sin(f.shineT*3.5f+f.phase);
        if(f.y>SH+8){f.y=-8;f.x=rx(rng);}}}
void Snowfall::draw(sf::RenderWindow&w){for(auto&f:flakes)drawFlake(w,f);}
void Snowfall::px(sf::RenderWindow&w,float x,float y,float s,sf::Color c){
    sf::RectangleShape r({s,s});r.setFillColor(c);r.setPosition({x-s/2,y-s/2});w.draw(r);}
void Snowfall::drawFlake(sf::RenderWindow&w,const Flake&f){
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

// ── GROUND ────────────────────────────────────────────────────────────────────
Ground::Ground(){
    (void)rt.resize({512,120});rt.clear(sf::Color::Transparent);
    sf::Color d1(108,62,30),d2(130,78,44),snow(215,230,242),ss(170,190,210);
    R(rt,0,14,512,106,d1);
    for(float x=0;x<512;x+=16){R(rt,x,14,14,10,d2);R(rt,x+4,24,10,8,d1);}
    R(rt,0,0,512,14,snow);R(rt,0,10,512,5,ss);
    for(float x=0;x<512;x+=8)R(rt,x,0,6,4,{235,248,255,255});
    rt.display(); tex=sf::Texture(rt.getTexture());tex.setRepeated(true);spr.emplace(tex);}
void Ground::update(float dt,float spd){offset+=spd*dt;if(offset>=512)offset-=512;}
void Ground::draw(sf::RenderWindow&w){
    for(int sx=0;sx<(int)SW;sx+=2){float gy=groundYat((float)sx);R(w,(float)sx,gy+10,2,SH-gy,{100,58,28});}
    for(int sx=0;sx<(int)SW;sx+=2){float gy=groundYat((float)sx);R(w,(float)sx,gy,2,12,{215,230,242});R(w,(float)sx,gy+8,2,4,{170,190,210});}
    for(float bx=std::fmod(-offset,16.f);bx<SW;bx+=16.f){float gy=groundYat(bx);R(w,bx,gy-2,6,4,{235,248,255});}}

// ── TREE ──────────────────────────────────────────────────────────────────────
Tree::Tree(float sx,float sc):x(sx),scale(sc){}
void Tree::update(float dt,float spd){x-=spd*0.38f*dt; if(x<-100)x=SW+80.f;}
void Tree::draw(sf::RenderWindow&w){
    float base=groundYat(x);
    float s=scale;
    sf::Color dk (0,  72, 28);
    sf::Color md (0,  98, 38);
    sf::Color lt (18,128, 52);
    sf::Color sn (210,238,252);
    sf::Color snS(170,205,228);
    sf::Color trk(82, 52, 22);
    sf::Color trkD(55, 34, 12);
    R(w, x-3*s, base-20*s, 6*s, 20*s, trk);
    R(w, x-1*s, base-20*s, 2*s, 20*s, trkD);
    struct Layer{ float hw,h,yoff; };
    Layer layers[]={
        {34,18,20},{28,16,34},{22,15,46},{16,14,57},{10,12,67}
    };
    for(int i=0;i<5;++i){
        float hw=layers[i].hw*s;
        float lh=layers[i].h*s;
        float ly=base-layers[i].yoff*s;
        int rows=(int)lh;
        for(int row=0;row<rows;++row){
            float t=(float)(row+1)/rows;
            float rowW=hw*2.f*t;
            float rx2=x-rowW/2.f;
            sf::Color rowC=(row<rows/3)?lt:(row<rows*2/3?md:dk);
            R(w,rx2,ly+row,rowW,1,dk);
            if(row>rows/3)
                R(w,x-rowW*0.18f,ly+row,rowW*0.36f,1,md);
        }
        for(int row=2;row<rows;++row){
            float t=(float)(row+1)/rows;
            float rowW=hw*2.f*t;
            R(w,x-rowW/2.f,ly+row,2,1,{0,40,15});
            R(w,x+rowW/2.f-2,ly+row,2,1,{0,40,15});
        }
        int snowH=(int)(lh*0.30f);
        for(int row=0;row<snowH;++row){
            float t=(float)(row+1)/snowH;
            float sw2=hw*0.68f*t;
            R(w,x-sw2,ly+row,sw2*2,1, row<snowH/2 ? sn : snS);
        }
        float blobY=ly+lh*0.55f;
        float blobW=hw*0.42f;
        R(w,x-hw+2,blobY,    blobW,  3, sn);
        R(w,x-hw+2,blobY+2,  blobW*0.7f,2,snS);
        R(w,x+hw*0.3f,blobY, hw*0.28f,2,sn);
    }
    R(w,x-2*s,base-81*s, 4*s,12*s, dk);
    R(w,x-1*s,base-83*s, 2*s, 4*s, sn);
}

// ── COIN ──────────────────────────────────────────────────────────────────────
Coin::Coin(float sx,float sy):x(sx),y(sy){}
void Coin::update(float dt,float spd){x-=spd*dt; sparkT+=dt; ageT+=dt;}
float Coin::drawY()const{return y;}
void Coin::draw(sf::RenderWindow&w){
    if(collected)return;
    float cy=drawY();
    float s=smooth01(ageT/0.28f);
    if(s<=0.f)return;
    sf::RenderStates st=scaleAbout(x,cy,s);
    sf::Color glowC(255,220,60,(uint8_t)(100+50*std::sin(sparkT*5.f)));
    R(w,x-13,cy-13,26,26,glowC,st);
    sf::Color out(20,18,5),gold(240,185,20),shine(255,215,80),hi(255,240,140),dk(190,130,10);
    R(w,x-8,cy-10,16,2,out,st); R(w,x-10,cy-8,20,16,out,st); R(w,x-8,cy+6,16,2,out,st);
    R(w,x-10,cy-8,2,16,out,st); R(w,x+8,cy-8,2,16,out,st);
    R(w,x-8,cy-8,16,16,gold,st);
    R(w,x-6,cy-10,12,2,gold,st); R(w,x-6,cy+8,12,2,gold,st);
    R(w,x-6,cy-6,4,12,shine,st); R(w,x-2,cy-8,3,4,hi,st); R(w,x-2,cy-6,3,10,shine,st);
    R(w,x+4,cy-6,4,12,dk,st);}
sf::FloatRect Coin::bounds()const{float cy=drawY();return{{x-9.f,cy-11.f},{18.f,22.f}};}

// ── ROCK ──────────────────────────────────────────────────────────────────────
Rock::Rock(float sx):x(sx){}
void Rock::update(float dt,float spd){x-=spd*dt;}
void Rock::draw(sf::RenderWindow&w){
    float y=groundYat(x)-RH;
    sf::Color b(118,108,98),hi(155,145,135),dk(72,65,58),sn(222,234,245);
    R(w,x+4,y+14,36,20,b);R(w,x,y+8,44,16,b);R(w,x+8,y+3,28,10,b);R(w,x+12,y,20,6,b);
    R(w,x+2,y+8,12,8,hi);R(w,x+8,y+3,8,5,hi);
    R(w,x+34,y+10,8,24,dk);R(w,x+2,y+28,10,8,dk);
    R(w,x+8,y-2,26,8,sn);R(w,x+12,y-5,18,6,{238,248,255,255});}
sf::FloatRect Rock::bounds()const{float y=groundYat(x)-RH;return{{x+8,y+4},{28,RH-6}};}

// ── DEER ENEMY ────────────────────────────────────────────────────────────────
void DeerEnemy::spawn(){x=SW+20;active=true;passed=false;dead=false;runT=0;spawnT=0;}
void DeerEnemy::update(float dt,float gameSpd){
    if(!active)return;
    x-=(gameSpd*SPD_MULT)*dt;
    runT+=dt*10.f; spawnT+=dt;
    if(x<-100){active=false;}}
void DeerEnemy::draw(sf::RenderWindow&w){
    if(!active)return;
    float base=groundYat(x);
    float s=smooth01(spawnT/0.4f);
    if(s<=0.f)return;
    sf::RenderStates st=scaleAbout(x,base,s);
    drawDeer(w,x,base,true,st);}
sf::FloatRect DeerEnemy::bounds()const{float base=groundYat(x);return{{x-30,base-H},{60,H}};}
void DeerEnemy::drawDeer(sf::RenderWindow&w,float cx,float base,bool flip,const sf::RenderStates&st){
    float f=flip?-1.f:1.f;
    float ls=std::sin(runT)*12.f;
    R(w,cx+f*(-18),(int)(base-28+ls),5,28,dark,st); R(w,cx+f*(-6),(int)(base-28-ls),5,28,dark,st);
    R(w,cx+f*(6), (int)(base-28-ls),5,28,dark,st);  R(w,cx+f*(16),(int)(base-28+ls),5,28,dark,st);
    R(w,cx-22,base-70,48,42,body,st); R(w,cx-16,base-68,36,30,body,st);
    R(w,cx-12,base-58,28,18,belly,st);
    for(int i=0;i<4;++i)R(w,cx-10+i*8,base-62,4,4,spot,st);
    R(w,cx+20,base-68,8,10,belly,st);
    R(w,cx+f*14,base-80,14,24,body,st);
    R(w,cx+f*12,base-98,22,20,body,st);
    R(w,cx+f*24,base-94,10,10,belly,st);
    R(w,cx+f*30,base-90,5,5,nose,st);
    R(w,cx+f*16,base-96,4,4,{20,15,12},st);
    R(w,cx+f*16,base-100,8,3,dark,st);
    R(w,cx+f*18,base-100,3,18,antl,st);
    R(w,cx+f*10,base-96,10,3,antl,st); R(w,cx+f*10,base-100,3,6,antl,st);
    R(w,cx+f*22,base-96,8,3,antl,st);  R(w,cx+f*26,base-100,3,6,antl,st);
    R(w,cx+f*12,base-78,16,7,scarf,st); R(w,cx+f*22,base-76,5,10,scarfD,st);}

// ── GIANT ENEMY ───────────────────────────────────────────────────────────────
void Giant::spawn(){x=SW+20;active=true;passed=false;walkT=0;spawnT=0;}
void Giant::update(float dt,float gameSpd){
    if(!active)return;
    x-=gameSpd*1.2f*dt; walkT+=dt*6.f; spawnT+=dt;
    if(x<-120)active=false;}
void Giant::draw(sf::RenderWindow&w){
    if(!active)return;
    float base=groundYat(x);
    float s=smooth01(spawnT/0.45f);
    if(s<=0.f)return;
    sf::RenderStates st=scaleAbout(x,base,s);
    drawGiant(w,x,base,st);}
sf::FloatRect Giant::bounds()const{float base=groundYat(x);return{{x-W/2.f,base-H},{W,H}};}
void Giant::drawGiant(sf::RenderWindow&w,float cx,float base,const sf::RenderStates&st){
    float sc=0.4f;
    sf::Color bdy(185,195,205),blight(215,225,232),bdark(145,155,165);
    sf::Color skin(220,185,150),eye_c(200,80,20),pupil(20,12,8);
    sf::Color cloth(120,75,45),club(130,85,40),clubDk(90,55,25);
    sf::Color metal(100,110,120),mlight(140,150,160);
    float lb=std::sin(walkT)*8.f*sc;
    R(w,cx-22*sc,(int)(base-38*sc+lb),18*sc,38*sc,bdy,st); R(w,cx+4*sc,(int)(base-38*sc-lb),18*sc,38*sc,bdy,st);
    R(w,cx-24*sc,(int)(base-4*sc+lb),20*sc,6*sc,bdark,st);R(w,cx+2*sc,(int)(base-4*sc-lb),20*sc,6*sc,bdark,st);
    R(w,cx-28*sc,base-45*sc,56*sc,14*sc,cloth,st);
    R(w,cx-32*sc,base-100*sc,64*sc,58*sc,bdy,st);
    R(w,cx-28*sc,base-104*sc,56*sc,10*sc,bdy,st);
    R(w,cx-20*sc,base-90*sc,40*sc,36*sc,blight,st);
    C(w,cx,base-72*sc,18*sc,{200,215,225},20,st);
    C(w,cx,base-72*sc,12*sc,{185,200,215},20,st);
    R(w,cx+26*sc,base-92*sc,16*sc,38*sc,bdy,st);
    R(w,cx+28*sc,base-56*sc,14*sc,22*sc,bdy,st);
    R(w,cx+30*sc,base-42*sc,10*sc,32*sc,club,st);
    R(w,cx+24*sc,base-76*sc,22*sc,14*sc,clubDk,st);
    R(w,cx+22*sc,base-84*sc,26*sc,12*sc,club,st);
    R(w,cx+20*sc,base-88*sc,28*sc,8*sc,blight,st);
    R(w,cx+25*sc,base-70*sc,18*sc,8*sc,metal,st); R(w,cx+26*sc,base-69*sc,16*sc,6*sc,mlight,st);
    R(w,cx-42*sc,base-92*sc,16*sc,34*sc,bdy,st);
    R(w,cx-14*sc,base-108*sc,28*sc,12*sc,bdy,st);
    R(w,cx-28*sc,base-140*sc,56*sc,36*sc,bdy,st);
    R(w,cx-24*sc,base-148*sc,48*sc,14*sc,bdy,st);
    R(w,cx-20*sc,base-150*sc,40*sc,8*sc,bdy,st);
    R(w,cx-22*sc,base-136*sc,18*sc,5*sc,bdark,st); R(w,cx+4*sc,base-136*sc,18*sc,5*sc,bdark,st);
    R(w,cx-20*sc,base-130*sc,12*sc,8*sc,eye_c,st); R(w,cx+8*sc,base-130*sc,12*sc,8*sc,eye_c,st);
    R(w,cx-16*sc,base-130*sc,6*sc,6*sc,{230,100,30},st); R(w,cx+10*sc,base-130*sc,6*sc,6*sc,{230,100,30},st);
    R(w,cx-14*sc,base-128*sc,4*sc,4*sc,pupil,st); R(w,cx+12*sc,base-128*sc,4*sc,4*sc,pupil,st);
    R(w,cx-14*sc,base-118*sc,28*sc,14*sc,blight,st);
    R(w,cx-10*sc,base-116*sc,20*sc,10*sc,{210,185,170},st);
    R(w,cx-8*sc,base-112*sc,5*sc,4*sc,bdark,st); R(w,cx+3*sc,base-112*sc,5*sc,4*sc,bdark,st);
    R(w,cx-30*sc,base-140*sc,8*sc,14*sc,bdy,st); R(w,cx+22*sc,base-140*sc,8*sc,14*sc,bdy,st);
    R(w,cx-28*sc,base-138*sc,4*sc,8*sc,skin,st); R(w,cx+24*sc,base-138*sc,4*sc,8*sc,skin,st);
    R(w,cx-10*sc,base-152*sc,8*sc,6*sc,blight,st); R(w,cx+2*sc,base-152*sc,8*sc,6*sc,blight,st);}

// ── PLAYER ────────────────────────────────────────────────────────────────────
Player::Player(){y=groundYat(PX);}
void Player::setYetiMode(bool on){
    yetiMode=on;
    int maxJ=yetiMode?3:2;
    if(jumpsLeft>maxJ) jumpsLeft=maxJ;}
int Player::maxJumps()const{return yetiMode?3:2;}
void Player::jump(){
    if(jumpsLeft<=0)return;
    float vel=(jumpsLeft==maxJumps())?-450.f:(jumpsLeft==2?-390.f:-340.f);
    vy=vel; --jumpsLeft;}
void Player::update(float dt){
    if(jumpsLeft<maxJumps()||y<groundYat(PX)){vy+=920.f*dt;y+=vy*dt;}
    float gy=groundYat(PX);
    if(y>=gy){y=gy;vy=0;jumpsLeft=maxJumps();}
    if(flashT>0)flashT-=dt;}
void Player::hit(){flashT=0.55f;}
void Player::draw(sf::RenderWindow&w){
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
sf::FloatRect Player::bounds()const{return{{PX-5.f,y-28.f+4.f},{10.f,24.f}};}

// ── HUD ───────────────────────────────────────────────────────────────────────
HUD::HUD(){buildHeart();buildCoin();}
void HUD::draw(sf::RenderWindow&w,int score,int coins,int lives){
    panel(w,8,8,115,28,{25,20,42,210},{120,100,155,200});
    drawNum(w,score,14,13,{255,215,70});
    panel(w,8,42,115,28,{25,20,42,210},{180,155,30,200});
    sf::Sprite cs(ctex);cs.setScale({1.5f,1.5f});cs.setPosition({12,46});w.draw(cs);
    drawNum(w,coins,36,47,{255,210,50});
    sf::Sprite hs(htex);hs.setScale({2.f,2.f});
    for(int i=0;i<3;++i){
        hs.setColor(i<lives?sf::Color(225,60,85):sf::Color(65,50,72,130));
        hs.setPosition({12.f+i*34.f,SH-36.f});w.draw(hs);}}
void HUD::buildHeart(){
    (void)hrt.resize({14,12});hrt.clear(sf::Color::Transparent);
    sf::Color r(208,40,65),hi(245,108,128);
    const char*rows[]={"_XX_XX_","XXXXXXX","XXXXXXX","_XXXXX_","__XXX__","___X___"};
    for(int row=0;row<6;++row)for(int col=0;col<7;++col)
        if(rows[row][col]=='X')R(hrt,(float)col*2,(float)row*2,2,2,r);
    R(hrt,2,0,2,2,hi);R(hrt,8,0,2,2,hi);hrt.display();htex=sf::Texture(hrt.getTexture());}
void HUD::buildCoin(){
    (void)crt.resize({16,16});crt.clear(sf::Color::Transparent);
    R(crt,2,0,12,2,{20,18,5});R(crt,0,2,16,12,{20,18,5});R(crt,2,14,12,2,{20,18,5});
    R(crt,2,2,12,12,{240,185,20});R(crt,1,1,14,1,{240,185,20});R(crt,1,14,14,1,{240,185,20});
    R(crt,2,2,5,10,{255,215,80});R(crt,3,2,3,4,{255,240,140});
    R(crt,10,4,4,8,{190,130,10});
    crt.display();ctex=sf::Texture(crt.getTexture());}
void HUD::panel(sf::RenderWindow&w,float x,float y,float ww,float hh,sf::Color f,sf::Color o){
    sf::RectangleShape s({ww,hh});s.setFillColor(f);s.setOutlineColor(o);s.setOutlineThickness(2.f);s.setPosition({x,y});w.draw(s);}
void HUD::digit(sf::RenderWindow&w,int d,float x,float y,sf::Color c){
    const bool S[10][7]={{1,1,1,0,1,1,1},{0,0,1,0,0,1,0},{1,0,1,1,1,0,1},{1,0,1,1,0,1,1},{0,1,1,1,0,1,0},{1,1,0,1,0,1,1},{1,1,0,1,1,1,1},{1,0,1,0,0,1,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};
    sf::RectangleShape h({8.f,2.f}),v({2.f,6.f});h.setFillColor(c);v.setFillColor(c);
    if(S[d][0]){h.setPosition({x,y});w.draw(h);}     if(S[d][1]){v.setPosition({x,y+2});w.draw(v);}
    if(S[d][2]){v.setPosition({x+8,y+2});w.draw(v);} if(S[d][3]){h.setPosition({x,y+8});w.draw(h);}
    if(S[d][4]){v.setPosition({x,y+10});w.draw(v);}  if(S[d][5]){v.setPosition({x+8,y+10});w.draw(v);}
    if(S[d][6]){h.setPosition({x,y+16});w.draw(h);}}
void HUD::drawNum(sf::RenderWindow&w,int n,float x,float y,sf::Color c){
    for(char ch:std::to_string(n)){digit(w,ch-'0',x,y,c);x+=14.f;}}

// ── GAME OVER ─────────────────────────────────────────────────────────────────
void GameOverScreen::show(){active=true;timer=0;}
void GameOverScreen::update(float dt){if(active)timer+=dt;}
bool GameOverScreen::wantsRestart(sf::Keyboard::Key k){return active&&timer>1.f&&k==sf::Keyboard::Key::Space;}
void GameOverScreen::draw(sf::RenderWindow&w,int score,int coins){
    if(!active)return;
    R(w,0,0,SW,SH,{10,8,22,205});
    float px=SW/2-140.f,py=SH/2-90.f;
    sf::RectangleShape pn({280.f,180.f});pn.setFillColor({38,30,55,248});
    pn.setOutlineColor({165,140,195,255});pn.setOutlineThickness(3.f);pn.setPosition({px,py});w.draw(pn);
    R(w,px+10,py+14,260,26,{165,30,30,245});R(w,px+14,py+18,252,18,{200,50,50,245});
    R(w,px+20,py+55,110,32,{22,18,40,240});  drawNum(w,score,px+28,py+62,{255,215,70});
    R(w,px+145,py+55,110,32,{22,18,40,240}); drawNum(w,coins,px+168,py+62,{255,210,50});
    if((int)(timer*2)%2==0)R(w,px+10,py+106,260,22,{40,105,48,210});}
void GameOverScreen::digit(sf::RenderWindow&w,int d,float x,float y,sf::Color c){
    const bool S[10][7]={{1,1,1,0,1,1,1},{0,0,1,0,0,1,0},{1,0,1,1,1,0,1},{1,0,1,1,0,1,1},{0,1,1,1,0,1,0},{1,1,0,1,0,1,1},{1,1,0,1,1,1,1},{1,0,1,0,0,1,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};
    sf::RectangleShape h({8.f,2.f}),v({2.f,6.f});h.setFillColor(c);v.setFillColor(c);
    if(S[d][0]){h.setPosition({x,y});w.draw(h);}     if(S[d][1]){v.setPosition({x,y+2});w.draw(v);}
    if(S[d][2]){v.setPosition({x+8,y+2});w.draw(v);} if(S[d][3]){h.setPosition({x,y+8});w.draw(h);}
    if(S[d][4]){v.setPosition({x,y+10});w.draw(v);}  if(S[d][5]){v.setPosition({x+8,y+10});w.draw(v);}
    if(S[d][6]){h.setPosition({x,y+16});w.draw(h);}}
void GameOverScreen::drawNum(sf::RenderWindow&w,int n,float x,float y,sf::Color c){
    for(char ch:std::to_string(n)){digit(w,ch-'0',x,y,c);x+=14.f;}}
