#include "filepool_impl.h"
#include "state_none.h"
#include <thread>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

using namespace file_pool;

bool createDir(const std::string& szPath) {
#ifdef _WIN32
    WIN32_FIND_DATA fd;
    HANDLE          hFind = ::FindFirstFile(szPath.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            return true;
        return false;
    }
    // not exist
    if (!::CreateDirectory(szPath.c_str(), NULL)) {
        return false;
    }
    return true;
#else
    return 0 == mkdir(szPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

// 多线程共同下发同一个文件,文件大小不符，尝试多次
void test1(file_pool::CFilePoolContext& filePoolContext) {
    auto lamda = [&filePoolContext](int index) {
        std::string uri        = "http://192.168.9.247:9081/3,54f963fd205b.jpg";
        long        exceptSize = 92978;
        for (int tries = 3; tries > 0;) {
            spdlog::debug("### 获取文件");
            std::string uri = "http://192.168.9.247:9081/3,54f963fd205b.jpg";
            std::string filePath;
            StateVale   stateValue = filePoolContext.getFileBlock(uri, exceptSize, filePath);
            if (stateValue == StateVale::NONE) {
                spdlog::debug("### 获取文件失败,第{}次", 4 - tries);
                tries--;
                exceptSize++;
            }
            else if (stateValue == StateVale::READY) {
                spdlog::debug("文件读取成功:{}", filePath);
                return;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };
    std::thread(lamda, 1).detach();
    std::thread(lamda, 2).detach();
    std::thread(lamda, 3).detach();
}

// 多线程下载多个文件
void test2(file_pool::CFilePoolContext& filePoolContext) {
    auto lamda = [&filePoolContext](int index, std::string uri) {
        // 正确大小999
        long exceptSize = 92980;
        for (int tries = 3; tries > 0;) {
            spdlog::debug("### 获取文件");
            std::string filePath;
            StateVale   stateValue = filePoolContext.getFileBlock(uri, exceptSize, filePath);
            if (stateValue == StateVale::NONE) {
                spdlog::debug("### 获取文件失败,第{}次", 4 - tries);
                tries--;
                exceptSize++;
            }
            else if (stateValue == StateVale::READY) {
                spdlog::debug("文件读取成功:{}", filePath);
                return;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };
    std::thread(lamda, 1, "http://1.jpg").detach();
    std::thread(lamda, 2, "http://2.jpg").detach();
    std::thread(lamda, 3, "http://3.jpg").detach();
}


// 多线程共同下发同一个文件,阻塞
void testblock(file_pool::CFilePoolContext& filePoolContext) {
    auto lamda = [&filePoolContext]() {
        std::string uri = "http://192.168.9.247:9081/3,54f963fd205b.jpg";

        spdlog::debug("阻塞获取测试");
        std::string filePath;
        StateVale   stateValue = filePoolContext.getFileBlock(uri, -1, filePath);
        if (stateValue == StateVale::NONE) {
            spdlog::debug("获取文件失败");
            return;
        }
        else if (stateValue == StateVale::READY) {
            spdlog::debug("文件读取成功:{}", filePath);
            return;
        }
    };
    std::thread(lamda).detach();
    std::thread(lamda).detach();
    std::thread(lamda).detach();
}

int main(int argc, char* argv[]) {

    std::string projectUuid = "efd8fcf58f734ddeba9d4409180a6ac0";
    createDir(projectUuid);
    {
        createDir("logs");
        std::string logDir = std::string("logs/") + projectUuid;
        if (!createDir(logDir)) {
            return -1;
        }
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);

        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(logDir + "/daily.log", 0, 0, false, 10);
        file_sink->set_level(spdlog::level::trace);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        auto                          logger = std::make_shared<spdlog::logger>("TEST", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::debug);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S] [t %t] [%n] [%l] %v");
        spdlog::set_default_logger(logger);
    }
    auto filePoolLogger = spdlog::default_logger()->clone("file_pool");

    // 创建文件夹
    std::string testfilepoolDir = projectUuid + "/testfilepool";
    createDir(testfilepoolDir);

    file_pool::CFilePoolContext* pFilePoolContext = new iotas_acd::CFilePoolImpl();
    pFilePoolContext->init(testfilepoolDir, nullptr, filePoolLogger);

    testblock(*pFilePoolContext);
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    return 0;
}
