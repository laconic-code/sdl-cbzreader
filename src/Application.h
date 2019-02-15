#pragma once
#include <unordered_map>
#include <algorithm>
#include <SDL.h>
#include <neither.h>
#include "SdlWindow.h"
#include "Book.h"
#include "Layout.h"
#include "Input.h"
#include "TextBox.h"

namespace app {

  enum class Key {
    Next,
    Prev,
    Exit,
    SwapOdd,
    SwapDirection,
    Fullscreen,
    GoToPage,
    ToggleStatusBar,
    Help,
  };

  class Application {
      std::string fontPath;
      bool statusBar = true;
      bool leftToRight = true;
      bool running = true;
      bool fullscreen = false;
      int page = 0;
      app::Book* book;
      app::SdlWindow* window;
      std::unordered_map<SDL_Keycode, Key> keyMap = {
          { SDLK_ESCAPE, Key::Exit },
          { SDLK_SPACE, Key::Next },
          { SDLK_b, Key::Prev },
          { SDLK_v, Key::SwapOdd },
          { SDLK_f, Key::Fullscreen },
          { SDLK_j, Key::SwapDirection },
          { SDLK_g, Key::GoToPage },
          { SDLK_s, Key::ToggleStatusBar },
          { SDLK_h, Key::Help },
      };
      std::unordered_map<Key, SDL_Keycode> reversedKeyMap;
      std::unordered_map<int, Key> mouseMap = {
          { SDL_BUTTON_LEFT, Key::Next },
          // {SDL_BUTTON_MIDDLE, Key::
          { SDL_BUTTON_RIGHT, Key::Prev },
      // SDL_BUTTON_X1
      // SDL_BUTTON_X2
          };

    public:

      Application(
          std::string path,
          std::string fontPath) {
        this->window = new SdlWindow(
            "cbzreader",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800,
            600);

        for (auto entry : this->keyMap) {
          reversedKeyMap[entry.second] = entry.first;
        }

        this->book = new Book(
            this->window->getRenderer(),
            path);

        this->fontPath = fontPath;

        this->redraw();
      }

      ~Application() {
        delete this->book;
        delete this->window;
      }

      bool isRunning() {
        return this->running;
      }

      void processEvents() {
        SDL_Event event;
        if (!SDL_WaitEvent(&event)) {
          return;
        }

        if (event.type == SDL_QUIT) {
          this->running = false;

        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
          unsigned int buttonCode = event.button.button;
          size_t count = this->mouseMap.count(buttonCode);
          if (count == 0) {
            return;
          }
          Key key = this->mouseMap[buttonCode];
          processKey(key);

          // SDL generates fake key presses if a button is held
          // the repeat field lets us know if it isn't the initial press
        } else if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
          SDL_Keycode keycode = event.key.keysym.sym;
          size_t count = this->keyMap.count(keycode);
          if (count == 0) {
            return;
          }
          Key key = this->keyMap[keycode];
          processKey(key);

        } else if (event.type == SDL_KEYUP) {
          // do nothing

        } else if (event.type == SDL_WINDOWEVENT) {
          if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            this->running = false;
          }
        }
      }

      void processKey(Key key) {
        switch (key) {
          case Key::Exit:
            this->running = false;
            break;
          case Key::Next:
            this->nextPage();
            break;
          case Key::Prev:
            this->previousPage();
            break;
          case Key::SwapOdd:
            this->swapOddPage();
            break;
          case Key::Fullscreen:
            this->toggleFullscreen();
            break;
          case Key::SwapDirection:
            this->leftToRight = !this->leftToRight;
            break;
          case Key::GoToPage:
            this->goToPage();
            break;
          case Key::ToggleStatusBar:
            this->statusBar = !this->statusBar;
            break;
          case Key::Help:
            this->showHelp();
            break;
        }
        this->redraw();
      }

      void swapOddPage() {
        this->page = (this->page % 2) == 0 ? this->page + 1 : this->page - 1;
      }

      void previousPage() {
        this->page -= 2;
      }

      void nextPage() {
        this->page += 2;
      }

      void goToPage() {
        size_t pageNumber = app::input::tryGetInt()
            .get(this->page);
        if (this->page % 2 != pageNumber % 2) {
          // the requested page doesn't match up with expected offset
          // TODO: handle this properly, lower number should be better?
          // take into account direction
          pageNumber = pageNumber + 1;
        }
        int maxPage = std::min(this->book->size(), pageNumber);
        int minPage = std::max(0, maxPage);
        this->page = minPage;
      }

      void toggleFullscreen() {
        if (this->fullscreen) {
          this->window->windowed();
        } else {
          this->window->fakeFullscreen();
        }
        this->fullscreen = !this->fullscreen;
      }

      void redraw() {

        int lIndex = this->leftToRight ? this->page : this->page + 1;
        int rIndex = this->leftToRight ? this->page + 1 : this->page;

        //SDL_SetRenderDrawColor(this->renderer, 0x00, 0x00, 0x00, 0xFF);
        Page* page1 = this->book->getPage(lIndex);
        Page* page2 = this->book->getPage(rIndex);
        SDL_Rect* src1 = page1->getSrc();
        SDL_Rect* src2 = page2->getSrc();
        SDL_Rect dst = this->window->getCanvas();
        std::pair<SDL_Rect, SDL_Rect> split = Layout::splitHorizontal(dst);
        SDL_Rect sizedl = Layout::centerVertically(
            Layout::alignRightAgainstLeft(
                Layout::scaleAspect(*src1, split.first),
                split.second),
            dst);
        SDL_Rect sizedr = Layout::centerVertically(
            Layout::alignLeftAgainstRight(
                Layout::scaleAspect(*src2, split.second),
                split.first),
            dst);

        this->window->clear();
        this->window->draw(page1->getTexture(), src1, &sizedl);
        this->window->draw(page2->getTexture(), src2, &sizedr);
        delete page1;
        delete page2;

        if (this->statusBar) {
          app::TextBox textBox = app::TextBox(this->window->getRenderer(), this->fontPath, 16);
          textBox.add(tfm::format(
            "page: %d/%d,    direction: %s,    first page: %s,    help: %s",
              this->page, this->book->size(),
              this->leftToRight ? "->" : "<-",
              this->page % 2,
              SDL_GetKeyName(this->reversedKeyMap[Key::Help])));
          RenderedText text = textBox.render();
          SDL_Rect src = { 0, 0, text.getW(), text.getH() };
          SDL_Rect location = Layout::alignToBottom(
              Layout::centerHorizontal(src, dst),
              dst);
          SDL_Rect backbarSrc = { 0, location.y, dst.w, location.h };

          SDL_SetRenderDrawBlendMode(window->getRenderer(), SDL_BLENDMODE_BLEND);
          SDL_SetRenderDrawColor(window->getRenderer(), 0, 0, 0, 150);
          SDL_RenderFillRect(window->getRenderer(), &backbarSrc);
          text.renderTo(this->window->getRenderer(), location.x, location.y);
        }
        this->window->update();
      }

      void showHelp() {
        app::TextBox names = app::TextBox(this->window->getRenderer(), this->fontPath, 16);
        names.addLines( {
            "next page:",
            "previous page:",
            "swap odd page:",
            "swap read direction:",
            "full screen toggle:",
            "go to page:",
            "toggle status bar:",
            "exit:",
            "help:",
        });
        app::TextBox buttons = app::TextBox(this->window->getRenderer(), this->fontPath, 16);
        buttons.addLines( {
            std::string(SDL_GetKeyName(this->reversedKeyMap[Key::Next])),
            std::string(SDL_GetKeyName(this->reversedKeyMap[Key::Prev])),
            std::string(SDL_GetKeyName(this->reversedKeyMap[Key::SwapOdd])),
            std::string(SDL_GetKeyName(this->reversedKeyMap[Key::SwapDirection])),
            std::string(SDL_GetKeyName(this->reversedKeyMap[Key::Fullscreen])),
            std::string(SDL_GetKeyName(this->reversedKeyMap[Key::GoToPage])),
            std::string(SDL_GetKeyName(this->reversedKeyMap[Key::ToggleStatusBar])),
            std::string(SDL_GetKeyName(this->reversedKeyMap[Key::Exit])),
            std::string(SDL_GetKeyName(this->reversedKeyMap[Key::Help])),
        });
        RenderedText namesText = names.render();
        RenderedText buttonsText = buttons.render();

        SDL_Rect namesSrc = { 0, 0, namesText.getW(), namesText.getH() };
        SDL_Rect paddingSrc = { 0, 0, 10, 0 };
        SDL_Rect buttonsSrc = { 0, 0, buttonsText.getW(), buttonsText.getH() };
        SDL_Rect panelSrc = Layout::minSpanning(
            namesSrc,
            Layout::alignLeftAgainstRight(buttonsSrc, Layout::alignLeftAgainstRight(paddingSrc, namesSrc)));

        SDL_Rect canvas = this->window->getCanvas();
        SDL_Rect centeredPanel = Layout::centerVertically(
            Layout::centerHorizontal(panelSrc, canvas),
            canvas);
        SDL_Rect namesLoc = Layout::alignToTop(
            Layout::alignToLeft(namesSrc, centeredPanel),
            centeredPanel);
        SDL_Rect paddingLoc = Layout::alignToTop(
            Layout::alignLeftAgainstRight(paddingSrc, namesLoc),
            namesLoc);
        SDL_Rect buttonsLoc = Layout::alignToTop(
            Layout::alignLeftAgainstRight(buttonsSrc, paddingLoc),
            paddingLoc);

        int xPad = centeredPanel.w * 0.05;
        int yPad = centeredPanel.h * 0.05;
        SDL_Rect backgroundLoc = {
            centeredPanel.x - xPad,
            centeredPanel.y - yPad,
            centeredPanel.w * 1.1,
            centeredPanel.h * 1.1
        };

        SDL_SetRenderDrawBlendMode(window->getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(window->getRenderer(), 0, 0, 0, 255);
        SDL_RenderFillRect(window->getRenderer(), &backgroundLoc);
        namesText.renderTo(this->window->getRenderer(), namesLoc.x, namesLoc.y);
        buttonsText.renderTo(this->window->getRenderer(), buttonsLoc.x, buttonsLoc.y);
        this->window->update();
        app::waitForAnyKey();
      }

  };

}

