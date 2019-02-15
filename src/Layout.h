#pragma once
#include <vector>
#include <algorithm>
#include <utility>
#include <SDL.h>

namespace app {

  enum class Align {
    Center,
    Left,
    Right,
    Top,
    Bottom,
  };

  enum class Scale {
    None,
    RetainAspect,
    Fill,
    FillWidth,
    FillHeight,
  };

  class Layout {
    private:
    public:

      /**
       * Split the given space in half horizontally
       * The resulting rectangles will cover the same space and not overlap
       * first: left
       * second: right
       */
      static std::pair<SDL_Rect, SDL_Rect> splitHorizontal(const SDL_Rect& space) {
        SDL_Rect left = { space.x, space.y, space.w / 2.0, space.h };
        SDL_Rect right = { space.x + left.w, space.y, space.w / 2.0, space.h };
        return std::make_pair(left, right);
      }

      /**
       * Split the given space in half vertically
       * The resulting rectangles will cover the same space and not overlap
       * first: top
       * second: bottom
       */
      static std::pair<SDL_Rect, SDL_Rect> splitVertical(const SDL_Rect& space) {
        SDL_Rect upper = { space.x, space.y, space.w, space.h / 2.0 };
        SDL_Rect lower = { space.x, space.y + upper.h, space.w, space.h / 2.0 };
        return std::make_pair(upper, lower);
      }

      /**
       * Create a space that spans the area covered by both spaces
       *  ------        --------
       *  |    |        |      |
       *  |  -----  ->  |      |
       *  |  |   |      |      |
       *  ---|   |      |      |
       *     -----      --------
       */
      static SDL_Rect minSpanning(const SDL_Rect& first, const SDL_Rect& second) {
        int topLeftX = std::min(first.x, second.x);
        int topLeftY = std::min(first.y, second.y);
        int bottomRightX = std::max(first.x + first.w, second.x + second.w);
        int bottomRightY = std::max(first.y + first.h, second.y + second.h);
        return {topLeftX, topLeftY, bottomRightX - topLeftX, bottomRightY - topLeftY};
      }

      //  /**
      //   * Create a space that spans the area covered by both spaces
      //   *  ------
      //   *  |    |
      //   *  |  -----  ->     ---
      //   *  |  |   |         | |
      //   *  ---|   |         ---
      //   *     -----
      //   */
      //  static SDL_Rect minOverlap(SDL_Rect* first, SDL_Rect* second) {
      //  }

      /**
       * Scale src to fit maximally within dst while maintaining aspect ratio
       */
      static SDL_Rect scaleAspect(const SDL_Rect& src, const SDL_Rect& dst) {
        double srcW = src.w;
        double dstW = dst.w;
        double srcH = src.h;
        double dstH = dst.h;
        double ratio = std::min(dstW / srcW, dstH / srcH);
        int scaledW = src.w * ratio;
        int scaledH = src.h * ratio;
        return {dst.x, dst.y, scaledW, scaledH};
      }

      /**
       * align the left side of src to the left side of dst
       */
      static SDL_Rect alignToLeft(const SDL_Rect& src, const SDL_Rect& dst) {
        return {dst.x, src.y, src.w, src.h};
      }

      /**
       * align the right side of src to the right side of dst
       */
      static SDL_Rect alignToRight(const SDL_Rect& src, const SDL_Rect& dst) {
        int dstRight = dst.x + dst.w;
        int srcRight = src.x + src.w;
        int delta = dstRight - srcRight;
        return {src.x + delta, src.y, src.w, src.h};
      }

      /**
       * align the top side of src to the top of dst
       */
      static SDL_Rect alignToTop(const SDL_Rect& src, const SDL_Rect& dst) {
        return {src.x, dst.y, src.w, src.h};
      }

      /**
       * align the bottom side of src to the bottom of dst
       */
      static SDL_Rect alignToBottom(const SDL_Rect& src, const SDL_Rect& dst) {
        int dstBottom = dst.y + dst.h;
        int srcBottom = src.y + src.h;
        int delta = dstBottom - srcBottom;
        return {src.x, src.y + delta, src.w, src.h};
      }

      /**
       * align the left side of src to the right side of dst
       *
       * dst|src
       */
      static SDL_Rect alignLeftAgainstRight(const SDL_Rect& src, const SDL_Rect& dst) {
        int dstRight = dst.x + dst.w;
        int srcLeft = src.x;
        int delta = dstRight - srcLeft;
        return {src.x + delta, src.y, src.w, src.h};
      }

      /**
       * align the right side of src to the left side of dst
       *
       * src|dst
       */
      static SDL_Rect alignRightAgainstLeft(const SDL_Rect& src, const SDL_Rect& dst) {
        int dstLeft = dst.x;
        int srcRight = src.x + src.w;
        int delta = dstLeft - srcRight;
        return {src.x + delta, src.y, src.w, src.h};
      }

      /**
       * align the top side of src to the bottom side of dst
       *   dst
       *  -----
       *   src
       */
      static SDL_Rect alignTopAgainstBottom(const SDL_Rect& src, const SDL_Rect& dst) {
        int dstBottom = dst.y + dst.h;
        int srcTop = src.y;
        int delta = dstBottom - srcTop;
        return {src.x, src.y + delta, src.w, src.h};
      }

      /**
       * align the bottom side of src to the top side of dst
       *   src
       *  -----
       *   dst
       */
      static SDL_Rect alignBottomAgainstTop(const SDL_Rect& src, const SDL_Rect& dst) {
        int dstTop = dst.y;
        int srcBottom = src.y + src.h;
        int delta = dstTop - srcBottom;
        return {src.x, src.y + delta, src.w, src.h};
      }

      /**
       * Center src horizontally within dst
       */
      static SDL_Rect centerHorizontal(const SDL_Rect& src, const SDL_Rect& dst) {
        int dstCenter = dst.x + (dst.w / 2);
        int srcCenter = src.x + (src.w / 2);
        int delta = dstCenter - srcCenter;
        return {src.x + delta, src.y, src.w, src.h};
      }

      /**
       * Center src vertically within dst
       */
      static SDL_Rect centerVertically(const SDL_Rect& src, const SDL_Rect& dst) {
        int dstCenter = dst.y + (dst.h / 2);
        int srcCenter = src.y + (src.h / 2);
        int delta = dstCenter - srcCenter;
        return {src.x, src.y + delta, src.w, src.h};
      }
  };
}


