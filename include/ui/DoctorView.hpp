/**
 * @file DoctorView.hpp
 * @brief Interface de visualização de exames médicos.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef DOCTORVIEW_HPP
#define DOCTORVIEW_HPP

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QMouseEvent>
#include <QWheelEvent>
#include "fslogging.hpp"

/**
 * @class DoctorView
 * @brief classe responsável pela renderização da imagem e processamento de brilho/contraste.
 */
class DoctorView : public QWidget {
    Q_OBJECT
public:
    explicit DoctorView(QWidget *parent = nullptr);
	void loadImage(const QString& path);

signals:
    void logoutRequested();

private slots:
	void requestImageUpdate();
	void updateDisplay();
	

private:
    void setupUI();
    QImage m_originalImage;
	QImage m_currentProcessedImage;
    QLabel* m_imageLabel = nullptr;
    QSlider* m_brightnessSlider = nullptr;
    QSlider* m_contrastSlider = nullptr;
    QPushButton* m_btnLogout = nullptr;
    QPoint m_lastMousePos;
    bool m_isDragging = false;	
    double m_zoomFactor = 1.0;
    QPoint m_panOffset = {0, 0};	
	
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;	
};

#endif

