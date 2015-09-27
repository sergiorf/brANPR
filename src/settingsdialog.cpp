#include <QtWidgets>
#include <QString>

#include "settingsdialog.h"
#include "anprengine.h"

namespace brANPR
{
  SettingsDialog::SettingsDialog(const ANPRSettings& settings, QWidget* parent)
    : QDialog(parent)
  {
    QFileInfo fileInfo("");

    tabWidget = new QTabWidget;
    _ocrTab = new OCRTab(settings.OCR);
    tabWidget->addTab(_ocrTab, tr("OCR"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
      | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Settings"));
  }

  ANPRSettings SettingsDialog::getSettings() const
  {
    ANPRSettings settings;
    settings.OCR = _ocrTab->getSettings();
    return settings;
  }

  OCRTab::OCRTab(const OCRSettings& settings, QWidget* parent)
    : QWidget(parent)
  {
    QLabel* trainFileLabel = new QLabel(tr("Train File:"));
    _trainFileEdit = new QLineEdit(QString(settings.trainFile.c_str()));
    QLabel* trainingDataPathLabel = new QLabel(tr("Train Data Path:"));
    _trainingDataPathEdit = new QLineEdit(QString(settings.trainingDataPath.c_str()));
    QLabel* segmentsStoreLabel = new QLabel(tr("Segments Store:"));
    _segmentsStoreEdit = new QLineEdit(QString(settings.segmentsStore.c_str()));
    _saveSegments = new QCheckBox(tr("Save Segments"));
    if (settings.saveSegments)
      _saveSegments->setChecked(true);
    _showSteps = new QCheckBox(tr("Show Steps"));
    if (settings.showSteps)
      _showSteps->setChecked(true);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(trainFileLabel);
    mainLayout->addWidget(_trainFileEdit);
    mainLayout->addWidget(trainingDataPathLabel);
    mainLayout->addWidget(_trainingDataPathEdit);
    mainLayout->addWidget(segmentsStoreLabel);
    mainLayout->addWidget(_segmentsStoreEdit);
    mainLayout->addWidget(_saveSegments);
    mainLayout->addWidget(_showSteps);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
  }

  OCRSettings OCRTab::getSettings() const
  {
    OCRSettings settings;
    settings.trainFile = _trainFileEdit->text().toUtf8().constData();
    settings.trainingDataPath = _trainingDataPathEdit->text().toUtf8().constData();
    settings.segmentsStore = _segmentsStoreEdit->text().toUtf8().constData();
    settings.saveSegments = _saveSegments->isChecked();
    settings.showSteps = _showSteps->isChecked();
    return settings;
  }
};