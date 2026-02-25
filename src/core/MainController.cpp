/**
 * @file MainController.cpp
 * @brief Implementação do orquestrador central do fluxo clínico.
 * @author Fabiano Souza
 * 
 * Este arquivo centraliza a lógica de navegação entre views e o despacho
 * de tarefas assíncronas para o Worker através da SafeQueue.
 */
 
#include "MainController.hpp"
#include "MainWindow.hpp"
#include "ui/LoginView.hpp"
#include "ui/PatientSelectionView.hpp"
#include "ui/MedicalRecordView.hpp"
#include "core/GlobalState.hpp"
#include "fslogging.hpp"
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include "core/Worker.hpp"

MainController::MainController(MainWindow* window, QObject* parent) 
    : QObject(parent), m_window(window) 
{
    LOG_FUNC_ENTRY();
    initializeDatabase();
    if (m_window) {
        setupConnections();
    }  
    LOG_FUNC_EXIT();	
}

MainController::~MainController() {}

/**
 * @brief Estabelece o mapeamento de sinais entre as Views e a lógica de negócio.
 * Utiliza Qt::UniqueConnection para garantir a integridade dos eventos de UI.
 */
void MainController::setupConnections() {
    LOG_FUNC_ENTRY();

    connect(m_window->getLoginView(), &LoginView::loginSuccessful, this, [this](bool isDoctor) {
        if (isDoctor) this->carregarListaPacientes(), Qt::UniqueConnection;
    });

    connect(m_window->getPatientSelectionView(), &PatientSelectionView::pacienteSelecionado, 
            this, &MainController::iniciarAtendimento, Qt::UniqueConnection);

    connect(m_window->getMedicalRecordView(), &MedicalRecordView::atendimentoFinalizado, 
            this, &MainController::finalizarAtendimento, Qt::UniqueConnection);
			
    connect(m_window->getMedicalRecordView(), &MedicalRecordView::importarExameSolicitado, 
            this, &MainController::handleImportarExame, Qt::UniqueConnection);
			
	connect(&GlobalState::instance(), &GlobalState::imageImported, this, [this](const QString& path) {
		LOG_INFO("Controller: Notificação de importação recebida: " + path.toStdString());
		
		this->carregarListaPacientes();
		
		LOG_INFO("Controller: Model atualizado. A View se auto-gerencia via Watcher/Sinal.");
	}, Qt::UniqueConnection); 
	
    LOG_INFO("Controller: Conexões de fluxo clínico estabelecidas.");
    LOG_FUNC_EXIT();
}

/**
 * @brief Interface com o sistema de arquivos para importação de exames (Requisito 2).
 * Captura o caminho via QFileDialog e delega a persistência ao Worker.
 */
void MainController::handleImportarExame(const QString& patientId) {
    LOG_FUNC_ENTRY();
    
    QString filePath = QFileDialog::getOpenFileName(nullptr, 
        "Selecionar Exame", "", "Imagens (*.png *.jpg *.bmp *.jpeg)");

    if (filePath.isEmpty()) return;
    LOG_INFO("Controller: Solicitando importacao ao Worker para " + filePath.toStdString());

    WorkerRequest req;
    req.type = WorkerRequest::IMPORT_EXAM;
    req.payload = QJsonObject{
        {"id", patientId},
        {"originPath", filePath}
    };
    GlobalState::instance().pushRequest(req);
    
    LOG_FUNC_EXIT();
}


void MainController::carregarListaPacientes() {
	LOG_FUNC_ENTRY();
    QJsonObject data = GlobalState::instance().getData();
    QJsonArray pacientes = data["pacientes"].toArray();
    m_window->getPatientSelectionView()->atualizarLista(pacientes);
	LOG_FUNC_EXIT();
}

/**
 * @brief Gerencia a transição para o prontuário do paciente.
 * Injeta os dados iniciais na MedicalRecordView e atualiza o status para "Em Atendimento".
 */
void MainController::iniciarAtendimento(const QJsonObject& paciente) {
	LOG_FUNC_ENTRY();
    QString id = paciente["id"].toString();
    LOG_INFO("Controller: Iniciando atendimento para " + id.toStdString());

    QJsonObject payload;
    payload["id"] = id;
    payload["status"] = "EM ATENDIMENTO";

    GlobalState::instance().pushRequest({WorkerRequest::UPDATE_STATUS, payload});
    m_window->getMedicalRecordView()->carregarDados(paciente);
	LOG_FUNC_EXIT();
}

/**
 * @brief Finaliza o ciclo de vida do atendimento atual.
 * Notifica o GlobalState para persistência em disco e retorna à fila de espera.
 */
void MainController::finalizarAtendimento() {
	LOG_FUNC_ENTRY();
    QString id = m_window->getMedicalRecordView()->getAtendimentoAtualId(); 
    QJsonObject payload;
    payload["id"] = id;
    payload["status"] = "FINALIZADO";

    GlobalState::instance().pushRequest({WorkerRequest::UPDATE_STATUS, payload});
    this->carregarListaPacientes();
	LOG_FUNC_EXIT();
}

void MainController::initializeDatabase() {
	LOG_FUNC_ENTRY();

    QString resPath = ":/resources/pacientes.json"; 
    if (!QFile::exists(resPath)) {
        resPath = ":/pacientes.json";
    }

    QFile resFile(resPath);
    if (resFile.open(QIODevice::ReadOnly)) {
        QByteArray data = resFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (!doc.isNull()) {
            GlobalState::instance().setData(doc.object());
            LOG_INFO("Controller: Dados carregados com sucesso do Resource: " + resPath.toStdString());
            
            this->carregarListaPacientes();
        } else {
            LOG_ERROR("Controller: Falha ao parsear JSON do Resource!");
        }
    } else {
        LOG_ERROR("ERRO CRITICO: Nao encontrou pacientes.json em :/resources ou :/");
    }
	LOG_FUNC_EXIT();
}


