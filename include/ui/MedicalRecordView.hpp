/**
 * @file MedicalRecordView.hpp
 * @brief Gerencia Prontuário Médico e Gestão de Exames.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef MEDICALRECORDVIEW_HPP
#define MEDICALRECORDVIEW_HPP

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include "fslogging.hpp"
#include <QFileSystemWatcher>
#include <QListWidget>
#include <QScrollArea>
#include <QDir>
#include <QFileInfo>
#include <QFrame>

/**
 * @class MedicalRecordView
 * @brief Responsável pela exibição de dados clínicos e listagem de exames.
 */
class MedicalRecordView : public QWidget {
    Q_OBJECT

public:
    explicit MedicalRecordView(QWidget *parent = nullptr);
    ~MedicalRecordView();

    void carregarDados(const QJsonObject& dados);
	QString getAtendimentoAtualId() const { return m_dadosPaciente["id"].toString(); }

signals:
    void abrirExameSolicitado(const QString& pathImagem);
    void atendimentoFinalizado();
    void voltarSolicitado();
	void importarExameSolicitado(const QString& patientId);

private slots:
    void onVerExameClicked();
    void onFinalizarClicked();
	void atualizarListaArquivos();

private:
	QFileSystemWatcher* m_folderWatcher = nullptr;
	QListWidget* m_listaExamesWidget = nullptr;
	QPushButton* m_btnVisualizar = nullptr;
    void setupUI();

    QLabel* lblNome = nullptr;
    QLabel* lblIdade = nullptr;
    QLabel* lblID = nullptr;
    QLabel* lblQueixa = nullptr;
    QLabel* lblHistorico = nullptr;
    QLabel* lblExameSugerido = nullptr;
    
    QJsonObject m_dadosPaciente;
    QString m_caminhoExame;
};

#endif
