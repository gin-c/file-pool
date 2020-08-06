#include "state_none.h"
#include "state_downloading.h"
#include "filepool_context.h"
#include <thread>

namespace file_pool {
CStateNone::CStateNone(const CStateInterface& preState) : CStateInterface(preState) {
    setStateValue(StateVale::NONE);
}

CStateNone::CStateNone(std::string uri) {
    setStateValue(StateVale::NONE);
    setUri(uri);
}

int CStateNone::downloadFile(long exceptSize) {
    getLogger()->debug("没有文件");
    CStateInterface* nextState = new CStateDownloading(*this);
    if (changeFileState(nextState) != 0) {
        return -1;
    }
    return nextState->downloadFile(exceptSize);
}
}  // namespace file_pool