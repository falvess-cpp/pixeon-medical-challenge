/**
 * @file ReceptionView.hpp
 * @brief Responsavel pela triagem e cadastramento de pacientes.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef RECEPTIONVIEW_HPP
#define RECEPTIONVIEW_HPP

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout> 
#include <QHBoxLayout>
#include "core/GlobalState.hpp"
#include "fslogging.hpp"

/**
 * @class ReceptionView
 * @brief Responsável pela entrada de pacientes no fluxo clínico.
 */
class ReceptionView : public QWidget {
    Q_OBJECT
public:
    explicit ReceptionView(QWidget *parent = nullptr); 

signals:
    void logoutRequested(); 

private slots:
    void onAddPatient();

private:
    void setupUI();

    QLineEdit* m_nameField = nullptr;
    QListWidget* m_listaRecepcao = nullptr;
    QPushButton* m_btnNovoPaciente = nullptr;
    QPushButton* m_btnLogout = nullptr;
};

#endif
