#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>

namespace app {

  class TTFException: app::Exception {
    public:
      TTFException(std::exception cause, std::string msg) :
          app::Exception(cause, msg) {
      }

      TTFException(std::exception cause) :
          app::Exception(cause) {
      }

      TTFException(std::string msg) :
          app::Exception(msg) {
      }
  };

  class RenderedText {
    private:
      std::vector<std::tuple<SDL_Texture*, int, int>> lines;

    public:

      RenderedText(std::vector<std::tuple<SDL_Texture*, int, int>> lines) {
        this->lines = lines;
      }

      ~RenderedText() {
        for (auto line : this->lines) {
          SDL_DestroyTexture(std::get<0>(line));
        }
      }

      int getW() {
        int wMax = 0;
        for (auto entry : this->lines) {
          wMax = std::max(wMax, std::get<1>(entry));
        }
        return wMax;
      }

      int getH() {
        int hSum = 0;
        for (auto entry : this->lines) {
          hSum += std::get<2>(entry);
        }
        return hSum;
      }

      void renderTo(SDL_Renderer* renderer, int x, int y) {
        int deltaY = 0;
        for (auto line : this->lines) {
          int w = std::get<1>(line);
          int h = std::get<2>(line);
          SDL_Rect src = { 0, 0, w, h };
          SDL_Rect dst = { x, y + deltaY, w, h };
          SDL_RenderCopy(renderer, std::get<0>(line), &src, &dst);
          deltaY += h;
        }
      }

  };

  class TextBox {
    private:
      std::vector<std::string> text;
      SDL_Renderer* renderer;
      TTF_Font* font;
      SDL_Color color;

      // TODO: if we know the window height, we can make sure text wraps correctly

    public:

      TextBox(
          SDL_Renderer* renderer,
          std::string fontPath,
          size_t point,
          SDL_Color color = { 255, 255, 255, 0 }) {
        this->renderer = renderer;
        this->color = color;
        this->font = TTF_OpenFont(fontPath.c_str(), point);
        if (this->font == NULL) {
          throw TTFException(tfm::format(
              "Failed to open font file, reason: %s",
              TTF_GetError()));
        }
      }

      ~TextBox() {
        TTF_CloseFont(this->font);
      }

      void addLines(std::vector<std::string> lines) {
        for (std::string line : lines) {
          addLine(line);
        }
      }

      void addLine(std::string text) {
        this->text.push_back(text);
      }

      void add(std::string text) {
        if (this->text.empty()) {
          this->text.push_back(text);
        } else {
          std::string modify = this->text.back();
          this->text.pop_back();
          this->text.push_back(modify + text);
        }
      }

      RenderedText render() {
        std::vector<std::tuple<SDL_Texture*, int, int>> lines;
        for (std::string line : this->text) {
          SDL_Surface* textSurface = TTF_RenderText_Blended(this->font, line.c_str(), this->color);
          SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
          int w = textSurface->w;
          int h = textSurface->h;
          SDL_FreeSurface(textSurface);
          lines.push_back(std::tuple<SDL_Texture*, int, int> { texture, w, h });
        }

        return RenderedText(lines);
      }
  };
}


