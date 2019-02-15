#pragma once
#include <exception>
#include <string>

namespace app {

  class Exception: public std::exception {
    private:
      std::string msg;

    public:

      Exception(std::exception cause, std::string msg) :
          std::exception(cause), msg(msg) {
      }

      Exception(std::exception cause) :
          std::exception(cause) {
      }

      Exception(std::string msg) :
          msg(msg) {
      }

      const char* what() const noexcept {
        return msg.c_str();
      }
  };

  class IOException: app::Exception {
    public:
      IOException(std::exception cause, std::string msg) :
          app::Exception(cause, msg) {
      }

      IOException(std::exception cause) :
          app::Exception(cause) {
      }

      IOException(std::string msg) :
          app::Exception(msg) {
      }

  };

}
