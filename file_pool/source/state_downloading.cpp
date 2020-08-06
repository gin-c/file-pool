#include "state_downloading.h"
#include "state_filecheck.h"
#include "filepool_context.h"
#include <thread>
#include <chrono>

namespace file_pool {
CStateDownloading::CStateDownloading(const CStateInterface& preState) : CStateInterface(preState) {
    setStateValue(StateVale::DOWNLOADING);
}

CStateDownloading::CStateDownloading(std::string uri) {
    setUri(uri);
    setStateValue(StateVale::DOWNLOADING);
}

int CStateDownloading::downloadFile(long exceptSize) {
    getLogger()->info("下载文件中,uri:{}", getUri());

    // 生成文件下载名称: 192.168.9.247:9081/3,54f963fd205b.jpg  --> 3,54f963fd205b.jpg
    std::string& uri = getUri();
    std::string filePath = getContext()->getFilePoolDir() + "/" + uri.substr(uri.find_last_of('/') + 1);

    if (getContext()->DownloadFunction(uri, filePath) == false) {
        getLogger()->error("下载文件失败:{}", uri);
        return 1;
    }
    getLogger()->info("下载文件完成,path:{}", filePath);
    setFilePath(filePath);

    CStateInterface* nextState = new CStateFileCheck(*this);
    if (changeFileState(nextState) != 0) {
        return -1;
    }
    return nextState->checkFileSize(exceptSize);
}

}  // namespace file_pool