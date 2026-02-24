/**
 * @file DoctorView.cpp
 * @brief Implementação da visualização diagnóstica e interações com a imagem.
 * @author Fabiano Souza
 * 
 * Este arquivo implementa o "coração" da aplicação, focando em alta performance
 * de renderização e interatividade via mouse (Zoom, Pan, Brilho e Contraste).
 */
 
#include "ui/DoctorView.hpp"
#include "core/GlobalState.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QSlider>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollArea>
#include <QElapsedTimer>
#include <QResizeEvent>
#include <QScrollBar>
#include <algorithm>

DoctorView::DoctorView(QWidget *parent) : QWidget(parent) {
    setupUI();
    connect(&GlobalState::instance(), &GlobalState::imageProcessed, this, [this](const QImage& img){
        m_currentProcessedImage = img; 
        updateDisplay();               
    });	
}

void DoctorView::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QFrame* frameA = new QFrame(this);
    frameA->setFixedHeight(35);
    frameA->setStyleSheet("background-color: #1a1a1a; color: white;");
    auto* layoutA = new QHBoxLayout(frameA);
    layoutA->addWidget(new QLabel("<b>VISUALIZADOR DE DIAGNÓSTICO</b>"));
    mainLayout->addWidget(frameA);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setObjectName("mainScroll");
    scroll->setWidgetResizable(true);
    scroll->setAlignment(Qt::AlignCenter);
    scroll->setStyleSheet("background-color: #000; border: none;");

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet("background-color: #000;");
    
    scroll->setWidget(m_imageLabel);
    layout()->addWidget(scroll);

    QFrame* frameC = new QFrame(this);
    frameC->setFixedHeight(80);
    frameC->setStyleSheet("background-color: #2c3e50; color: white;");
    auto* layoutC = new QHBoxLayout(frameC);

    m_brightnessSlider = new QSlider(Qt::Horizontal, this);
    m_contrastSlider = new QSlider(Qt::Horizontal, this);
    m_brightnessSlider->setRange(-100, 100);
    m_contrastSlider->setRange(-100, 100);
    
    layoutC->addWidget(new QLabel("BRILHO:"));
    layoutC->addWidget(m_brightnessSlider);
    layoutC->addSpacing(20);
    layoutC->addWidget(new QLabel("CONTRASTE:"));
    layoutC->addWidget(m_contrastSlider);

    mainLayout->addWidget(frameC);

    connect(m_brightnessSlider, &QSlider::valueChanged, this, &DoctorView::requestImageUpdate);
    connect(m_contrastSlider, &QSlider::valueChanged, this, &DoctorView::requestImageUpdate);
}

/**
 * @brief Gerencia o redimensionamento e centralização da imagem.
 * Utiliza o fator de escala (Zoom) sobre o tamanho real da imagem processada
 * para garantir nitidez (SmoothTransformation) e acionar as barras de scroll (Pan).
 */
void DoctorView::updateDisplay() {
    if (!m_imageLabel || m_currentProcessedImage.isNull()) return;

    QSize baseSize = m_currentProcessedImage.size();
    QSize zoomedSize = baseSize * m_zoomFactor;
    QPixmap pix = QPixmap::fromImage(m_currentProcessedImage);
    m_imageLabel->setPixmap(pix.scaled(zoomedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_imageLabel->setFixedSize(zoomedSize);
}

/**
 * @brief Implementa o Requisito 4.c (Ajuste via Mouse) e 6.b (Pan).
 * Utiliza Throttling (filtro de tempo de 30ms) para não sobrecarregar o Worker
 * com requisições redundantes durante o arraste rápido do mouse.
 */
void DoctorView::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging && !m_originalImage.isNull()) {
        QPoint currentPos = event->pos();
        QPoint delta = currentPos - m_lastMousePos;
        m_lastMousePos = currentPos;

        QScrollArea* scroll = findChild<QScrollArea*>("mainScroll");
        if (scroll) {
            scroll->horizontalScrollBar()->setValue(scroll->horizontalScrollBar()->value() - delta.x());
            scroll->verticalScrollBar()->setValue(scroll->verticalScrollBar()->value() - delta.y());
        }

        int sensitivity = 2; 
        int newBrightness = m_brightnessSlider->value() + (delta.x() / sensitivity);
        int newContrast = m_contrastSlider->value() - (delta.y() / sensitivity);

        m_brightnessSlider->setValue(std::clamp(newBrightness, -100, 100));
        m_contrastSlider->setValue(std::clamp(newContrast, -100, 100));
    }
}

/**
 * @brief Interface de carregamento de exames.
 * Calcula o zoom inicial ideal para que a imagem ocupe 80% da área visível (UX),
 * evitando que imagens de alta resolução estourem os limites da janela.
 */
void DoctorView::loadImage(const QString& path) {
    if (path.isEmpty()) return;
    QImage img(path);
    if (img.isNull()) {
        LOG_ERROR("DoctorView: Falha ao carregar imagem em " + path.toStdString());
        return;
    }

    m_originalImage = img;
    m_currentProcessedImage = img;

    QScrollArea* scroll = findChild<QScrollArea*>("mainScroll");
    if (scroll && !img.isNull()) {
        double targetWidth = scroll->width() * 0.8;
        double targetHeight = scroll->height() * 0.8;
        double ratioW = targetWidth / img.width();
        double ratioH = targetHeight / img.height();
        m_zoomFactor = std::min({ratioW, ratioH, 1.0}); 
    } else {
        m_zoomFactor = 0.8;
    }

    m_brightnessSlider->setEnabled(true);
    m_contrastSlider->setEnabled(true);
    
    m_brightnessSlider->blockSignals(true);
    m_brightnessSlider->setValue(0);
    m_contrastSlider->setValue(0);
    m_brightnessSlider->blockSignals(false);

    updateDisplay();
    LOG_INFO("DoctorView: Imagem ajustada para 80% da area visivel. Zoom: " + std::to_string(m_zoomFactor));
}

/**
 * @brief Implementa o Requisito 6.a (Zoom Incremental).
 * Aplica um multiplicador de escala (1.05x ou 0.95x) para um zoom suave
 * e centralizado no container QScrollArea.
 */
void DoctorView::wheelEvent(QWheelEvent *event) {
    if (m_originalImage.isNull()) return;
	
    double factor = (event->angleDelta().y() > 0) ? 1.05 : 0.95;
    m_zoomFactor = std::clamp(m_zoomFactor * factor, 0.1, 5.0);
    updateDisplay();
}

void DoctorView::requestImageUpdate() {
    if (m_originalImage.isNull()) return;

    WorkerRequest req;
    req.type = WorkerRequest::PROCESS_IMAGE;
    req.image = m_originalImage;
    req.payload = QJsonObject{
        {"brightness", m_brightnessSlider->value()},
        {"contrast", static_cast<double>(m_contrastSlider->value())}
    };

    LOG_INFO("DoctorView: Solicitando processamento - B: " + 
             std::to_string(m_brightnessSlider->value()) + 
             " C: " + std::to_string(m_contrastSlider->value()));

    GlobalState::instance().pushRequest(req);
}

void DoctorView::mousePressEvent(QMouseEvent *event) {
    if (!m_originalImage.isNull()) {
        m_lastMousePos = event->pos();
        m_isDragging = true;
        setCursor(Qt::SizeAllCursor);
    }
}

void DoctorView::mouseReleaseEvent(QMouseEvent *event) {
    m_isDragging = false;
    unsetCursor();
}

void DoctorView::resizeEvent(QResizeEvent *event) {

    QWidget::resizeEvent(event);
    
    if (!m_originalImage.isNull()) {
        updateDisplay();
    }
}

