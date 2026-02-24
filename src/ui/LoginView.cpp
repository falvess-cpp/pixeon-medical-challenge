/**
 * @file LoginView.cpp
 * @brief Implementação da interface de autenticação do sistema médico.
 * @author Fabiano Souza
 * 
 * Este arquivo define a estética inicial da aplicação, utilizando o layout 
 * de 3 frames para garantir unidade visual com o restante do ecossistema.
 */
 
#include "ui/LoginView.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

LoginView::LoginView(QWidget *parent) : QWidget(parent), m_isDoctorSelected(true) {
    LOG_FUNC_ENTRY();
	this->setObjectName("loginView");
    setupUI();
    LOG_FUNC_EXIT();
}

/**
 * @brief Constrói a hierarquia visual da tela de login.
 * 
 * Implementa a arquitetura de frames:
 * - Frame A (Topo): Branding e título da clínica.
 * - Frame B (Meio): Seleção de perfil e entrada de Token (Segurança).
 * - Frame C (Rodapé): Botão de acesso em destaque (Verde).
 */
void LoginView::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QFont defaultFont("Segoe UI", 12, QFont::Bold);
    QFont labelFont("Segoe UI", 10, QFont::Normal);

    QFrame* frameA = new QFrame(this);
    frameA->setFixedHeight(80);
    frameA->setStyleSheet("background-color: #2c3e50; color: white;");
    auto* layoutA = new QHBoxLayout(frameA);
    QLabel* title = new QLabel("<b>CLÍNICA TOP MEDICAL</b> | Sistema de Gestão", this);
    title->setFont(QFont("Segoe UI", 16));
    layoutA->addWidget(title, 0, Qt::AlignCenter);
    mainLayout->addWidget(frameA);

    QFrame* frameB = new QFrame(this);
    auto* layoutB = new QVBoxLayout(frameB);
    layoutB->setContentsMargins(50, 20, 50, 20);
    layoutB->setSpacing(15);

    auto* profileLayout = new QHBoxLayout();
    auto* btnMed = new QPushButton(" MÉDICO ", this);
    auto* btnRec = new QPushButton(" RECEPCIONISTA ", this);
    QString styleBtn = "background-color: #ecf0f1; border: 2px solid #bdc3c7; border-radius: 8px; font-weight: bold; min-height: 80px;";
    btnMed->setStyleSheet(styleBtn);
    btnRec->setStyleSheet(styleBtn);
    profileLayout->addWidget(btnMed);
    profileLayout->addWidget(btnRec);
    layoutB->addLayout(profileLayout);

    layoutB->addWidget(new QLabel("SELECIONE O USUÁRIO:", this), 0, Qt::AlignCenter);
    m_userCombo = new QComboBox(this);
    m_userCombo->setMinimumHeight(45);
    m_userCombo->setFixedWidth(350);
    m_userCombo->setFont(defaultFont);
    layoutB->addWidget(m_userCombo, 0, Qt::AlignCenter);

    layoutB->addWidget(new QLabel("TOKEN DE ACESSO:", this), 0, Qt::AlignCenter);
    m_tokenField = new QLineEdit(this);
    m_tokenField->setPlaceholderText("******");
    m_tokenField->setEchoMode(QLineEdit::Password);
    m_tokenField->setMaxLength(6);
    m_tokenField->setFixedWidth(200);
    m_tokenField->setMinimumHeight(45);
    m_tokenField->setFont(defaultFont);
    m_tokenField->setAlignment(Qt::AlignCenter);
    layoutB->addWidget(m_tokenField, 0, Qt::AlignCenter);

    m_errorLabel = new QLabel("", this);
    m_errorLabel->setStyleSheet("color: red; font-weight: bold;");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    layoutB->addWidget(m_errorLabel);
    
    layoutB->addStretch();
    mainLayout->addWidget(frameB, 1);

    QFrame* frameC = new QFrame(this);
    frameC->setFixedHeight(100);
    frameC->setStyleSheet("background-color: #f8f9fa; border-top: 1px solid #ccc;");
    auto* layoutC = new QHBoxLayout(frameC);

    auto* btnLogin = new QPushButton("🔓 ACESSAR SISTEMA", this);
    btnLogin->setMinimumHeight(60);
    btnLogin->setFixedWidth(350);
    btnLogin->setFont(defaultFont);
    btnLogin->setStyleSheet("QPushButton { background-color: #27ae60; color: white; border-radius: 10px; }"
                            "QPushButton:hover { background-color: #2ecc71; }");

    layoutC->addWidget(btnLogin, 0, Qt::AlignCenter);
    mainLayout->addWidget(frameC);

    connect(btnMed, &QPushButton::clicked, [this](){ emit perfilSolicitado(true); });
    connect(btnRec, &QPushButton::clicked, [this](){ emit perfilSolicitado(false); });
    
    connect(btnLogin, &QPushButton::clicked, [this](){
        m_errorLabel->clear();
        emit tentativaLogin(m_userCombo->currentText(), m_tokenField->text());
        m_tokenField->clear();
        m_tokenField->setFocus();
    });

    connect(m_tokenField, &QLineEdit::textChanged, [this](){ m_errorLabel->clear(); });
}


void LoginView::atualizarListaUsuarios(const QStringList& usuarios, bool isDoctor) {
    m_isDoctorSelected = isDoctor;
    m_userCombo->clear();
    m_userCombo->addItems(usuarios);
    m_tokenField->clear();
}

void LoginView::exibirMensagemErro(const QString& msg) {
    m_errorLabel->setText(msg);
    m_tokenField->setFocus();
    LOG_WARN("UI: Erro exibido - " + msg.toStdString());
}
