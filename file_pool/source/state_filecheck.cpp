#include "state_none.h"
#include "state_downloading.h"
#include "state_filecheck.h"
#include "state_ready.h"
#include "filepool_context.h"
#include <thread>
#include <fstream>

namespace file_pool {
CStateFileCheck::CStateFileCheck(const CStateInterface& preState) : CStateInterface(preState) {
    setStateValue(StateVale::CHECKING);
}

int CStateFileCheck::checkFileSize(long exceptSize) {
    getLogger()->info("检查文件大小,{}", getFilePath());
    if (exceptSize == -1) {
        getLogger()->info("忽略文件检查\n");
        //保存记录
        saveFileState();
        CStateInterface* nextState = new CStateReady(*this);
        if (changeFileState(nextState) != 0) {
            return -1;
        }
        return 0;
    }
    std::ifstream   ifs(getFilePath());
    if (!ifs) {
        getLogger()->error("打开文件失败");
        return -3;
    }
    // Some work ...
    ifs.seekg(0, std::ios::end);
    size_t dstFileSize = ifs.tellg();
    ifs.close();

    if (dstFileSize != exceptSize) {
        getLogger()->error("文件大小不符,realSize:{},exceptSize:{}\n", dstFileSize, exceptSize);
        CStateInterface* nextState = new CStateNone(*this);
        if (changeFileState(nextState) != 0) {
            return -1;
        }
    }
    else {
        getLogger()->info("文件合法\n");
        //保存记录
        saveFileState();
        CStateInterface* nextState = new CStateReady(*this);
        if (changeFileState(nextState) != 0) {
            return -1;
        }
    }
    return 0;
}
}  // namespace file_pool