#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <tinyformat.h>
#include "Exception.h"

namespace app {

  class SDLException: app::Exception {
    public:
      SDLException(std::exception cause, std::string msg) :
          app::Exception(cause, msg) {
      }

      SDLException(std::exception cause) :
      app::Exception(cause) {
      }

      SDLException(std::string msg) :
          app::Exception(msg) {
      }
  };

  class SdlEngine {
    public:
      SdlEngine() {
        int result = SDL_Init(SDL_INIT_VIDEO);
        if (result < 0) {
          throw SDLException(tfm::format(
              "failed to initialize sdl. result: %s, reason: %s",
              result, SDL_GetError()));
        }

        // IMG_INIT_TIF
        // IMG_INIT_WEBP
        int imageFlags = IMG_INIT_PNG | IMG_INIT_JPG;
        result = IMG_Init(imageFlags);
        if (imageFlags != result) {
          throw SDLException(tfm::format(
              "failed to initialize sdl image. result: %s (expected: %s), reason: %s",
              result, imageFlags, IMG_GetError()));
        }

        if (TTF_Init() == -1) {
          throw SDLException(tfm::format(
              "failed to initialize sdl fonts. reason: %s",
              TTF_GetError()));
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

      }

      ~SdlEngine() {
        IMG_Quit();
        SDL_Quit();
      }
  };

}
