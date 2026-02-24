/**
 * @file PatientSelectionView.cpp
 * @brief Implementação da interface de seleção de pacientes e gestão de fila.
 * @author Fabiano Souza
 * 
 * Este arquivo define a lógica da fila de espera, aplicando filtros de status
 * e organizando a interface em 3 frames para manter a unidade visual do sistema.
 */
 
#include "PatientSelectionView.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

PatientSelectionView::PatientSelectionView(QWidget *parent) : QWidget(parent) {
    LOG_FUNC_ENTRY();
    setupUI();
    LOG_FUNC_EXIT();
}

PatientSelectionView::~PatientSelectionView() {
    LOG_FUNC_ENTRY();
    LOG_FUNC_EXIT();
}

/**
 * @brief Configura a estrutura visual de 3 frames (Requisito 7 - Surpreenda-nos).
 * 
 * - Frame A: Título da tela centralizado.
 * - Frame B: QListWidget estilizado para exibição da fila.
 * - Frame C: Botões de ação com cores semânticas (Verde para Prosseguir, Vermelho para Sair).
 */
void PatientSelectionView::setupUI() {
    LOG_FUNC_ENTRY();
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QFrame* frameA = new QFrame(this);
    frameA->setFixedHeight(60);
    frameA->setStyleSheet("background-color: #2c3e50; color: white;");
    auto* layoutA = new QHBoxLayout(frameA);
    
    QLabel* title = new QLabel("<b>PACIENTES AGUARDANDO ATENDIMENTO</b>", this);
    title->setFont(QFont("Segoe UI", 14));
    layoutA->addWidget(title);
    layoutA->addStretch();
    mainLayout->addWidget(frameA);

    QFrame* frameB = new QFrame(this);
    auto* layoutB = new QVBoxLayout(frameB);
    layoutB->setContentsMargins(20, 20, 20, 20);

    m_listaWidget = new QListWidget(this);
    m_listaWidget->setFont(QFont("Segoe UI", 12));
    m_listaWidget->setStyleSheet(
        "QListWidget { border: 2px solid #bdc3c7; border-radius: 8px; padding: 10px; background: white; }"
        "QListWidget::item { padding: 15px; border-bottom: 1px solid #eee; }"
        "QListWidget::item:selected { background-color: #27ae60; color: white; border-radius: 5px; }"
    );
    layoutB->addWidget(m_listaWidget);
    mainLayout->addWidget(frameB, 1);

    QFrame* frameC = new QFrame(this);
    frameC->setFixedHeight(100);
    frameC->setStyleSheet("background-color: #f8f9fa; border-top: 1px solid #ccc;");
    auto* layoutC = new QHBoxLayout(frameC);
    layoutC->setContentsMargins(20, 0, 20, 0);

    m_btnSelecionar = new QPushButton(" ABRIR PRONTUÁRIO ", this);
    m_btnSelecionar->setMinimumHeight(60);
    m_btnSelecionar->setFixedWidth(300);
    m_btnSelecionar->setEnabled(false);
    m_btnSelecionar->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; border-radius: 5px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #2ecc71; }"
        "QPushButton:disabled { background-color: #bdc3c7; }"
    );

    m_btnLogout = new QPushButton(" SAIR / LOGOUT ", this);
    m_btnLogout->setMinimumHeight(50);
    m_btnLogout->setFixedWidth(150);
    m_btnLogout->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #c0392b; }"
    );

    layoutC->addWidget(m_btnSelecionar);
    layoutC->addStretch();
    layoutC->addWidget(m_btnLogout);
    mainLayout->addWidget(frameC);

    connect(m_listaWidget, &QListWidget::itemSelectionChanged, [this]() {
        m_btnSelecionar->setEnabled(m_listaWidget->currentRow() != -1);
    });

    connect(m_btnSelecionar, &QPushButton::clicked, this, &PatientSelectionView::onConfirmarSelecao);
    connect(m_btnLogout, &QPushButton::clicked, this, &PatientSelectionView::logoutRequested);
    
    LOG_INFO("UI da Seleção de Pacientes reestruturada em 3 Frames.");
    LOG_FUNC_EXIT();
}

/**
 * @brief Popula a lista de pacientes com base no estado global (Model).
 * 
 * Implementa a Regra de Negócio: Exibe apenas pacientes com status "AGUARDANDO".
 * Armazena os objetos originais em m_pacientesAtuais para garantir que o 
 * mapeamento do índice da lista corresponda ao JSON correto (Corretude).
 * 
 * @param pacientes Array JSON contendo a base completa de dados.
 */
void PatientSelectionView::atualizarLista(const QJsonArray& pacientes) {
    LOG_FUNC_ENTRY();
    m_listaWidget->clear();
    m_pacientesAtuais.clear();

    for (const QJsonValue& value : pacientes) {
        QJsonObject obj = value.toObject();
        
        if (obj["status"].toString() == "AGUARDANDO") {
            m_pacientesAtuais.append(obj);
            
            QString displayText = QString("[%1] %2 \nQueixa: %3")
                                    .arg(obj["id"].toString())
                                    .arg(obj["nome"].toString())
                                    .arg(obj["queixa"].toString());
            
            m_listaWidget->addItem(displayText);
        }
    }
    
    LOG_INFO("Lista de pacientes atualizada no display. Total aguardando: " + std::to_string(m_pacientesAtuais.size()));
    LOG_FUNC_EXIT();
}

/**
 * @brief Valida a seleção e inicia o fluxo de atendimento.
 * 
 * Recupera o objeto JSON do paciente através do índice selecionado e emite 
 * o sinal pacienteSelecionado para que o Controller realize a transição de tela.
 */
void PatientSelectionView::onConfirmarSelecao() {
    LOG_FUNC_ENTRY();
    int index = m_listaWidget->currentRow();
    if (index >= 0 && index < m_pacientesAtuais.size()) {
        QJsonObject selecionado = m_pacientesAtuais[index];
        LOG_INFO("Solicitando abertura de prontuário para: " + selecionado["id"].toString().toStdString());
        emit pacienteSelecionado(selecionado);
    }
    LOG_FUNC_EXIT();
}
