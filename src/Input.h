#pragma once
#include <string>
#include <neither.h>
#include <src/util.h>
#include <SDL.h>
#include "util.h"

namespace app {
  namespace input {

    std::string readString() {
      std::string input = "";
      SDL_StartTextInput();
      SDL_Event event;

      while (SDL_PollEvent(&event)) {
        // clear the event queue
      }

      while (SDL_WaitEvent(&event)) {
        bool control = SDL_GetModState() & KMOD_CTRL;
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
          SDL_Keycode keycode = event.key.keysym.sym;
          if (keycode == SDLK_RETURN) {
            break;
          } else if (keycode == SDLK_BACKSPACE && input.length() > 0) {
            input.pop_back();
          } else if (keycode == SDLK_c && control) {
            SDL_SetClipboardText(input.c_str());
          } else if (keycode == SDLK_v && control) {
            input = SDL_GetClipboardText();
          }
        } else if (event.type == SDL_TEXTINPUT) {
          char firstChar = event.text.text[0];
          bool copy = (firstChar == 'c' || firstChar == 'C') && control;
          bool paste = (firstChar == 'v' || firstChar == 'V') && control;
          if (!copy && !paste) {
            input += event.text.text;
          }
        }
      }
      printf(("input: " + input + "\n").c_str());
      SDL_StopTextInput();
      return input;
    }

    neither::Maybe<int> tryGetInt() {
      return app::tryParseInt(readString());
    }

  }
}

