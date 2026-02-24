/**
 * @file LoginController.cpp
 * @brief Implementação do controlador de autenticação (Mediador MVC).
 * @author Fabiano Souza
 * 
 * Este arquivo orquestra a comunicação entre a LoginView e o AuthManager,
 * garantindo que a interface permaneça desacoplada da lógica de segurança.
 */
 
#include "core/LoginController.hpp"
#include "ui/LoginView.hpp"
#include "core/AuthManager.hpp"
#include "core/PerfilModel.hpp"

LoginController::LoginController(LoginView* view, QObject* parent)
    : QObject(parent), m_view(view) 
{
    m_model = std::make_unique<PerfilModel>();

    connect(m_view, &LoginView::perfilSolicitado, this, &LoginController::aoMudarPerfil);
    connect(m_view, &LoginView::tentativaLogin, this, &LoginController::aoTentarLogin);
    
    LOG_INFO("LoginController: Instanciado e conectado.");
}

void LoginController::inicializar() {

    if (m_model->getMedicos().isEmpty() && m_model->getRecepcionistas().isEmpty()) {
        LOG_WARN("LoginController: Falha ao carregar perfis - Listas vazias.");
        m_view->exibirMensagemErro("ERRO: Falha ao carregar banco de perfis.");
        return;
    }

    LOG_INFO("LoginController: Inicializando View com perfil Medico.");
    aoMudarPerfil(true); 
}

void LoginController::aoTentarLogin(const QString& usuario, const QString& token) {
    bool autorizado = AuthManager::validateToken(token);

    if (autorizado) {
        LOG_INFO("AuthManager: Token validado com sucesso.");

        m_view->loginSuccessful(usuario.startsWith("DR")); 
    } else {
        LOG_WARN("AuthManager: Falha na tentativa de login - Token incorreto.");
        m_view->exibirMensagemErro("TOKEN INVÁLIDO OU PERFIL INCORRETO");
    }
}

void LoginController::aoMudarPerfil(bool isDoctor) {
    QStringList lista = isDoctor ? m_model->getMedicos() : m_model->getRecepcionistas();
    m_view->atualizarListaUsuarios(lista, isDoctor);
}
