/**
 * @file AuthManager.hpp
 * @brief Autenticação e validação de permissões de acesso.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef AUTHMANAGER_HPP
#define AUTHMANAGER_HPP

#include <QString>
#include "fslogging.hpp"

namespace AuthConfig {
    inline constexpr const char* SYSTEM_TOKEN = "123456"; 
}

/**
 * @class AuthManager
 * @brief Responsável por validar as credenciais de acesso ao sistema.
 */
class AuthManager {
public:
    static bool validateToken(const QString& inputToken);
};

#endif
