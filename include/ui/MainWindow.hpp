/**
 * @file MainWindow.hpp
 * @brief Gerencia o ciclo de vida da aplicação e navegação entre telas.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include <QJsonObject>
#include <QLabel>
#include "fslogging.hpp"

class LoginView;
class DoctorView;
class ReceptionView;
class PatientSelectionView;
class MedicalRecordView;

/**
 * @class MainWindow
 * @brief Container principal da aplicação.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
	LoginView* getLoginView() const { return m_loginView; }
    PatientSelectionView* getPatientSelectionView() const { return m_patientSelectionView; }
    MedicalRecordView*    getMedicalRecordView()    const { return m_medicalRecordView; }	
	
private slots:
    void handleLogin(bool isDoctor); 
    void handleLogout();
    void handlePatientSelected(const QJsonObject& patientData);
    void handleOpenExam(const QString& imagePath);
    void handleFinishAttendance();	

private:
    void setupUI();
    
    QStackedWidget* m_stackedWidget = nullptr;
	LoginView* m_loginView = nullptr;
	ReceptionView* m_receptionView = nullptr; 
    PatientSelectionView* m_patientSelectionView = nullptr;
    MedicalRecordView*    m_medicalRecordView = nullptr;	
	DoctorView*           m_doctorViewWindow = nullptr; 
    QLabel* m_lblStatusUsuario = nullptr; 
	
};

#endif