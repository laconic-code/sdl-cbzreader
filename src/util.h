#pragma once
#include <string>
#include <vector>
#include <locale>
#include <sstream>
#include <neither.h>

namespace app {

  neither::Maybe<bool> tryParseBool(std::string input) {
    try {
      return {"true" == input || "True" == input};
    } catch (const std::invalid_argument& e) {
      return {};
    } catch (const std::out_of_range& e) {
      return {};
    }
  }

  neither::Maybe<int> tryParseInt(std::string input) {
    try {
      return {std::stoi(input)};
    } catch (const std::invalid_argument& e) {
      return {};
    } catch (const std::out_of_range& e) {
      return {};
    }
  }

  neither::Maybe<double> tryParseDouble(std::string input) {
    try {
      return {std::stod(input)};
    } catch (const std::invalid_argument& e) {
      return {};
    } catch (const std::out_of_range& e) {
      return {};
    }
  }

  std::vector<std::string> splitSpaceSeparated(std::string input) {
    std::string buf;
    std::stringstream ss(input);
    std::vector<std::string> tokens;
    while (ss >> buf)
      tokens.push_back(buf);
    return tokens;
  }

  template<typename T, typename U>
  constexpr std::vector<U> map(
      const std::vector<T>& input,
      const std::function<U(T)> f) {
    std::vector<U> result = std::vector < U > (input.size());
    for (T e : input) {
      result.push_back(f(e));
    }
    return result;
  }

  template<typename T, typename U>
  constexpr neither::Maybe<std::vector<U>> tryMap(
      const std::vector<T>& input,
      const std::function<neither::Maybe<U>(T)> f) {
    std::vector<U> result = std::vector < U > (input.size());
    for (T e : input) {
      neither::Maybe<U> r = f(e);
      if (!r.hasValue) {
        return {};
      }
      result.push_back(r.unsafeGet());
    }
    return {result};
  }

  std::string join(std::string join, std::vector<std::string> args) {
    std::stringstream stream;
    for (size_t i = 0; i < args.size() - 1; i++) {
      stream << args[i] << join;
    }
    if (args.empty()) {
      stream << args[args.size() - 1];
    }
    return stream.str();
  }

  void waitForAnyKey() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      // clear the event queue
    }

    while (SDL_WaitEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
        return;
    }
  }
}

}


