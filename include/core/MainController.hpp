/**
 * @file MainController.hpp
 * @brief Orquestrador principal do fluxo de atendimento e gestão de prontuários.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <QObject>
#include <QJsonObject>
#include <QString>
#include "fslogging.hpp"

class MainWindow;

/**
 * @class MainController
 * @brief Controlador central da lógica de negócio pós-login (Padrão MVC).
 */
class MainController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construtor do controlador principal.
     * Estabelece as conexões de sinais e slots entre a MainWindow e as sub-views.
     * @param window Ponteiro para a janela principal (MainWindow).
     * @param parent Ponteiro opcional para o objeto pai.
     */
    explicit MainController(MainWindow* window, QObject* parent = nullptr);
    virtual ~MainController();
	
    /** 
     * @brief Configura o mapeamento de sinais de navegação do sistema.
     * Utiliza Qt::UniqueConnection para evitar duplicidade de disparos (Requisito de Corretude).
     */
    void setupConnections();
	
    /** 
     * @brief Carrega o estado inicial do banco de dados (database.json).
     * Garante a persistência entre sessões da aplicação.
     */	
    void initializeDatabase();	
	
private slots:

    /**
     * @brief Recarrega a lista de pacientes da fila de espera a partir do GlobalState.
     * Acionado após logins ou mudanças de status (Aguardando/Finalizado).
     */
    void carregarListaPacientes();
	
    /**
     * @brief Inicializa a carga de dados e prepara a interface para o atendimento.
     * @param paciente Objeto JSON contendo os dados do paciente selecionado.
     */	
    void iniciarAtendimento(const QJsonObject& paciente);
	
    /**
     * @brief Finaliza o ciclo de atendimento, atualiza o status no banco de dados e retorna à lista.
     * Envia um WorkerRequest do tipo UPDATE_STATUS para o GlobalState.
     */	
    void finalizarAtendimento();
	
    /**
     * @brief Processa a solicitação de importação de um novo exame para o paciente atual.
     * Abre o QFileDialog e despacha a tarefa de cópia para o Worker via SafeQueue.
     * @param patientId Identificador único do paciente (ex: CM004).
     */	
	void handleImportarExame(const QString& patientId);

private:
    MainWindow* m_window = nullptr;	
};

#endif // MAINCONTROLLER_HPP
