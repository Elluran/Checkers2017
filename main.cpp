#include <iostream>
#include "SDL2/SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <vector>
#include <sstream>

// version 1.1

using namespace std;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;

int  f = 2, last = -1;
SDL_Window *window = NULL;
SDL_Surface *image = NULL;
SDL_Texture *texture = NULL;
SDL_Texture *texturemenu = NULL;
SDL_Texture *textureb = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *textTexture = NULL;
TTF_Font *font = NULL;
SDL_Texture *loadTexture(string path);


SDL_Texture *buttons = NULL;
SDL_Texture *pieces = NULL;

class button {
public:
    button(int x=0, int y=0, int type=2) {
        buttonSize.x = 0;
        buttonSize.y = 0;
        buttonSize.w = 500;
        buttonSize.h = 200;

        if (type == 2) buttonSize.y += 200;
        else if(type == 3) buttonSize.y += 400;

        buttonPos.x = x - 125;
        buttonPos.y = y - 40;
        buttonPos.w = 250;
        buttonPos.h = 80;

        render();
    }
    void render() {
        SDL_Rect background1 = {0,0,800,800};
        SDL_RenderSetViewport(renderer, &background1);
        SDL_RenderCopy(renderer, buttons, &buttonSize, &buttonPos);
    }
    void init(int x=0, int y=0, int type=2){
        buttonSize.x = 0;
        buttonSize.y = 0;
        buttonSize.w = 500;
        buttonSize.h = 200;

        if (type == 2) buttonSize.y += 200;
        else if(type == 3) buttonSize.y += 400;

        buttonPos.x = x - 125;
        buttonPos.y = y - 40;
        buttonPos.w = 250;
        buttonPos.h = 80;

        render();
    }
    bool collision(int x, int y) {
        if (x > buttonPos.x && x < buttonPos.x + 250) {
            if (y > buttonPos.y && y < buttonPos.y + 80) return true;
        }
        return false;
    }
private:
    SDL_Rect buttonSize, buttonPos;
};



void game();

bool quit = false;

void init() {

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");


    window = SDL_CreateWindow("Checkers2017", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);


}
void loadMedia() {
    textureb = loadTexture("res/desk.png");
    texturemenu = loadTexture("res/bum2.png");
    buttons = loadTexture("res/buttons.png");
    pieces = loadTexture("res/pieces2.png");
    font = TTF_OpenFont("res/ubuntu.ttf", 60);
}
void close() {
    SDL_FreeSurface(image);
    image = NULL;
    SDL_DestroyTexture(texture);
    texture = NULL;
    SDL_DestroyTexture(textureb);
    textureb = NULL;
    SDL_DestroyTexture(pieces);
    pieces = NULL;
    SDL_DestroyTexture(buttons);
    buttons = NULL;
    SDL_DestroyTexture(texturemenu);
    texturemenu = NULL;
    SDL_DestroyTexture(textTexture);
    textTexture = NULL;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;

    IMG_Quit();
    SDL_Quit();

}

SDL_Texture *loadTexture(string path) {
    SDL_Texture *newTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);

    return newTexture;
}



class text{
public:
    int width, height;
    void renderText(string str){
        SDL_Color color = {0,0,0,255};
        SDL_Surface* textsurf = TTF_RenderUTF8_Blended(font, str.c_str(), color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textsurf);
        width = textsurf-> w/2;
        height = textsurf-> h/2;
        SDL_FreeSurface(textsurf);
    }
    void renderTextPos(int x, int y){
        SDL_Rect fontr = {x,y,width,height};
        SDL_RenderCopy(renderer, textTexture, NULL, &fontr);
    }
}text;

void mainmenu() {
    int x,y;
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texturemenu, NULL, NULL);
    button play(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 40, 1);
    button exit(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, 2);
    text.renderText("version 2.1");
    text.renderTextPos(SCREEN_WIDTH/2-text.width/2, SCREEN_HEIGHT - text.height);
    SDL_RenderPresent(renderer);

    bool g = false;
    SDL_Event e;
    while (!quit && !g) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                if (exit.collision(x, y)) quit = true;
                else if (play.collision(x, y)) g = true;
            }
        }
    }
    if (g) game();
}



class piece{
public:
    struct point{
        int x;
        int y;
    };
    int color, selected = 0, h = 0, s = 0, g = 0;
    point last;
    bool d;
    piece(int c = 0){
        color = c;
        d = 0;
    }
    void click(vector<vector<piece > > &desk,int player, int x,int y, int &f){
//        cout << "selected "<< selected <<" d " << d <<endl;
        if(color == 0 && !selected) for(int i=0;i<8;i++) for(int e=0;e<8;e++) {
                    desk[i][e].selected = 0;
                    desk[i][e].s = 0;
                }
        if(color == player && !h) {
            for(int i=0;i<8;i++) for(int e=0;e<8;e++) {
                    desk[i][e].selected = 0;
                    desk[i][e].s = 0;
                }
            if (d==0) {
                if(x > 0 && y >0 && desk[y-1][x-1].color==0 && color == 1) {
                    desk[y-1][x-1].last.x = x;
                    desk[y-1][x-1].last.y = y;
                    desk[y-1][x-1].selected = 1;
                    desk[y][x].s = 1;
                }
                if(x < 7 && y >0 && desk[y-1][x+1].color==0 && color == 1) {
                    desk[y-1][x+1].last.x = x;
                    desk[y-1][x+1].last.y = y;
                    desk[y-1][x+1].selected = 1;
                    desk[y][x].s = 1;
                }
                if(x > 0 && y <7 && desk[y+1][x-1].color==0 && color == 2) {
                    desk[y+1][x-1].last.x = x;
                    desk[y+1][x-1].last.y = y;
                    desk[y+1][x-1].selected = 1;
                    desk[y][x].s = 1;
                }
                if(x < 7 && y <7 && desk[y+1][x+1].color==0 && color == 2) {
                    desk[y+1][x+1].last.x = x;
                    desk[y+1][x+1].last.y = y;
                    desk[y+1][x+1].selected = 1;
                    desk[y][x].s = 1;
                }
            }
            else {
                int a, b;
                a = x;
                b = y;
                while(a>0 && b>0 && desk[b-1][a-1].color == 0) {
                    desk[b-1][a-1].selected = 1;
                    desk[b-1][a-1].last.x = x;
                    desk[b-1][a-1].last.y = y;
                    a--;
                    b--;
                }
                a = x;
                b = y;
                while(a<7 && b>0 && desk[b-1][a+1].color == 0) {
                    desk[b-1][a+1].selected = 1;
                    desk[b-1][a+1].last.x = x;
                    desk[b-1][a+1].last.y = y;
                    a++;
                    b--;
                }
                a = x;
                b = y;
                while(a>0 && b<7 && desk[b+1][a-1].color == 0) {
                    desk[b+1][a-1].selected = 1;
                    desk[b+1][a-1].last.x = x;
                    desk[b+1][a-1].last.y = y;
                    a--;
                    b++;
                }
                a = x;
                b = y;
                while(a<7 && b<7 && desk[b+1][a+1].color == 0) {
                    desk[b+1][a+1].selected = 1;
                    desk[b+1][a+1].last.x = x;
                    desk[b+1][a+1].last.y = y;
                    a++;
                    b++;
                }
            }
        }
        if(h && color == player) {
            for (int i = 0; i < 8; i++)
                for (int e = 0; e < 8; e++) {
                    desk[i][e].selected = 0;
                    desk[i][e].s = 0;
                }
            eatf(desk,player,x,y);
        }
        if(selected) {
            if (h) {
                dia(x,y,last.x,last.y,desk);
                desk[last.y][last.x].color = 0;
                d = desk[last.y][last.x].d;
                desk[last.y][last.x].d = 0;
                desk[y][x].color = player;
                g = 1;
                for (int i = 0; i < 8; i++)
                    for (int e = 0; e < 8; e++) {
                        desk[i][e].selected = 0;
                        desk[i][e].s = 0;
                    }
                f++;
            }
            else {
                color = desk[last.y][last.x].color;
                d = desk[last.y][last.x].d;
                desk[last.y][last.x].color = 0;
                desk[last.y][last.x].d = 0;
                f++;
                for (int i = 0; i < 8; i++)
                    for (int e = 0; e < 8; e++) {
                        desk[i][e].selected = 0;
                        desk[i][e].s = 0;
                    }
            }
        }
    }
    void eatf(vector<vector<piece > > &desk,int player, int x,int y, int silent = 0) {
        if(!d) { // usual
            if (x > 1 && y > 1)
                if (desk[y - 1][x - 1].color != 0 && desk[y - 1][x - 1].color != color &&
                    desk[y - 2][x - 2].color == 0) {
                    if (!silent) desk[y - 2][x - 2].selected = 1;
                    desk[y][x].s = 1;
                    desk[y - 2][x - 2].last.x = x;
                    desk[y - 2][x - 2].last.y = y;
                    for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) desk[i][e].h = 1;

                }
            if (x < 6 && y > 1)
                if (desk[y - 1][x + 1].color != 0 && desk[y - 1][x + 1].color != color &&
                    desk[y - 2][x + 2].color == 0) {
                    if (!silent) desk[y - 2][x + 2].selected = 1;
                    desk[y][x].s = 1;
                    desk[y - 2][x + 2].last.x = x;
                    desk[y - 2][x + 2].last.y = y;
                    for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) desk[i][e].h = 1;

                }
            if (x > 1 && y < 6)
                if (desk[y + 1][x - 1].color != 0 && desk[y + 1][x - 1].color != color &&
                    desk[y + 2][x - 2].color == 0) {
                    if (!silent) desk[y + 2][x - 2].selected = 1;
                    desk[y][x].s = 1;
                    desk[y + 2][x - 2].last.x = x;
                    desk[y + 2][x - 2].last.y = y;
                    for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) desk[i][e].h = 1;

                }
            if (x < 6 && y < 6)
                if (desk[y + 1][x + 1].color != 0 && desk[y + 1][x + 1].color != color &&
                    desk[y + 2][x + 2].color == 0) {
                    if (!silent) desk[y + 2][x + 2].selected = 1;
                    desk[y][x].s = 1;
                    desk[y + 2][x + 2].last.x = x;
                    desk[y + 2][x + 2].last.y = y;
                    for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) desk[i][e].h = 1;

                }
        }
        else { // damka
            int a,b;
            a = x;
            b = y;
            while(a > 1 && b > 1 && desk[b-1][a-1].color == 0) {
                a--;
                b--;
            }
            if(a>1 && b>1 && desk[b-1][a-1].color != player && desk[b-2][a-2].color==0)  {
                if (!silent) desk[b-2][a-2].selected = 1;
                desk[y][x].s = 1;
                desk[b-2][a-2].last.x = x;
                desk[b-2][a-2].last.y = y;
                for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) desk[i][e].h = 1;
            }
            a = x;
            b = y;
            while(a < 6 && b > 1 && desk[b-1][a+1].color == 0) {
                a++;
                b--;
            }
            if(a<6 && b>1 && desk[b-1][a+1].color != player && desk[b-2][a+2].color==0)  {
                if (!silent) desk[b-2][a+2].selected = 1;
                desk[y][x].s = 1;
                desk[b-2][a+2].last.x = x;
                desk[b-2][a+2].last.y = y;
                for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) desk[i][e].h = 1;
            }
            // -------------------------------------------
            a = x;
            b = y;
            while(a > 1 && b < 6  && desk[b+1][a-1].color == 0) {
                a--;
                b++;
            }
            if(a>1 && b<6 && desk[b+1][a-1].color != player && desk[b+2][a-2].color==0)  {
                if (!silent) desk[b+2][a-2].selected = 1;
                desk[y][x].s = 1;
                desk[b+2][a-2].last.x = x;
                desk[b+2][a-2].last.y = y;
                for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) desk[i][e].h = 1;
            }
            a = x;
            b = y;
            while(a < 6 && b < 6 && desk[b+1][a+1].color == 0) {
                a++;
                b++;
            }
            if(a<6 && b<6 && desk[b+1][a+1].color != player && desk[b+2][a+2].color==0)  {
                if (!silent) desk[b+2][a+2].selected = 1;
                desk[y][x].s = 1;
                desk[b+2][a+2].last.x = x;
                desk[b+2][a+2].last.y = y;
                for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) desk[i][e].h = 1;
            }
        }
    }
    void vdamke(int y) {
        if (y==0 && color == 1)  d=1;
        else if (y==7 && color == 2)  d=1;
    }
private:
    void dia(int x1,int y1,int x2,int y2, vector<vector<piece> > &desk){
        for(int i = 0;i<8;i++) for(int e = 0;e<8;e++) {
                desk[i][e].h = 0;
                desk[i][e].selected=0;
                desk[i][e].s = 0;
            }
        while(x2!=x1 && y2 != y1) {
            desk[y1][x1].color = 0;
            if(x2>x1) x1++;
            else x1--;
            if(y2>y1) y1++;
            else y1--;
        }
    }
};

class gamec{
public:
    bool quit;
    button exitb;
    button restart;
    int player, f, eat = 0;
    vector<vector<piece> > desk;
    gamec(){

        quit = false;
        player = 1;
        int i,j;
        desk.resize(8);
        for(i=0;i<8;i++) desk[i].resize(8);

        for (j = 1; j < 8; j += 2) desk[0][j].color = 2;
        for (j = 0; j < 8; j += 2) desk[1][j].color  = 2;
        for (j = 1; j < 8; j += 2) desk[2][j].color  = 2;

        for (j = 0; j < 8; j += 2) desk[7][j].color  = 1;
        for (j = 1; j < 8; j += 2) desk[6][j].color  = 1;
        for (j = 0; j < 8; j += 2) desk[5][j].color  = 1;

        SDL_RenderClear(renderer);
        renderDesk();
    }
    void renderDesk(){
        SDL_RenderSetViewport(renderer, &background1);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, textureb, NULL, &background1);

        int i, e;
        SDL_Rect pie, coords = {0,0,100,100};
        s(pie);
        for (i = 0; i < 8; i++)
            for (e = 0; e < 8; e++) {
                coords.y = 100 * (i);
                coords.x = 100 * (e);
                if (desk[i][e].color  == 0);
                else if (desk[i][e].color == 2 && !desk[i][e].d) {
                    pie.w = 80;
                    SDL_RenderCopy(renderer, pieces, &pie, &coords);
                }// black
                else if (desk[i][e].color == 1 && !desk[i][e].d) {
                    pie.x = 80;
                    pie.w = 80;
                    SDL_RenderCopy(renderer, pieces, &pie, &coords);
                }//white
                else if (desk[i][e].color == 2) {
                    pie.y = 80;
                    pie.w = 80;
                    SDL_RenderCopy(renderer, pieces, &pie, &coords);
                }//blackD
                else if (desk[i][e].color == 1) {
                    pie.x = 80;
                    pie.w = 80;
                    pie.y = 80;
                    SDL_RenderCopy(renderer, pieces, &pie, &coords);
                }//whiteD
                s(pie);
            }
        for(i=0;i<8;i++) for(e=0;e<8;e++) if(desk[i][e].selected || desk[i][e].s) selector(e,i);
        renderBar();
        SDL_RenderPresent(renderer);
    }
    void renderBar(){
        SDL_RenderSetViewport(renderer,&background);
        SDL_RenderCopy(renderer, texturemenu, NULL, NULL);
        string out = "Белые:  ";
        stringstream ss;
        ss << out << white;
        text.renderText(ss.str());
        text.renderTextPos(20,50-text.height);
        ss.str(" ");
        out = "Черные: ";
        ss << out << black;
        text.renderText(ss.str());
        text.renderTextPos(20,90-text.height);
        ss.str(" ");
        out = "Ходов: ";
        ss << out << turns;
        text.renderText(ss.str());
        text.renderTextPos(20,800-text.height);
    }
    void selector(int x, int y) {
        SDL_Rect pie1 = {160,0,80,80}, pie2 = {x*100,y*100,100,100};
        SDL_RenderCopy(renderer, pieces, &pie1, &pie2);
        SDL_RenderPresent(renderer);
    }
    void event(){
        SDL_GetMouseState(&x, &y);
        if (endd) {
            quit = exitb.collision(x,y);
            if(restart.collision(x,y)) restartf();
        }
        else {
            x/=100;
            y/=100;
            if (x < 8 && y < 8) {
                f = player;
                desk[y][x].click(desk, player, x, y, f);
                if (f != player) nextTurn();
                if (!endd)renderDesk();
            }
        }
    }
    void nextTurn() {

        if (eat) {
            eat = 0;
            if (player == 1) white++;
            else black++;
            for (int i = 0; i < 8; i++)
                for (int e = 0; e < 8; e++)
                    if (desk[i][e].color == player && desk[i][e].g == 1)desk[i][e].eatf(desk, player, e, i, 1);
            for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) if (desk[i][e].s) eat = 1;
        }
        if (!eat) {
            if (player == 1) player = 2;
            else player = 1;
            f = 0;
            turns++;
            for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) desk[i][e].g = 0;
        }
        if (eat == 0) {
            for (int i = 0; i < 8; i++)
                for (int e = 0; e < 8; e++)
                    if (desk[i][e].color == player)
                        desk[i][e].eatf(desk, player, e, i, 1);
            for (int i = 0; i < 8; i++) for (int e = 0; e < 8; e++) if (desk[i][e].s) eat = 1;
        }
        for (int i = 0; i < 8; i++) desk[0][i].vdamke(0);
        for (int i = 0; i < 8; i++) desk[7][i].vdamke(7);
        if (white == 12 || black == 12) end();
    }
    void end(){
        endd = 1;
        SDL_Rect end = {0,200,800,400};
        SDL_RenderSetViewport(renderer,&end);
        SDL_SetRenderDrawColor(renderer, 236, 240, 241,255);
        SDL_RenderFillRect(renderer,NULL);

        string out = "победили!!!";
        stringstream ss;
        if(white==12) ss << "Белые " << out;
        else  ss << "Черные " << out;
        text.renderText(ss.str());
        text.renderTextPos(400-text.width/2,105-text.height);
        exitb.init(650,400,2);
        restart.init(150,400,3);
        SDL_RenderPresent(renderer);

    }
    void restartf(){
        white = 0;
        black = 0;
        turns = 0;
        endd = 0;
        player = 1;
        for(int i=0;i<8;i++) for(int e=0;e<8;e++) {
                desk[i][e].color = 0;
                desk[i][e].d = 0;
                desk[i][e].h = 0;
                desk[i][e].s = 0;
                desk[i][e].selected = 0;
            }
        for (int j = 1; j < 8; j += 2) desk[0][j].color = 2;
        for (int j = 0; j < 8; j += 2) desk[1][j].color  = 2;
        for (int j = 1; j < 8; j += 2) desk[2][j].color  = 2;

        for (int j = 0; j < 8; j += 2) desk[7][j].color  = 1;
        for (int j = 1; j < 8; j += 2) desk[6][j].color  = 1;
        for (int j = 0; j < 8; j += 2) desk[5][j].color  = 1;
        renderDesk();
    }


private:
    int white=0, black=0, x, y, turns=0, endd = 0;
    struct point{
        int x;
        int y;
    };
    vector <point> ways;
    point last;
    void s(SDL_Rect &pie){
        pie.x = 0;
        pie.y = 0;
        pie.w = 0;
        pie.h = 80;
    }
    SDL_Rect background = {800,0,200,1000};
    SDL_Rect background1 = {0,0,800,800};
};



void game() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, textureb, NULL, NULL);
    SDL_RenderPresent(renderer);

    gamec gameobj;

    SDL_Event e;
    while (!gameobj.quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                gameobj.quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) gameobj.event();
        }
    }
}

int main(int argc, char *args[]) {
    init();
    loadMedia();
    mainmenu();
    close();
    return 0;
}