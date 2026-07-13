#include "src/Header/level4.h"

int main(){
    sf::RenderWindow win(sf::VideoMode({(unsigned)SW,(unsigned)SH}),"Pixel Winter");
    win.setFramerateLimit(60);
    win.setVerticalSyncEnabled(true);

    Sky    sky;  Ground ground; Snowfall snow;
    Player player;
    HUD hud; GameOverScreen gameOver;

    std::vector<Tree>      trees;
    std::vector<Rock>      rocks;
    std::vector<DeerEnemy> deerEnemies(1);
    std::vector<Giant>     giants(1);
    std::vector<Coin>      coins;

    float treeXs[]={80,200,350,500,650,140,420,570};
    float treeSc[]={1.0f,1.3f,0.8f,1.1f,1.4f,0.9f,1.2f,0.75f};
    for(int i=0;i<8;++i) trees.emplace_back(treeXs[i],treeSc[i]);

    std::mt19937 rng(42);
    float rockTimer=1.8f,  rockNext=2.8f;
    float deerTimer=0,     deerNext=6.f;
    float yetiTimer=0,     yetiNext=20.f;
    float coinTimer=0,     coinNext=3.f;

    int   lives=3, score=0, coinCount=0;
    float dist=0, hitCD=0;

    auto gameSpeed=[&]()->float{
        return BASE_SPEED*(1.f+std::min((float)score/150.f,1.5f));};

    auto reset=[&](){
        player=Player();
        lives=3;score=0;coinCount=0;dist=0;hitCD=0;
        rocks.clear();coins.clear();
        for(auto&d:deerEnemies)d.active=false;
        for(auto&g:giants)g.active=false;
        gameOver.active=false;
        rockTimer=1.8f;rockNext=2.8f;
        deerTimer=0;deerNext=6.f;
        yetiTimer=0;yetiNext=20.f;
        coinTimer=0;coinNext=3.f;};

    sf::Clock clock;

    while(win.isOpen()){
        while(auto ev=win.pollEvent()){
            if(ev->is<sf::Event::Closed>())win.close();
            if(auto*kp=ev->getIf<sf::Event::KeyPressed>()){
                if(kp->code==sf::Keyboard::Key::Escape)win.close();
                if(kp->code==sf::Keyboard::Key::Space){
                    if(gameOver.wantsRestart(kp->code))reset();
                    else player.jump();}}}

        float dt=std::min(clock.restart().asSeconds(),0.05f);
        float spd=gameSpeed();
        gameOver.update(dt);

        if(!gameOver.active){
            dist+=spd*dt; score=(int)(dist/50.f);

            sky.update(dt); ground.update(dt,spd); snow.update(dt);
            for(auto&t:trees)t.update(dt,spd);
            player.update(dt);

            auto rocksVisible=[&]()->bool{
                for(auto&r:rocks)if(r.x>-40.f)return true;
                return false;};
            auto deerActive=[&]()->bool{
                for(auto&d:deerEnemies)if(d.active)return true;
                return false;};
            auto yetiActive=[&]()->bool{
                for(auto&g:giants)if(g.active)return true;
                return false;};

            rockTimer+=dt;
            if(rockTimer>=rockNext&&!deerActive()&&!yetiActive()){
                rockTimer=0;rockNext=std::uniform_real_distribution<float>(2.f,4.f)(rng);
                rocks.emplace_back(SW+40.f);}
            for(auto&r:rocks)r.update(dt,spd);
            rocks.erase(std::remove_if(rocks.begin(),rocks.end(),[](const Rock&r){return r.x<-80;}),rocks.end());

            deerTimer+=dt;
            if(deerTimer>=deerNext&&!rocksVisible()&&!yetiActive()){
                deerTimer=0;
                deerNext=std::uniform_real_distribution<float>(6.f,12.f)(rng);
                for(auto&d:deerEnemies)if(!d.active){d.spawn();break;}}
            for(auto&d:deerEnemies)d.update(dt,spd);

            yetiTimer+=dt;
            if(yetiTimer>=yetiNext&&!rocksVisible()&&!deerActive()){
                yetiTimer=0;
                yetiNext=std::uniform_real_distribution<float>(20.f,30.f)(rng);
                for(auto&g:giants)if(!g.active){g.spawn();break;}}
            for(auto&g:giants)g.update(dt,spd);

            bool yetiOnScreen=false;
            for(auto&g:giants) if(g.active) yetiOnScreen=true;
            player.setYetiMode(yetiOnScreen);

            coinTimer+=dt;
            if(coinTimer>=coinNext){
                coinTimer=0;coinNext=std::uniform_real_distribution<float>(2.2f,5.5f)(rng);
                bool blocked=false;
                for(auto&r:rocks) if(r.x>SW-30.f) blocked=true;
                for(auto&d:deerEnemies) if(d.active&&d.x>SW-50.f) blocked=true;
                for(auto&g:giants) if(g.active&&g.x>SW-50.f) blocked=true;
                if(!blocked){
                    std::uniform_int_distribution<int> clusterDist(1,3);
                    std::uniform_real_distribution<float> gapDist(24.f,38.f);
                    std::uniform_real_distribution<float> jitterDist(-3.f,3.f);
                    int n=clusterDist(rng);
                    float cx=SW+30.f;
                    for(int i=0;i<n;++i){
                        float cy=groundYat(cx)-12.f+jitterDist(rng);
                        coins.emplace_back(cx,cy);
                        cx+=gapDist(rng);}}}
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
        if(player.yetiMode){
            R(win,SW/2-85,8,170,24,{40,30,60,210});
            sf::RectangleShape hb({170.f,24.f});
            hb.setFillColor(sf::Color::Transparent);
            hb.setOutlineColor({200,80,255,200});hb.setOutlineThickness(2.f);
            hb.setPosition({SW/2-85,8});win.draw(hb);
            for(int i=0;i<3;++i){
                sf::Color jc=i<player.jumpsLeft?sf::Color(180,100,255):sf::Color(60,40,80,150);
                R(win,SW/2-30.f+i*22.f,14,14,12,jc);}}
        gameOver.draw(win,score,coinCount);
        win.display();}
}
