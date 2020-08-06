#pragma once
#include "filepool_context.h"

namespace iotas_acd {

class CFilePoolImpl : public file_pool::CFilePoolContext {
  public:
    bool downloadFunction(const std::string& downloadUri, const std::string& filePath, void* userData) override {
        spdlog::debug("HttpÏÂÔØÎÄ¼ş...");

        std::this_thread::sleep_for(std::chrono::seconds(6));
        return true;
    }
};
}  // namespace iotas_acd