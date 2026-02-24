/**
 * @file Worker.hpp
 * @brief Motor de processamento assíncrono e persistência de dados.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef WORKER_HPP
#define WORKER_HPP

#include <QString>

/**
 * @brief Função principal da Thread de Background (Worker Loop).
 * 
 * Executa um loop infinito (Consumer) que consome requisições da SafeQueue 
 * no GlobalState. Gerencia o ciclo de vida de processamento de imagem, 
 * importação de arquivos e sincronização com o disco (database.json).
 * 
 * @note Esta função deve ser iniciada em uma std::thread separada para 
 * cumprir o Requisito de Eficiência (4).
 */
void backgroundWorker();

/**
 * @brief Gerencia a importação física e o vínculo lógico de exames (Bônus 5).
 * 
 * Realiza a cópia do arquivo para o diretório portátil do sistema e 
 * atualiza o array de exames do paciente no modelo JSON.
 * 
 * @param id Identificador único do paciente (ex: CM005).
 * @param originPath Caminho da imagem selecionada via QFileDialog (Requisito 2).
 */
void processarImportacao(const QString& id, const QString& originPath);

#endif // WORKER_HPP
