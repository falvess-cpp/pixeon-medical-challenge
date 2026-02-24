/**
 * @file LoginController.hpp
 * @brief Controlador responsável pela orquestração do fluxo de autenticação.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef LOGINCONTROLLER_HPP
#define LOGINCONTROLLER_HPP

#include <QObject>
#include <memory>
#include "PerfilModel.hpp"
#include "fslogging.hpp"

class LoginView;

/**
 * @class LoginController
 * @brief Gerencia a lógica de negócio da tela de login (Padrão MVC).
 */
class LoginController : public QObject {
    Q_OBJECT

public:

    /**
     * @brief Construtor do controlador de login.
     * Realiza o mapeamento inicial de sinais entre a View e o Controller.
     * @param view Ponteiro para a interface de login.
     * @param parent Ponteiro opcional para o objeto pai.
     */
    explicit LoginController(LoginView* view, QObject* parent = nullptr);
    virtual ~LoginController() = default;

	/** @brief Inicializa a View com o perfil padrão (Médico). */
    void inicializar();

private slots:

    /**
     * @brief Slot acionado quando o usuário altera o perfil na interface.
     * Filtra e envia a lista de usuários correspondentes (Médicos ou Recepcionistas).
     * @param isDoctor Booleano indicando o perfil solicitado.
     */
    void aoMudarPerfil(bool isDoctor);
	
    /**
     * @brief Slot que executa a tentativa de autenticação.
     * Consulta o AuthManager e, em caso de falha, solicita que a View exiba o erro.
     * @param usuario Nome do usuário selecionado.
     * @param token Código de acesso inserido.
     */	
    void aoTentarLogin(const QString& usuario, const QString& token);

private:
    LoginView* m_view = nullptr;
    std::unique_ptr<PerfilModel> m_model;
};

#endif // LOGINCONTROLLER_HPP
