#include <string>
#include <iostream>
#include <CLI11.h>
#include <SDL.h>
#include <SDL_image.h>
#include <src/SdlEngine.h>
#include <src/Application.h>

void application(std::string filename, std::string fontPath) {
  app::Application application = app::Application(filename, fontPath);

  // SDL_RenderClear(renderer);
  // SDL_RenderCopy(renderer, tiles, NULL, NULL);
  //SDL_RenderPresent(renderer);

  while (application.isRunning()) {
    application.processEvents();
  }
}

int main(int argc, char** argv) {
  CLI::App app { "reader for comic book zip archives" };
  std::string filename = "";
  std::string fontPath = "";
  app.add_option("-f,--file", filename, "path to the cbz file to open");
  app.add_option("-t,--ttf", fontPath, "path to font to use for menus");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  try {
    app::SdlEngine sdl = app::SdlEngine();

    // wait for dlls to load
    SDL_Delay(500);

    application(filename, fontPath);
  } catch (const std::exception& e) {
    std::cerr << "application exited with error: " << std::endl
        << e.what() << std::endl;
  }

  return 0;
}
