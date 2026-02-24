/**
 * @file LoginView.hpp
 * @brief Tela de autenticação inicial do sistema.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef LOGINVIEW_HPP
#define LOGINVIEW_HPP

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include "fslogging.hpp"

/**
 * @class LoginView
 * @brief Gerencia o acesso ao sistema.
 */
class LoginView : public QWidget {
    Q_OBJECT

public:
    explicit LoginView(QWidget *parent = nullptr);
    void atualizarListaUsuarios(const QStringList& usuarios, bool isDoctor);
    void exibirMensagemErro(const QString& msg);

signals:
    void perfilSolicitado(bool isDoctor);
    void tentativaLogin(const QString& usuario, const QString& token);
    void loginSuccessful(bool isDoctor);

private:
    void setupUI();
    
    QComboBox* m_userCombo = nullptr;
    QLineEdit* m_tokenField = nullptr;
    QLabel*    m_errorLabel = nullptr;
    bool       m_isDoctorSelected;
};

#endif // LOGINVIEW_HPP
