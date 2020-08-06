
#include "state_interface.h"

namespace file_pool {
CStateInterface::CStateInterface(const CStateInterface& other) : m_context(nullptr) {
    m_uri      = other.m_uri;
    m_filePath = other.m_filePath;
}
CStateInterface::CStateInterface() : m_context(nullptr) {
    m_uri      = "";
    m_filePath = "";
}

void CStateInterface::setContext(CFilePoolContext* context) {
    m_context = context;
}
CFilePoolContext* CStateInterface::getContext() {
    return m_context;
}

std::string CStateInterface::getUri() {
    return m_uri;
}
void CStateInterface::setUri(std::string uri) {
    m_uri = uri;
}
std::string CStateInterface::getFilePath() {
    return m_filePath;
}
void CStateInterface::setFilePath(std::string filePath) {
    m_filePath = filePath;
}

StateVale CStateInterface::getStateValue() {
    return m_state;
}
void CStateInterface::setStateValue(StateVale stateValue) {
    m_state = stateValue;
}

int CStateInterface::changeFileState(CStateInterface* nextState) {
    if (m_context == nullptr) {
        return -1;
    }
    m_context->changeFileState(nextState->getUri(), nextState);
    return 0;
}
int CStateInterface::saveFileState() {
    if (m_context == nullptr) {
        return -1;
    }
    return m_context->saveFileState(this);
}

std::shared_ptr<spdlog::logger> CStateInterface::getLogger() {
    if (m_context == nullptr) {
        return nullptr;
    }
    return m_context->getLogger();
}
}  // namespace file_pool