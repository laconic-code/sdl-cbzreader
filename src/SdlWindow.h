#pragma once
#include <string>
#include <SDL.h>

namespace app {

  class SdlWindow {
    private:
      SDL_Window* window;
      SDL_Renderer* renderer;
    public:
      SdlWindow(
          std::string windowName,
          int x, int y, int w, int h) {
        this->window = SDL_CreateWindow(
            windowName.c_str(),
            x, y, w, h,
            SDL_WINDOW_SHOWN);
        this->renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED);
      }

      ~SdlWindow() {
        SDL_DestroyRenderer(this->renderer);
        SDL_DestroyWindow(this->window);
      }

      SDL_Rect getScreen() {
        int x, y, w, h;
        SDL_GetWindowPosition(this->window, &x, &y);
        SDL_GetWindowSize(this->window, &w, &h);
        return {x, y, w, h};
      }

      SDL_Rect getCanvas() {
        int w, h;
        SDL_GetWindowSize(this->window, &w, &h);
        return {0, 0, w, h};
      }

      void fullscreen() {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_Rect screen = this->getScreen();
        SDL_RenderSetLogicalSize(this->renderer, screen.w, screen.h);
      }

      void fakeFullscreen() {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_Rect screen = this->getScreen();
        SDL_RenderSetLogicalSize(this->renderer, screen.w, screen.h);
      }

      void windowed() {
        SDL_SetWindowFullscreen(window, 0);
        SDL_Rect screen = this->getScreen();
        SDL_RenderSetLogicalSize(this->renderer, screen.w, screen.h);
      }

      void clear() {
        SDL_RenderClear(renderer);
      }

      void update() {
        SDL_RenderPresent(renderer);
        SDL_UpdateWindowSurface(window);
      }

      SDL_Renderer* getRenderer() {
        return this->renderer;
      }

      void draw(
          SDL_Texture* src,
          SDL_Rect* srcBound,
          SDL_Rect* dstBound) {
        SDL_RenderCopy(
            this->renderer,
            src,
            srcBound,
            dstBound);
      }
  };

}



