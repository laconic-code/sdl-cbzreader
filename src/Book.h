#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <libzippp.h>
#include "Page.h"


namespace app {

  class Book {
    private:
      SDL_Renderer* renderer;
      libzippp::ZipArchive* file;
      std::string path;
      std::vector<std::string> files;
      std::vector<Page*> pages;

    public:
      Book(SDL_Renderer* renderer, std::string path) {
        this->path = path;
        this->renderer = renderer;
        this->file = new libzippp::ZipArchive(path);
        this->file->open(libzippp::ZipArchive::READ_ONLY);

        std::vector<libzippp::ZipEntry> entries = this->file->getEntries();
        std::vector<libzippp::ZipEntry>::iterator it;
        for (it = entries.begin(); it != entries.end(); ++it) {
          libzippp::ZipEntry entry = *it;
          this->files.push_back(entry.getName());
        }
        std::sort(this->files.begin(), this->files.end());
      }

      ~Book() {
        this->file->close();
      }

      size_t size() {
        return this->files.size();
      }

      Page* getPage(size_t pageNumber) {
        libzippp::ZipEntry entry = this->file->getEntry(this->files.at(pageNumber));
        int size = entry.getSize();
        void* binaryData = entry.readAsBinary();
        // TODO track and init only once
        // FIXME leak
        return app::Page::fromMemory(this->renderer, binaryData, size);
      }
  };
}


