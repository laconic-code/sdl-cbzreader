#pragma once
#include <string>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include <tinyformat.h>
#include "Exception.h"

namespace app {

  class ImageOpenException: app::IOException {
    public:
      ImageOpenException(std::exception cause, std::string msg) :
          app::IOException(cause, msg) {

      }

      ImageOpenException(std::string msg) :
          app::IOException(msg) {

      }
  };

  class Page {
    private:
      SDL_Surface* surface;
      SDL_Texture* texture;
      SDL_Rect* src;

    public:

      Page(SDL_Renderer* renderer, SDL_Surface* surface) {
        this->surface = surface;
        this->texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (this->texture == NULL) {
          SDL_FreeSurface(this->surface);
          throw ImageOpenException(tfm::format(
              "Failed to create surface, reason: %s",
              SDL_GetError()));
        }

        this->src = new SDL_Rect();
        this->src->x = 0;
        this->src->y = 0;
        this->src->w = this->surface->w;
        this->src->h = this->surface->h;
      }

      ~Page() {
        SDL_DestroyTexture(this->texture);
        SDL_FreeSurface(this->surface);
        delete this->src;
      }

      static Page* fromFile(SDL_Renderer* renderer, std::string path) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (surface == NULL) {
          const char* reason = IMG_GetError();
          throw ImageOpenException(tfm::format(
              "Failed to open file %s, reason: %s",
              path, reason));
        }

        return new Page(renderer, surface);
      }

      static Page* fromMemory(SDL_Renderer* renderer, void* memory, size_t length) {
        SDL_RWops* mem = SDL_RWFromConstMem(memory, length);
        SDL_Surface* surface = IMG_Load_RW(mem, 0);
        if (surface == NULL) {
          delete[] memory;
          const char* reason = IMG_GetError();
          throw ImageOpenException(tfm::format(
              "Failed to open surface from memory, reason: %s",
              reason));
        }

        return new Page(renderer, surface);
      }

      SDL_Texture* getTexture() {
        return this->texture;
      }

      SDL_Rect* getSrc() {
        return this->src;
      }
  };

}
