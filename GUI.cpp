#include <iostream>
#include<SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define numPoints 8
using namespace std;

class GUI{
  int winWidth = 900;
  int windHeight = 600;

  SDL_Renderer *render;
  SDL_Surface *surface;

  SDL_Surface *messageSurface;
  SDL_Texture *message;
  SDL_Rect textRect;

  int coordinates[numPoints] = {10,10,800,500,330,400,400,200};

public:
  GUI(){
  }
  void drawLine(vector<int>path,int length){
    cout << "testing" << endl;
    int x = 0;
    int y = 0;
    int x2 = 0;
    int y2 = 0;
    for(int i =0; i <length;i++){
      if(path[i]%2 != 0){
        x = path[i];
        y = x+1;
        i++;
      }else{
        x = path[i]+1;
        y = x+1;
        i++;
      }

      if(path[i]%2 != 0){
        x2 = path[i];
        y2 = x2+1;
      }else{
        x2 = path[i]+1;
        y2 = x2+1;
      }
      SDL_RenderDrawLine(render,x,y,x2,y2);
    }
  }

  void drawCities(){
    // TTF_Font *font = TTF_OpenFont("advanced-sans-serif-7/advanced_sans_serif_7.ttf",20);
    // SDL_Color black = {0,0,0};
    // messageSurface = TTF_RenderText_Solid(font,"testing",black);
    // message = SDL_CreateTextureFromSurface(render,messageSurface);
    // textRect.x = 5;
    // textRect.y = 5;
    // textRect.w = 20;
    // textRect.h = 20;
    //
    // SDL_QueryTexture(message,NULL,NULL,&textRect.w,&textRect.h);
    SDL_Rect rect;
    rect.w = 50;
    rect.h = 50;
    for(int i =0; i < numPoints; i++){
      rect.x = coordinates[i];
      i++;
      rect.y = coordinates[i];
      SDL_SetRenderDrawColor(render,200,0,200,255);
      SDL_RenderFillRect(render,&rect);
    }
  }

  void running() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
      cout <<"SDL initialization failed" << SDL_GetError() << endl;
      // return EXIT_FAILURE;
    }

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
      cout << "SDL failed to initialize png image " << SDL_GetError() << endl;
      // return EXIT_FAILURE;
    }

    if(TTF_Init() < 0){
      cout << "SDL failed to initialize text " << SDL_GetError() << endl;
      // return EXIT_FAILURE;
    }

    surface = IMG_Load("roomPlan.png");
    SDL_Texture *backgroundTexture;


    SDL_Window *window = SDL_CreateWindow("Genetic Algorithm",SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,winWidth,windHeight,SDL_WINDOW_ALLOW_HIGHDPI);

    if(window == NULL){
       cout << "Could not create window" << SDL_GetError() << endl;
       // return EXIT_FAILURE;
    }

    render = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    if(render == NULL){
       cout << "Could not create renderer" << SDL_GetError() << endl;
       // return EXIT_FAILURE;
    }

    backgroundTexture = SDL_CreateTextureFromSurface(render,surface);



   SDL_Event windowEvent;

   while(true){
     SDL_SetRenderDrawColor(render,0,255,255,255);
     SDL_RenderClear(render);

     if(SDL_PollEvent(&windowEvent)){
       if(SDL_QUIT == windowEvent.type){
         break;
       }
     }

     // SDL_RenderCopy(render, backgroundTexture, NULL, NULL);
     // SDL_RenderCopy(render,message,NULL,&textRect);
     drawCities();
     // drawLine();
     SDL_RenderPresent(render);
   }

   SDL_DestroyTexture(backgroundTexture);
   SDL_DestroyTexture(message);
   SDL_FreeSurface(surface);
   SDL_FreeSurface(messageSurface);
   SDL_DestroyRenderer(render);
   SDL_DestroyWindow(window);
   SDL_Quit();
      // return 0;
  }
};
// int main(int argv, char** args) {
//   // GUI gui;
//   // gui.running();
//   return 0;
// }
