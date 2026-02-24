#include "AuthManager.hpp"

bool AuthManager::validateToken(const QString& inputToken) {
    LOG_FUNC_ENTRY();
    
    bool success = (inputToken == AuthConfig::SYSTEM_TOKEN);
    
    if (success) {
        LOG_INFO("AuthManager: Token validado com sucesso.");
    } else {
        LOG_WARN("AuthManager: Falha na tentativa de login - Token incorreto.");
    }

    LOG_FUNC_EXIT();
    return success;
}
