/**
 * @file PerfilModel.cpp
 * @brief Implementação do modelo de perfis de usuários.
 * @author Fabiano Souza
 * 
 * Este arquivo contém a lógica de filtragem de profissionais (Médicos/Recepcionistas)
 * e o gerenciamento do cache de usuários para acesso rápido durante o login.
 */
 
#include "core/PerfilModel.hpp"
#include "fslogging.hpp" 
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief Construtor: Inicializa o modelo carregando os dados do estado global.
 */
PerfilModel::PerfilModel() {
	LOG_FUNC_ENTRY();
    carregarDados();
	LOG_FUNC_EXIT();
}

/**
 * @brief Sincroniza o cache local com o GlobalState.
 * Utiliza o mecanismo de leitura segura (shared_lock) do GlobalState.
 */
bool PerfilModel::carregarDados() {
	LOG_FUNC_ENTRY();
    QFile file(":/perfis.json"); 
    
    if (!file.open(QIODevice::ReadOnly)) {
        LOG_ERROR("PerfilModel: Nao foi possivel abrir o arquivo JSON no recurso.");
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (doc.isNull() || !doc.isObject()) {
        LOG_WARN("PerfilModel: Formato JSON invalido.");
        return false;
    }

    QJsonObject obj = doc.object();

    m_medicos.clear();
    QJsonArray medArray = obj["medicos"].toArray();
    for (const QJsonValue &v : medArray) {
        m_medicos << v.toString();
    }

    m_recepcionistas.clear();
    QJsonArray recArray = obj["recepcionistas"].toArray();
    for (const QJsonValue &v : recArray) {
        m_recepcionistas << v.toString();
    }

    file.close();
    LOG_INFO("PerfilModel: Dados carregados com sucesso.");
    return true;
	LOG_FUNC_EXIT();
}

QStringList PerfilModel::getMedicos() const { return m_medicos; }
QStringList PerfilModel::getRecepcionistas() const { return m_recepcionistas; }
