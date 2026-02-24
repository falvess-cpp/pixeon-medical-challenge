/**
 * @file PatientSelectionView.hpp
 * @brief Interface de gerenciamento de lista de pacientes.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef PATIENTSELECTIONVIEW_HPP
#define PATIENTSELECTIONVIEW_HPP

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include "fslogging.hpp"

/**
 * @class PatientSelectionView
 * @brief Responsável pela listagem de pacientes aguardando atendimento.
 */
class PatientSelectionView : public QWidget {
    Q_OBJECT

public:
    explicit PatientSelectionView(QWidget *parent = nullptr);
    ~PatientSelectionView();

    void atualizarLista(const QJsonArray& pacientes);

signals:
    void pacienteSelecionado(const QJsonObject& dadosPaciente);
    void logoutRequested();

private slots:
    void onConfirmarSelecao();

private:
    void setupUI();

    QListWidget* m_listaWidget = nullptr;
    QPushButton* m_btnSelecionar = nullptr;
    QPushButton* m_btnLogout = nullptr;
    QList<QJsonObject> m_pacientesAtuais; 
};

#endif
