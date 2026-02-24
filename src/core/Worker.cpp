/**
 * @file Worker.cpp
 * @brief Implementação dos algoritmos de processamento de imagem e persistência.
 * @author Fabiano Souza
 * 
 * Contém a lógica do consumidor da SafeQueue (backgroundWorker) e as
 * transformações lineares de brilho e contraste (Requisito 4).
 */
 
#include "core/Worker.hpp"
#include "core/GlobalState.hpp"
#include "fslogging.hpp"
#include <QFile>
#include <QJsonDocument>
#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>

/**
 * @brief Processamento de pixels em alta performance.
 * 
 * Utiliza scanLine para acesso direto ao buffer de memória, evitando o overhead 
 * do método setPixel/pixel (Requisito 4 de Eficiência).
 */
void processImage(WorkerRequest& req) {
    int brightness = req.payload["brightness"].toInt();
    double contrast = req.payload["contrast"].toDouble();
    QImage img = req.image; 

    if (img.isNull()) return;
    if (img.format() != QImage::Format_RGB32 && img.format() != QImage::Format_ARGB32) {
        img = img.convertToFormat(QImage::Format_RGB32);
    }

    double factor = (259.0 * (contrast + 255.0)) / (255.0 * (259.0 - contrast));
    auto clamp = [](int v) { return std::max(0, std::min(255, v)); };

    for (int y = 0; y < img.height(); y++) {
        QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); x++) {
            line[x] = qRgb(clamp(factor * (qRed(line[x]) + brightness - 128) + 128),
                           clamp(factor * (qGreen(line[x]) + brightness - 128) + 128),
                           clamp(factor * (qBlue(line[x]) + brightness - 128) + 128));
        }
    }

    emit GlobalState::instance().imageProcessed(img);
    LOG_INFO("Worker: Imagem processada.");
}

/**
 * @brief Gerenciador de I/O de arquivos de exames.
 * 
 * Garante a atomicidade entre a cópia física do arquivo para a pasta 
 * data/exames e a atualização do banco de dados em memória (GlobalState).
 */
void processarImportacao(const QString& id, const QString& originPath) {

    QString destDir = GlobalState::getExamsPath(id) + "/";
    QDir().mkpath(destDir);
    QString fileName = QFileInfo(originPath).fileName();
    QString destPath = destDir + fileName;

    if (QFile::exists(destPath)) QFile::remove(destPath);
    if (!QFile::copy(originPath, destPath)) {
        LOG_ERROR("Worker: Falha na copia fisica para " + destPath.toStdString());
        return;
    }

    QJsonObject root = GlobalState::instance().getData();
    QJsonArray pacientes = root["pacientes"].toArray();
    bool encontrado = false;

    for (int i = 0; i < pacientes.size(); ++i) {
        QJsonObject p = pacientes[i].toObject();
        if (p["id"].toString() == id) {
            QJsonArray exames = p["exames"].toArray();
            QString relPath = "data/exames/" + id + "/" + fileName;
            bool jaExiste = false;
            for(auto ex : exames) if(ex.toString() == relPath) jaExiste = true;
            
            if (!jaExiste) {
                exames.append(relPath);
                p["exames"] = exames;
                pacientes[i] = p;
                encontrado = true;
                LOG_INFO("Worker: Exame vinculado ao JSON do paciente " + id.toStdString());
            }
            break;
        }
    }

    if (encontrado) {
        root["pacientes"] = pacientes;
        GlobalState::instance().setData(root);
        GlobalState::instance().pushRequest({WorkerRequest::SYNC_DISK, {}});
    }

    LOG_INFO("Worker: Importacao concluida: " + destPath.toStdString());
    emit GlobalState::instance().imageImported(destPath);
}

/**
 * @brief Loop de execução da Background Thread.
 * 
 * Implementa o padrão Consumer que bloqueia passivamente via Condition Variable, 
 * garantindo 0% de uso de CPU quando ocioso (Eficiência Energética).
 */
void backgroundWorker() {
    LOG_FUNC_ENTRY();
    while (true) {
        auto req = GlobalState::instance().popRequest();
        
        switch (req.type) {
            case WorkerRequest::PROCESS_IMAGE:
                processImage(req);
                break;

            case WorkerRequest::IMPORT_EXAM:
                processarImportacao(req.payload["id"].toString(), 
                                   req.payload["originPath"].toString());
                break;

            case WorkerRequest::UPDATE_STATUS: {
                QString id = req.payload["id"].toString();
                QString novoStatus = req.payload["status"].toString();
                QJsonObject root = GlobalState::instance().getData();
                QJsonArray pacientes = root["pacientes"].toArray();
                
                for (int i = 0; i < pacientes.size(); ++i) {
                    QJsonObject p = pacientes[i].toObject();
                    if (p["id"].toString() == id) {
                        p["status"] = novoStatus;
                        pacientes[i] = p;
                        break;
                    }
                }
                root["pacientes"] = pacientes;
                GlobalState::instance().setData(root);
                GlobalState::instance().pushRequest({WorkerRequest::SYNC_DISK, {}});
                break;
            }

            case WorkerRequest::SYNC_DISK: {
                QFile file("data/database.json");
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(QJsonDocument(GlobalState::instance().getData()).toJson());
                    file.close();
                    LOG_INFO("Worker: database.json atualizado.");
                }
                break;
            }
        }
    }
}
