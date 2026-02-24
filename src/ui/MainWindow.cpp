/**
 * @file MainWindow.cpp
 * @brief Implementação da janela principal e coordenação do QStackedWidget.
 * @author Fabiano Souza
 * 
 * Este arquivo define a moldura global da aplicação (Header, Body, Footer)
 * e gerencia a alternância entre as telas de Login, Seleção de Pacientes,
 * Prontuário e Recepção.
 */
 
#include "MainWindow.hpp"
#include "ui/PatientSelectionView.hpp"
#include "ui/MedicalRecordView.hpp"
#include "LoginView.hpp"
#include "DoctorView.hpp"
#include "ReceptionView.hpp"
#include "fslogging.hpp"

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QWidget>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    LOG_FUNC_ENTRY();
    setupUI();
    this->resize(1280, 720);
    this->setMinimumSize(1024, 768);
    LOG_FUNC_EXIT();
}

MainWindow::~MainWindow() {
    LOG_FUNC_ENTRY();
	if (m_doctorViewWindow) {
		m_doctorViewWindow->close(); 
		m_doctorViewWindow = nullptr; 
	}

    LOG_INFO("Closing MainWindow");
    LOG_FUNC_EXIT();
}

/**
 * @brief Constrói a arquitetura global de 3 frames (A, B e C).
 * 
 * O Frame B atua como o container dinâmico (m_stackedWidget), permitindo
 * que o Header (Frame A) e o Footer (Frame C) permaneçam persistentes
 * durante toda a sessão do usuário, garantindo unidade visual.
 */
void MainWindow::setupUI() {
    LOG_FUNC_ENTRY();
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QFrame* frameA = new QFrame(this);
    frameA->setFixedHeight(50);
    frameA->setStyleSheet("background-color: #1a252f; color: white; border-bottom: 2px solid #2c3e50;");
    QHBoxLayout* layoutA = new QHBoxLayout(frameA);
    
    QLabel* logo = new QLabel("<b>PIXEON</b> | Medical System", this);
    logo->setStyleSheet("font-size: 16px; color: #3498db;");
    
    m_lblStatusUsuario = new QLabel("Aguardando Login...", this);
    m_lblStatusUsuario->setStyleSheet("color: #ecf0f1; font-size: 12px;");

    layoutA->addWidget(logo);
    layoutA->addStretch();
    layoutA->addWidget(m_lblStatusUsuario);
    mainLayout->addWidget(frameA);
    m_stackedWidget = new QStackedWidget(this);
    
    m_loginView           = new LoginView(this);
    m_patientSelectionView = new PatientSelectionView(this);
    m_medicalRecordView    = new MedicalRecordView(this);
    m_receptionView       = new ReceptionView(this);

    m_stackedWidget->addWidget(m_loginView); 
    m_stackedWidget->addWidget(m_patientSelectionView);
    m_stackedWidget->addWidget(m_medicalRecordView);
    m_stackedWidget->addWidget(m_receptionView);
    
    mainLayout->addWidget(m_stackedWidget, 1);

    QFrame* frameC = new QFrame(this);
    frameC->setFixedHeight(30);
    frameC->setStyleSheet("background-color: #f8f9fa; border-top: 1px solid #dee2e6; color: #7f8c8d;");
    QHBoxLayout* layoutC = new QHBoxLayout(frameC);
    layoutC->setContentsMargins(15, 0, 15, 0);

    QLabel* version = new QLabel("Versão 2026.1.0-Release", this);
    QLabel* dateLabel = new QLabel("Data: 24/02/2026", this);

    layoutC->addWidget(version);
    layoutC->addStretch();
    layoutC->addWidget(dateLabel);
    mainLayout->addWidget(frameC);

    connect(m_loginView, &LoginView::loginSuccessful, this, &MainWindow::handleLogin);
    connect(m_patientSelectionView, &PatientSelectionView::pacienteSelecionado, this, &MainWindow::handlePatientSelected);
    connect(m_medicalRecordView, &MedicalRecordView::abrirExameSolicitado, this, &MainWindow::handleOpenExam);
    connect(m_medicalRecordView, &MedicalRecordView::atendimentoFinalizado, this, &MainWindow::handleFinishAttendance);
    connect(m_patientSelectionView, &PatientSelectionView::logoutRequested, this, &MainWindow::handleLogout);
    connect(m_receptionView, &ReceptionView::logoutRequested, this, &MainWindow::handleLogout);

    LOG_INFO("MainWindow reestruturada com moldura global de 3 frames.");
	LOG_FUNC_EXIT();
}


void MainWindow::handleLogin(bool isDoctor) {
    LOG_FUNC_ENTRY();
    if (isDoctor) {
        m_lblStatusUsuario->setText("<b>Médico:</b> Conectado");
        m_stackedWidget->setCurrentWidget(m_patientSelectionView);
    } else {
        m_lblStatusUsuario->setText("<b>Recepção:</b> Conectado");
        m_stackedWidget->setCurrentWidget(m_receptionView);
    }
	LOG_FUNC_EXIT();
}

/**
 * @brief Realiza o encerramento da sessão e limpeza de estado.
 * 
 * Garante que ao deslogar, o sistema retorne à tela inicial e oculte
 * informações sensíveis do paciente anterior (Segurança).
 */
void MainWindow::handleLogout() {
    LOG_FUNC_ENTRY();
    LOG_INFO("User logged out. Returning to Login Screen.");
	m_stackedWidget->setCurrentWidget(m_loginView);
    LOG_FUNC_EXIT();
}

/**
 * @brief Executa a transição de fluxo após a escolha do paciente.
 * 
 * Aciona o carregamento de dados na MedicalRecordView e altera o índice
 * do m_stackedWidget para exibir o prontuário selecionado.
 */
void MainWindow::handlePatientSelected(const QJsonObject& patientData) {
	LOG_FUNC_ENTRY();
    LOG_INFO("Paciente selecionado: " + patientData["nome"].toString().toStdString());
    
    m_medicalRecordView->carregarDados(patientData);
    m_stackedWidget->setCurrentWidget(m_medicalRecordView);
	LOG_FUNC_EXIT();
}

/**
 * @brief Gerencia a abertura do Visualizador de Diagnóstico (Requisito Bônus 6).
 * 
 * Cria uma instância independente da DoctorView, permitindo que o médico
 * utilize um segundo monitor ou visualize o exame em tela cheia sem
 * perder o contexto do prontuário na janela principal.
 */
void MainWindow::handleOpenExam(const QString& imagePath) {
	LOG_FUNC_ENTRY();
    if (m_doctorViewWindow) m_doctorViewWindow->close();

    m_doctorViewWindow = new DoctorView();
    m_doctorViewWindow->setWindowTitle("Visualizador de Diagnóstico Pixeon");
    m_doctorViewWindow->resize(1200, 850); 
    m_doctorViewWindow->show();    
    m_doctorViewWindow->loadImage(imagePath);
	LOG_FUNC_EXIT();
}

void MainWindow::handleFinishAttendance() {
	LOG_FUNC_ENTRY();
    LOG_INFO("Atendimento finalizado. Retornando à lista.");
    m_stackedWidget->setCurrentWidget(m_patientSelectionView);
	LOG_FUNC_EXIT();
}
