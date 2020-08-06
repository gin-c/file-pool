# filepool
线程安全的文件下载缓存池，如果文件已经下载完成过，则会直接使用已经下载好的。

## 使用示例

```c++
#include "filepool_context.h"

// 重写downloadFunction下载函数
class CFilePoolImpl : public file_pool::CFilePoolContext {
    public:
    bool downloadFunction(const std::string& downloadUri, const std::string& filePath, void* userData) override {
        spdlog::debug("自定义的下载文件方法");
        return true;
    }
};

int main(int argc, char* argv[]) {
    // 创建实例
    file_pool::CFilePoolContext* pFilePoolContext = new iotas_acd::CFilePoolImpl();
    pFilePoolContext->init("file_pool", nullptr, spdlog::default_logger());

    // 下载文件
    std::string uri = "http://192.168.9.247:9081/3,54f963fd205b.jpg";
    std::string filePath;
    StateVale   stateValue = filePoolContext.getFileBlock(uri, -1, filePath);
    if (stateValue == StateVale::NONE) {
        spdlog::debug("获取文件失败");
    }
    else if (stateValue == StateVale::READY) {
        spdlog::debug("文件读取成功:{}", filePath);
    }
	// 之后再通过uri获取文件都是，已经下载好的，不会重新下载
    stateValue = filePoolContext.getFileBlock(uri, -1, filePath);
    if (stateValue == StateVale::NONE) {
        spdlog::debug("获取文件失败");
    }
    else if (stateValue == StateVale::READY) {
        spdlog::debug("文件读取成功:{}", filePath);
    }
    delete pFilePoolContext;
    return 0
}
```



## 如何嵌入到你自己的项目中

## 如何嵌入到你自己的项目中

- ### 源码方式

  把file_pool下代码文件拷贝到你自己的项目中，使用C++11编译器

- ### **静态编译**

  使用CMake生成项目，然后编译，需要C++11编译器

## 实现原理

CFilePoolContext 中保存下载链接的文件状态，通过文件动作，转变其状态。主要设计模式：状态模式。

### 文件状态

- **NONE**

  初始状态

- **DOWNLOADING** 

  文件下载中

- **CHECKING**

  文件校验中

- **READY**

  文件就绪

### 动作

- 下载动作

  状态由**NONE**切换到**DOWNLOADING**

- 校验动作

  状态由**DOWNLOADING** 切换到**CHECKING**，校验成功转为**READY**，校验失败转为**NONE**



## 项目依赖

- 日志使用spdlog https://github.com/gabime/spdlog.git

 