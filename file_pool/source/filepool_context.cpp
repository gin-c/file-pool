#include "filepool_context.h"
#include "state_interface.h"
#include "state_downloading.h"
#include "state_ready.h"
#include <fstream>

namespace file_pool {
CFilePoolContext::CFilePoolContext() : m_pLogger(nullptr), m_recordFilePath(), m_filePoolDir() {}

CFilePoolContext::~CFilePoolContext() {
    for (auto iter : m_fileState) {
        delete iter.second;
    }
    m_fileState.clear();
}

int CFilePoolContext::init(IN const std::string& filePoolDir, void* userData, IN std::shared_ptr<spdlog::logger> pLogger) {
    m_filePoolDir    = filePoolDir;
    m_recordFilePath = filePoolDir + "/" + "file_record.text";
    m_userData       = userData;
    m_pLogger        = pLogger;
    // 读文件
    std::ifstream ifs(m_recordFilePath, std::fstream::in);
    if (ifs.is_open())  // 有该文件
    {
        std::string line;
        getLogger()->info("开始导入文件记录");
        while (std::getline(ifs, line))  // line中不包括每行的换行符
        {
            std::string uri;
            std::string filePath;

            size_t iPos = line.find_first_of("-->");
            uri.assign(line.substr(0, iPos));
            filePath.assign(line.substr(iPos + 3));
            changeFileState(uri, new CStateReady(uri, filePath));
        }
        ifs.close();
    }
    else  // 没有该文件
    {
        std::ofstream out;
        out.open(m_recordFilePath);
        if (out.is_open()) {
            getLogger()->info("创建文件成功:{}", m_recordFilePath);
            out.close();
        }
        else {
            getLogger()->info("创建文件失败:{}", m_recordFilePath);
        }
    }
    return 0;
}

int CFilePoolContext::changeFileState(IN const std::string& uri, IN CStateInterface* state) {
    std::lock_guard<std::mutex> locker(m_fileStateMutex);
    auto                        iter = m_fileState.find(uri);
    if (iter != m_fileState.end()) {
        if (iter->second->getStateValue() == state->getStateValue()) {
            return 1;
        }
        delete iter->second;
        iter->second = state;
    }
    else {
        m_fileState.insert(std::make_pair(uri, state));
    }
    state->setContext(this);
    return 0;
}

int CFilePoolContext::saveFileState(IN CStateInterface* state) {
    if (state == nullptr) {
        return -1;
    }
    getLogger()->info("保存下发完成的记录,uri:{},filePath:{}", state->getUri(), state->getFilePath());

    std::ofstream ofs(m_recordFilePath, std::ios::app);
    if (ofs) {
        ofs << state->getUri() << "-->" << state->getFilePath() << std::endl;
    }
    else {
        getLogger()->error("没有文件:{}", m_recordFilePath);
    }

    return 0;
}

bool CFilePoolContext::DownloadFunction(IN const std::string& downloadUri,IN const std::string& filePath) {
    return downloadFunction(downloadUri, filePath, m_userData);
}

std::shared_ptr<spdlog::logger> CFilePoolContext::getLogger() {
    return m_pLogger;
}

CStateInterface* CFilePoolContext::getFileState(IN const std::string& uri) {
    std::lock_guard<std::mutex> locker(m_fileStateMutex);

    auto iter = m_fileState.find(uri);
    if (iter != m_fileState.end()) {
        return iter->second;
    }
    return nullptr;
}

StateVale CFilePoolContext::getFileStateValue(IN const std::string& uri) {
    CStateInterface* stateInterFace = getFileState(uri);
    if (stateInterFace != nullptr) {
        return stateInterFace->getStateValue();
    }
    return StateVale::NONE;
}

void* CFilePoolContext::getUserData() {
    return m_userData;
}

std::string CFilePoolContext::getFilePoolDir() {
    return m_filePoolDir;
}

int CFilePoolContext::downloadFile(IN const std::string& uri,IN long exceptSize) {
    CStateInterface* stateInterFace = getFileState(uri);
    if (stateInterFace == nullptr) {
        stateInterFace = new CStateDownloading(uri);
        if (changeFileState(uri, stateInterFace) != 0) {
            return 0;
        }
    }
    // 多线程会同时进入downloadFile，注意前面的状态已失效
    return stateInterFace->downloadFile(exceptSize);
}

StateVale CFilePoolContext::getFileNoBlock(IN const std::string& uri, IN long exceptSize, std::string& filePath) {
    if (getFileStateValue(uri) == StateVale::NONE) {
        if (downloadFile(uri, exceptSize) != 0) {
            return getFileStateValue(uri);
        }
    }

    if (getFileStateValue(uri) == StateVale::READY) {
        filePath = getFileState(uri)->getFilePath();
        return READY;
    }
    return getFileStateValue(uri);
}

StateVale CFilePoolContext::getFileBlock(IN const std::string& uri, IN long exceptSize, OUT std::string& filePath) {
    StateVale stateValue;
    auto      lamda = [&]() {
        int seconds = 30;
        do {
            getLogger()->debug("getFileBlock 获取文件");
            stateValue = getFileNoBlock(uri, exceptSize, filePath);
            if (stateValue == StateVale::NONE) {
                getLogger()->debug("文件获取失败");
                break;
            }
            else if (stateValue == StateVale::DOWNLOADING) {
                getLogger()->debug("文件下载中");
            }
            else if (stateValue == StateVale::CHECKING) {
                getLogger()->debug("文件校验中");
            }
            else if (stateValue == StateVale::READY) {
                getLogger()->debug("文件读取成功");
                break;
            }
            else {
                spdlog::debug("非法状态");
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            seconds--;
        } while (seconds > 0);
    };
    std::thread(lamda).join();
    return stateValue;
}

}  // namespace file_pool