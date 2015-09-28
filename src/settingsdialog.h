#pragma once

#include <QDialog>

class QDialogButtonBox;
class QFileInfo;
class QTabWidget;
class QCheckBox;
class QLineEdit;

namespace brANPR
{
  struct ANPRSettings;
  struct OCRSettings;
  struct DetectRegionsSettings;

  class OCRTab : public QWidget
  {
    Q_OBJECT

    friend class SettingsDialog;
  public:
    explicit OCRTab(const OCRSettings& settings, QWidget* parent = 0);
    OCRSettings getSettings() const;
  private:
    QLineEdit* _trainFileEdit;
    QLineEdit* _trainingDataPathEdit;
    QLineEdit* _segmentsStoreEdit;
    QCheckBox* _saveSegments;
    QCheckBox* _showSteps;
  };

  class DetectRegionsTab : public QWidget
  {
    Q_OBJECT

    friend class SettingsDialog;
  public:
    explicit DetectRegionsTab(const DetectRegionsSettings& settings, QWidget* parent = 0);
    DetectRegionsSettings getSettings() const;
  private:
    QCheckBox* _saveSegments;
    QCheckBox* _showSteps;
  };

  class SettingsDialog : public QDialog
  {
    Q_OBJECT

  public:
    explicit SettingsDialog(const ANPRSettings& settings, QWidget* parent = 0);
    ANPRSettings getSettings() const;
  private:
    QTabWidget* tabWidget;
    QDialogButtonBox* buttonBox;
    OCRTab* _ocrTab;
    DetectRegionsTab* _detectRegionsTab;
  };
};
