#include <iostream>
#include<SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;

int winWidth = 900;
int windHeight = 600;

SDL_Renderer *render;
void drawLine(){
  SDL_RenderDrawLine(render,0,0,100,100);
  SDL_RenderDrawLine(render,100,100,200,250);
}

int main(int argv, char** args) {
  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    cout <<"SDL initialization failed" << SDL_GetError() << endl;
    return EXIT_FAILURE;
  }

  if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
    cout << "SDL failed to initialize png image " << SDL_GetError() << endl;
    return EXIT_FAILURE;
  }

  SDL_Surface *surface = IMG_Load("roomPlan.png");
  SDL_Texture *background;


  SDL_Window *window = SDL_CreateWindow("Genetic Algorithm",SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,winWidth,windHeight,SDL_WINDOW_ALLOW_HIGHDPI);

  if(window == NULL){
     cout << "Could not create window" << SDL_GetError() << endl;
     return EXIT_FAILURE;
  }

  render = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

  if(render == NULL){
     cout << "Could not create renderer" << SDL_GetError() << endl;
     return EXIT_FAILURE;
  }

  background = SDL_CreateTextureFromSurface(render,surface);


 SDL_Event windowEvent;

 while(1){
   SDL_SetRenderDrawColor(render,0,0,0,255);
   SDL_RenderClear(render);

   if(SDL_PollEvent(&windowEvent)){
     if(SDL_QUIT == windowEvent.type){
       break;
     }
   }

   SDL_RenderCopy(render, background, NULL, NULL);
   drawLine();
   SDL_RenderPresent(render);
 }

 SDL_DestroyTexture(background);
 SDL_FreeSurface(surface);
 SDL_DestroyRenderer(render);
 SDL_DestroyWindow(window);
 SDL_Quit();
    return 0;
}
