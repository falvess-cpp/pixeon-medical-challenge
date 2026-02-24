/**
 * @file ReceptionView.cpp
 * @brief Implementação da interface de recepção e triagem de pacientes.
 * @author Fabiano Souza
 * 
 * Este arquivo define a lógica de entrada de novos registros no sistema,
 * utilizando o padrão de 3 frames para manter a unidade visual com as telas médicas.
 */
 
#include "ui/ReceptionView.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QDateTime>
#include <QJsonObject>

ReceptionView::ReceptionView(QWidget *parent) : QWidget(parent) {
    LOG_FUNC_ENTRY();
    setupUI();
    LOG_FUNC_EXIT();
}

/**
 * @brief Constrói a hierarquia visual da Recepção.
 * 
 * Estrutura aplicada:
 * - Frame A (Topo): Título do Painel de Recepção.
 * - Frame B (Meio): Formulário de entrada (QLineEdit) e histórico local (QListWidget).
 * - Frame C (Rodapé): Botão de Agendamento (Verde) e Logout (Vermelho) à direita.
 */
void ReceptionView::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QFrame* frameA = new QFrame(this);
    frameA->setFixedHeight(60);
    frameA->setStyleSheet("background-color: #2c3e50; color: white;");
    auto* layoutA = new QHBoxLayout(frameA);
    
    QLabel* title = new QLabel("<b>PAINEL DE RECEPÇÃO E TRIAGEM</b>", this);
    title->setFont(QFont("Segoe UI", 14));
    layoutA->addWidget(title);
    layoutA->addStretch();
    mainLayout->addWidget(frameA);

    QFrame* frameB = new QFrame(this);
    auto* layoutB = new QVBoxLayout(frameB);
    layoutB->setContentsMargins(40, 40, 40, 40);
    layoutB->setSpacing(15);

    layoutB->addWidget(new QLabel("<b>NOVO AGENDAMENTO:</b>", this));
    
    m_nameField = new QLineEdit(this);
    m_nameField->setPlaceholderText("Digite o nome completo do paciente...");
    m_nameField->setMinimumHeight(40);
    m_nameField->setFont(QFont("Segoe UI", 11));
    m_nameField->setStyleSheet("border: 2px solid #bdc3c7; border-radius: 5px; padding-left: 10px;");
    
    layoutB->addWidget(m_nameField);
    
    m_listaRecepcao = new QListWidget(this);
    m_listaRecepcao->setStyleSheet("border: 1px solid #eee; background: #fdfdfd;");
    layoutB->addWidget(new QLabel("Fila de Espera Recente:"));
    layoutB->addWidget(m_listaRecepcao, 1);

    mainLayout->addWidget(frameB, 1);

    QFrame* frameC = new QFrame(this);
    frameC->setFixedHeight(100);
    frameC->setStyleSheet("background-color: #f8f9fa; border-top: 1px solid #ccc;");
    auto* layoutC = new QHBoxLayout(frameC);
    layoutC->setContentsMargins(20, 0, 20, 0);

    m_btnNovoPaciente = new QPushButton(" AGENDAR CONSULTA ", this);
    m_btnNovoPaciente->setMinimumHeight(55);
    m_btnNovoPaciente->setFixedWidth(250);
    m_btnNovoPaciente->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; font-weight: bold; border-radius: 5px; }"
        "QPushButton:hover { background-color: #2ecc71; }"
    );

    m_btnLogout = new QPushButton(" SAIR / LOGOUT ", this);
    m_btnLogout->setMinimumHeight(45);
    m_btnLogout->setFixedWidth(150);
    m_btnLogout->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; font-weight: bold; border-radius: 5px; }"
        "QPushButton:hover { background-color: #c0392b; }"
    );

    layoutC->addWidget(m_btnNovoPaciente);
    layoutC->addStretch();
    layoutC->addWidget(m_btnLogout);
    mainLayout->addWidget(frameC);

    connect(m_btnNovoPaciente, &QPushButton::clicked, this, &ReceptionView::onAddPatient);
    connect(m_btnLogout, &QPushButton::clicked, this, &ReceptionView::logoutRequested);
}

/**
 * @brief Processa o agendamento de um novo paciente.
 * 
 * Valida a entrada, cria um objeto JSON com metadados (timestamp/status) e 
 * despacha um WorkerRequest::SAVE_PATIENT para a SafeQueue.
 * Garante que a UI não bloqueie durante a persistência em disco (Eficiência).
 */
void ReceptionView::onAddPatient() {
    if (m_nameField->text().isEmpty()) return;

    LOG_INFO("Recepcionista: Gerando request para " + m_nameField->text().toStdString());

    QJsonObject patient;
    patient["name"] = m_nameField->text();
    patient["timestamp"] = QDateTime::currentDateTime().toString();
    patient["status"] = "AGUARDANDO";

    WorkerRequest req;
    req.type = WorkerRequest::SAVE_PATIENT;
    req.payload = patient;
    
    GlobalState::instance().pushRequest(req);

    m_listaRecepcao->insertItem(0, "Aguardando: " + m_nameField->text());
    
    m_nameField->clear();
}
