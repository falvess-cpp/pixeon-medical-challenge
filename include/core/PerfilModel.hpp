/**
 * @file PerfilModel.hpp
 * @brief Modelo de dados para gerenciamento de perfis de usuários do sistema.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef PERFILMODEL_HPP
#define PERFILMODEL_HPP

#include "fslogging.hpp"
#include <QStringList>
#include <QJsonObject>

/**
 * @class PerfilModel
 * @brief Responsável por gerenciar a lista de usuários e seus respectivos papéis.
 */
class PerfilModel {
public:

    /**
     * @brief Inicializa o modelo de perfis.
     * @param parent Ponteiro opcional para o objeto pai.
     */
    PerfilModel();
    
    /**
     * @brief Listagem de medicos.
     */
    QStringList getMedicos() const;
	
    /**
     * @brief Listagem de recepcionistas.
     */	
    QStringList getRecepcionistas() const;

private:
    QStringList m_medicos;
    QStringList m_recepcionistas;
    const QString m_caminhoArquivo = "perfis.json";
	
    /** 
     * @brief Carrega os dados iniciais de perfis a partir dos recursos do sistema.
     * Utiliza o GlobalState para acessar o JSON unificado de forma Thread-Safe.
     */
    bool carregarDados();	
};

#endif // PERFILMODEL_HPP
