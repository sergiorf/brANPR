#pragma once

#include <QFile>
#include <QDialog>
#include <string>
#include <vector>

namespace brANPR
{
  struct AnalyzerData
  {
    std::string truePlate;
    std::string detectedPlate;
    std::string fileName;
  };

  struct AnalyzerResult
  {
    std::vector<AnalyzerData> data;
    size_t ncorrect0;
    size_t ncorrect1;
    size_t ncorrect2;
  };

  class SummaryDialog : public QDialog
  {
    Q_OBJECT

  public:
    explicit SummaryDialog(const AnalyzerResult& result, QWidget* parent = 0);
  };

  class Analyzer
  {
  public:
    Analyzer(QFile& summary);
    AnalyzerResult run();
  private:
    std::vector<AnalyzerData> _data;
  };
};
