/**
 * @file main.cpp
 * @brief Ponto de entrada da aplicação Pixeon Medical System.
 * @author Fabiano Souza
 * @date 24/02/2026
 * 
 * Este arquivo coordena a inicialização do Qt, a subida da thread de 
 * processamento assíncrono (Worker) e o acoplamento entre Views e Controllers.
 */
 
#include <QApplication>
#include <QDebug>

#include "fslogging.hpp"
#include "ui/MainWindow.hpp"
#include "ui/LoginView.hpp"
#include "core/LoginController.hpp"
#include "core/MainController.hpp"
#include <thread>
#include "core/Worker.hpp"

/**
 * @brief Função principal (Entry Point).
 * 
 * Fluxo de inicialização:
 * 1. Configura a instância do QApplication.
 * 2. Inicia a Background Thread (Worker) para processamento não-bloqueante (Requisito 4).
 * 3. Instancia a MainWindow (View) e o MainController (Controller).
 * 4. Inicia o Loop de Eventos do Qt.
 * 
 * @param argc Quantidade de argumentos de linha de comando.
 * @param argv Vetor de argumentos de linha de comando.
 * @return int Status de saída da aplicação.
 */
int main(int argc, char *argv[]) {
    LOG_FUNC_ENTRY();
    LOG_INFO("Iniciando Pixeon Medical System (Fluxo Clínico)...");

    QApplication a(argc, argv);
	
	qRegisterMetaType<QString>("QString");
    
    MainWindow w;

    LoginView* loginView = w.getLoginView();
    if (loginView) {
        LOG_INFO("Inicializando LoginController...");
        auto* loginController = new LoginController(loginView, loginView);
        loginController->inicializar();
    } else {
        LOG_ERROR("ERRO CRÍTICO: LoginView não encontrada!");
        return -1;
    }

    LOG_INFO("Inicializando MainController para gestão de prontuários...");
    auto* mainController = new MainController(&w, &w);
    
    mainController->setupConnections();

    std::thread workerThread(backgroundWorker);
    workerThread.detach();
    w.show();
    
    LOG_INFO("Sistema pronto. Aguardando interação do usuário.");
    int result = a.exec();

    LOG_INFO("Encerrando aplicação Pixeon.");
    LOG_FUNC_EXIT();
    return result;
}
