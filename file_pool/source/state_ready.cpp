#include "state_ready.h"
#include "filepool_context.h"
#include <thread>

namespace file_pool {
CStateReady::CStateReady(const CStateInterface& preState) : CStateInterface(preState) {
    setStateValue(StateVale::READY);
}

CStateReady::CStateReady(const std::string uri, const std::string filePath) {
    setStateValue(StateVale::READY);
    setUri(uri);
    setFilePath(filePath);
}

}  // namespace file_pool