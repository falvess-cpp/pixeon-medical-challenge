/**
 * @file GlobalState.hpp
 * @brief Singleton para gerenciamento de estado global e sincronização de Threads.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef GLOBALSTATE_HPP
#define GLOBALSTATE_HPP

#include <QObject>
#include <QImage>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>
#include <shared_mutex>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "fslogging.hpp"

namespace SystemConfig {
    inline constexpr const char* SYSTEM_TOKEN = "102030";
}

/**
 * @struct WorkerRequest
 * @brief Estrutura unificada para requisições de background (Padrão Command).
 */
struct WorkerRequest {
    enum Type { 
        SAVE_PATIENT, 
        UPDATE_EXAM, 
        SYNC_DISK, 
        PROCESS_IMAGE,
		UPDATE_STATUS,
		IMPORT_EXAM
    };
    
    Type type;
    QJsonObject payload;
    QImage image;
};

/**
 * @class GlobalState
 * @brief Orquestrador de Estado e Fila de Trabalho Assíncrona.
 */
class GlobalState : public QObject {
    Q_OBJECT

public:
	/** @brief Acesso à instância única do Singleton (Thread-Safe). */
    static GlobalState& instance() {
        static GlobalState inst;
        return inst;
    }

    /**
     * @brief Helper estático para resolver caminhos de exames de forma portátil.
     * @param patientId ID opcional para retornar a subpasta do paciente.
     * @return QString Caminho nativo absoluto baseado na raiz da aplicação.
     */
    static QString getExamsPath(const QString& patientId = "") {
        QString baseDir = QCoreApplication::applicationDirPath();
        QString path = baseDir + "/data/exames";
        
        if (!patientId.isEmpty()) {
            path += "/" + patientId;
        }

        QDir().mkpath(path);
        
        return QDir::toNativeSeparators(path);
    }
	
    /**
     * @brief Adiciona uma nova tarefa à SafeQueue.
     * Notifica o Worker que há trabalho pendente (Requisito de Eficiência).
     * @param req Objeto contendo o comando e os dados.
     */	
    void pushRequest(WorkerRequest req) {
        std::lock_guard<std::mutex> lock(m_queueMtx);
        m_queue.push(std::move(req));
        m_cv.notify_one();
    }

    /**
     * @brief Recupera uma tarefa da fila. Bloqueia a thread se a fila estiver vazia.
     * @return WorkerRequest O próximo comando a ser executado pelo Worker.
     */
    WorkerRequest popRequest() {
        std::unique_lock<std::mutex> lock(m_queueMtx);
        m_cv.wait(lock, [this]{ return !m_queue.empty(); });
        auto req = std::move(m_queue.front());
        m_queue.pop();
        return req;
    }

    /**
     * @brief Obtém uma cópia segura do Banco de Dados em memória (Thread-Safe).
     * Utiliza shared_lock para permitir múltiplas leituras concorrentes.
     * @return QJsonObject O estado atual de todos os pacientes.
     */
    QJsonObject getData() {
        std::shared_lock lock(m_dataMtx);
        return m_root;
    }

    /**
     * @brief Atualiza o estado global dos dados.
     * Requer lock exclusivo (unique_lock) para garantir atomicidade.
     * @param obj Novo objeto JSON de estado.
     */
    void setData(const QJsonObject& obj) {
        std::unique_lock lock(m_dataMtx);
        m_root = obj;
    }

signals:
	/** @brief Notifica a UI que uma imagem foi processada */
    void imageProcessed(const QImage& img);
	
	/** @brief Notifica a UI que um novo arquivo físico foi importado. */
	void imageImported(QString path);

private:
    explicit GlobalState(QObject* parent = nullptr) : QObject(parent) {}
    
    QJsonObject m_root;
    std::shared_mutex m_dataMtx;
    
    std::queue<WorkerRequest> m_queue;
    std::mutex m_queueMtx;
    std::condition_variable m_cv;

    GlobalState(const GlobalState&) = delete;
    GlobalState& operator=(const GlobalState&) = delete;
};

#endif // GLOBALSTATE_HPP
