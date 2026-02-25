/**
 * @file MedicalRecordView.cpp
 * @brief Implementação da interface de prontuário e galeria de exames.
 * @author Fabiano Souza
 * 
 * Este arquivo define a lógica de visualização dos dados do paciente e o 
 * monitoramento dinâmico da pasta de exames, garantindo que novos arquivos 
 * importados apareçam instantaneamente para o médico.
 */
 
#include "MedicalRecordView.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFont>
#include <QFile>
#include <QJsonArray>
#include <QTimer>
#include <QDir>
#include <QScrollArea>
#include <QFrame>
#include "core/GlobalState.hpp"

MedicalRecordView::MedicalRecordView(QWidget *parent) : QWidget(parent) {
    LOG_FUNC_ENTRY();
    setupUI();
    LOG_FUNC_EXIT();
}

MedicalRecordView::~MedicalRecordView() {
    LOG_FUNC_ENTRY();
    LOG_FUNC_EXIT();
}

/**
 * @brief Constrói a interface dividida em 3 Frames funcionais.
 * 
 * Estrutura aplicada:
 * - Frame A: Cabeçalho persistente com dados vitais (Nome/ID).
 * - Frame B: Área de prontuário com QScrollArea para textos longos (Anamnese).
 * - Frame C: Lista de exames (QListWidget) e botões de ação com cores semânticas.
 */
void MedicalRecordView::setupUI() {
	LOG_FUNC_ENTRY();
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_folderWatcher = new QFileSystemWatcher(this);

    QFrame* frameA = new QFrame(this);
    frameA->setFixedHeight(60);
    frameA->setStyleSheet("background-color: #2c3e50; color: white;");
    auto* layoutA = new QHBoxLayout(frameA);
    lblNome = new QLabel("NOME DO PACIENTE", this);
    lblID = new QLabel("ID: --", this);
    layoutA->addWidget(lblNome);
    layoutA->addStretch();
    layoutA->addWidget(lblID);
    mainLayout->addWidget(frameA);

    QFrame* frameB = new QFrame(this);
    auto* layoutB = new QVBoxLayout(frameB);
    auto* scroll = new QScrollArea(this);
    auto* containerB = new QWidget();
    auto* innerLayoutB = new QVBoxLayout(containerB);

    lblIdade = new QLabel(this);
    lblQueixa = new QLabel(this);
    lblHistorico = new QLabel(this);
    lblHistorico->setWordWrap(true);
    lblExameSugerido = new QLabel(this);

    innerLayoutB->addWidget(new QLabel("<b>DADOS CLÍNICOS:</b>"));
    innerLayoutB->addWidget(lblIdade);
    innerLayoutB->addWidget(lblQueixa);
    innerLayoutB->addSpacing(10);
    innerLayoutB->addWidget(new QLabel("<b>HISTÓRICO E EVOLUÇÃO:</b>"));
    innerLayoutB->addWidget(lblHistorico);
    innerLayoutB->addSpacing(10);
    innerLayoutB->addWidget(new QLabel("<b>SUGESTÃO DO SISTEMA:</b>"));
    innerLayoutB->addWidget(lblExameSugerido);
    innerLayoutB->addStretch();

    scroll->setWidget(containerB);
    scroll->setWidgetResizable(true);
    layoutB->addWidget(scroll);
    mainLayout->addWidget(frameB, 1);

    QFrame* frameC = new QFrame(this);
    frameC->setFixedHeight(180);
    frameC->setStyleSheet("background-color: #f8f9fa; border-top: 1px solid #ccc;");
    auto* layoutC = new QHBoxLayout(frameC);

    auto* layoutC1 = new QVBoxLayout();
    layoutC1->addWidget(new QLabel("<b>ARQUIVOS NA PASTA DE EXAMES:</b>"));
    m_listaExamesWidget = new QListWidget(this);
    layoutC1->addWidget(m_listaExamesWidget);

    auto* layoutC2 = new QVBoxLayout();
    auto* btnImportar = new QPushButton(" IMPORTAR ", this);
    m_btnVisualizar = new QPushButton(" VISUALIZAR ", this);
    auto* btnFinalizar = new QPushButton(" CONCLUIR ATENDIMENTO ", this);

    QString styleVerde = "QPushButton { background-color: #27ae60; color: white; font-weight: bold; border-radius: 5px; min-height: 40px; }"
                         "QPushButton:hover { background-color: #2ecc71; }"
                         "QPushButton:disabled { background-color: #bdc3c7; color: #7f8c8d; }";
    
    QString styleVermelho = "QPushButton { background-color: #c0392b; color: white; font-weight: bold; border-radius: 5px; min-height: 40px; }"
                            "QPushButton:hover { background-color: #e74c3c; }";

    btnImportar->setStyleSheet(styleVerde);
    m_btnVisualizar->setStyleSheet(styleVerde);
    btnFinalizar->setStyleSheet(styleVermelho);

    m_btnVisualizar->setEnabled(false);

    layoutC2->addWidget(btnImportar);
    layoutC2->addWidget(m_btnVisualizar);
    layoutC2->addWidget(btnFinalizar);

    layoutC->addLayout(layoutC1, 1);
    layoutC->addLayout(layoutC2);
    mainLayout->addWidget(frameC);

    connect(m_listaExamesWidget, &QListWidget::itemSelectionChanged, [this]() {
        m_btnVisualizar->setEnabled(m_listaExamesWidget->currentRow() != -1);
    });

    connect(m_btnVisualizar, &QPushButton::clicked, this, &MedicalRecordView::onVerExameClicked);
    
    connect(btnImportar, &QPushButton::clicked, [this]() { 
        static bool processando = false;
        if(processando) return;
        processando = true;

        emit importarExameSolicitado(m_dadosPaciente["id"].toString()); 
        QTimer::singleShot(500, [this](){ processando = false; }); 
    });

    connect(btnFinalizar, &QPushButton::clicked, this, &MedicalRecordView::onFinalizarClicked);

    connect(&GlobalState::instance(), &GlobalState::imageImported, this, [this](QString path){
        this->atualizarListaArquivos(); 
    });

    connect(m_folderWatcher, &QFileSystemWatcher::directoryChanged, 
            this, &MedicalRecordView::atualizarListaArquivos);
	LOG_FUNC_EXIT();
}

/**
 * @brief Sincroniza a View com os dados do paciente selecionado.
 * 
 * Além de preencher os labels, configura o QFileSystemWatcher para monitorar 
 * a pasta específica do paciente (Requisito de Portabilidade de Caminhos).
 */
void MedicalRecordView::carregarDados(const QJsonObject& dados) {
	LOG_FUNC_ENTRY();
    m_dadosPaciente = dados;
    QString id = dados["id"].toString();
    
    lblNome->setText(dados["nome"].toString());
    lblID->setText("ID: " + id);
    lblIdade->setText("Idade: " + QString::number(dados["idade"].toInt()) + " anos");
    lblQueixa->setText("Queixa: " + dados["queixa"].toString());
    lblHistorico->setText(dados["historico"].toString());
    lblExameSugerido->setText(dados["exame_sugerido"].toString());

	QString baseDir = QCoreApplication::applicationDirPath();
    QString pathPasta = baseDir + "/data/exames/" + id;
	pathPasta = QDir::toNativeSeparators(pathPasta);
	LOG_INFO("MedicalRecordView: Monitorando pasta do paciente: " + pathPasta.toStdString());
    
    if (!m_folderWatcher->directories().isEmpty()) {
        m_folderWatcher->removePaths(m_folderWatcher->directories());
    }
    
    QDir dir(pathPasta);
    if (!dir.exists()) dir.mkpath("."); 
    m_folderWatcher->addPath(pathPasta);
    
    atualizarListaArquivos();
	LOG_FUNC_EXIT();
}

/**
 * @brief Varre o diretório físico em busca de imagens (BMP, PNG, JPEG).
 * 
 * Popula o m_listaExamesWidget e armazena o caminho absoluto em cada item 
 * (Qt::UserRole) para permitir a abertura precisa na DoctorView.
 */
void MedicalRecordView::atualizarListaArquivos() {
	LOG_FUNC_ENTRY();
    m_listaExamesWidget->clear();
    m_btnVisualizar->setEnabled(false); 

    QString id = m_dadosPaciente["id"].toString();
    QString pathPasta = GlobalState::getExamsPath(id);
    QDir dir(pathPasta);
    
    QStringList filtros;
    filtros << "*.jpg" << "*.png" << "*.bmp" << "*.jpeg";
    
    QFileInfoList arquivos = dir.entryInfoList(filtros, QDir::Files, QDir::Time); 

    for (const QFileInfo& info : arquivos) {
        QListWidgetItem* item = new QListWidgetItem(info.fileName(), m_listaExamesWidget);
        item->setData(Qt::UserRole, info.absoluteFilePath()); 
    }
	LOG_FUNC_EXIT();
}

/**
 * @brief Dispara a abertura do exame selecionado (Requisito 2).
 * 
 * Recupera o path do item selecionado e emite o sinal abrirExameSolicitado. 
 * O botão de visualização é habilitado/desabilitado via sinais de seleção da lista.
 */
void MedicalRecordView::onVerExameClicked() {
	LOG_FUNC_ENTRY();
    QListWidgetItem* item = m_listaExamesWidget->currentItem();
    if (!item) return;

    QString path = item->data(Qt::UserRole).toString();
    if (QFile::exists(path)) {
        emit abrirExameSolicitado(path); 
    } else {
        atualizarListaArquivos(); 
    }
	LOG_FUNC_EXIT();
}

/**
 * @brief Finaliza o atendimento atual.
 * Notifica o sistema para que o status do paciente seja atualizado para 'FINALIZADO'.
 */
void MedicalRecordView::onFinalizarClicked() {
	LOG_FUNC_ENTRY();
    LOG_INFO("Atendimento concluído.");
    emit atendimentoFinalizado();
	LOG_FUNC_EXIT();
}
