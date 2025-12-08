#pragma once
#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QTabWidget>
#include <QGroupBox>
#include <QListWidget>
#include <QFileDialog>
#include "FTDScopeView.h"

// Forward declarations
class FTDDataSource;
class FTDController;
class FTDFTDISource;

class FTDMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit FTDMainWindow(FTDController* controller = nullptr, QWidget* parent = nullptr);
    ~FTDMainWindow();

private slots:
    void onSourceChanged(int idx);
    void onStartStop();
    void onTimebaseChanged(int idx);
    void onScaleChanged(int idx);
    void onSampleTimeChanged();
    void onSampleDurationChanged();
    void onWaveTest();
    
    // I/O Operations
    void onReadBytes();
    void onWriteBytes();
    
    // Filters
    void onAddShiftFilter();
    void onAddScaleFilter();
    void onExecutePipe();
    void onClearFilters();
    
    // File Operations
    void onReadFile();
    void onWriteFile();
    void onBrowseReadFile();
    void onBrowseWriteFile();

private:
    void buildUi();
    void buildOscilloscopeTab(QWidget* parent);
    void buildIOTab(QWidget* parent);
    void buildFiltersTab(QWidget* parent);
    void buildFileTab(QWidget* parent);

    FTDScopeView* m_view = nullptr;
    QTabWidget* m_tabs = nullptr;
    
    // Oscilloscope controls
    QComboBox* m_sourceCombo = nullptr;
    QPushButton* m_startStop = nullptr;
    QComboBox* m_timebaseCombo = nullptr;
    QComboBox* m_scaleCombo = nullptr;
    QDoubleSpinBox* m_sampleTime = nullptr;
    QComboBox* m_sampleTimeUnit = nullptr;
    QDoubleSpinBox* m_sampleDuration = nullptr;
    QComboBox* m_sampleDurationUnit = nullptr;
    QPushButton* m_waveTestButton = nullptr;
    QSpinBox* m_waveTestSamples = nullptr;
    QSpinBox* m_waveTestAmplitude = nullptr;
    
    // I/O controls
    QSpinBox* m_readCount = nullptr;
    QPushButton* m_readButton = nullptr;
    QTextEdit* m_readDisplay = nullptr;
    QLineEdit* m_writeByteValue = nullptr;
    QSpinBox* m_writeCount = nullptr;
    QPushButton* m_writeButton = nullptr;
    
    // Filter controls
    QSpinBox* m_shiftOffset = nullptr;
    QPushButton* m_addShiftButton = nullptr;
    QSpinBox* m_scaleFactor = nullptr;
    QPushButton* m_addScaleButton = nullptr;
    QPushButton* m_executePipeButton = nullptr;
    QPushButton* m_clearFiltersButton = nullptr;
    QListWidget* m_filtersList = nullptr;
    
    // File controls
    QLineEdit* m_readFileName = nullptr;
    QPushButton* m_browseReadButton = nullptr;
    QPushButton* m_readFileButton = nullptr;
    QLineEdit* m_writeFileName = nullptr;
    QPushButton* m_browseWriteButton = nullptr;
    QPushButton* m_writeFileButton = nullptr;
    
    QLabel* m_status = nullptr;

    FTDController* m_controller = nullptr;
    FTDFTDISource* m_ftdiSource = nullptr;
    FTDDataSource* m_current = nullptr;
    bool m_running = false;
};

