#pragma once

#include <QDialog>

class QDialogButtonBox;
class QFileInfo;
class QTabWidget;

namespace brANPR
{
  struct ANPRSettings;
  struct OCRSettings;

  class GeneralTab : public QWidget
  {
    Q_OBJECT

  public:
    explicit GeneralTab(const QFileInfo& fileInfo, QWidget* parent = 0);
  };

  class PermissionsTab : public QWidget
  {
    Q_OBJECT

  public:
    explicit PermissionsTab(const QFileInfo& fileInfo, QWidget* parent = 0);
  };

  class ApplicationsTab : public QWidget
  {
    Q_OBJECT

  public:
    explicit ApplicationsTab(const QFileInfo& fileInfo, QWidget* parent = 0);
  };

  class OCRTab : public QWidget
  {
    Q_OBJECT

  public:
    explicit OCRTab(const OCRSettings& settings, QWidget* parent = 0);
  };

  class SettingsDialog : public QDialog
  {
    Q_OBJECT

  public:
    explicit SettingsDialog(const ANPRSettings& settings, QWidget* parent = 0);

  private:
    QTabWidget* tabWidget;
    QDialogButtonBox* buttonBox;
  };
};
