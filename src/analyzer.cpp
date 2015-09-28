#include "analyzer.h"
#include "anprengine.h"
#include <QTextStream>
#include <string>
#include "utils.h"

using namespace std;

namespace brANPR
{
  Analyzer::Analyzer(QFile& summary)
  {
    if (summary.open(QIODevice::ReadOnly))
    {
      QTextStream in(&summary);
      while (!in.atEnd())
      {
        string line = in.readLine().toUtf8().constData();;
        size_t pos = 0;
        if ((pos = line.find(';')) != string::npos)
        {
          stringstream str;
          str << summary.fileName().section("/", 0, -2).toUtf8().constData();
          string plate = trim(line.substr(0, pos));
          string fileName = trim(line.substr(pos + 1, line.size() - 1));
          str << "/" << fileName;
          AnalyzerData data = { plate, "?", str.str() };
          _data.push_back(data);
        }
      }
      summary.close();
    }
  }

  AnalyzerResult Analyzer::run()
  {
    AnalyzerResult result;
    result.ncorrect2 = 0;
    result.ncorrect1 = 0;
    result.ncorrect0 = 0;
    ANPREngine engine;
    for (auto& item : _data)
    {
      int nmatch = 0;
      vector<string> plates(engine.run(item.fileName));
      for (string& plate : plates)
      {
        std::vector<char> bagOfWords(item.truePlate.begin(), item.truePlate.end());
        for (char& c : plate)
        {
          auto it = find(bagOfWords.begin(), bagOfWords.end(), c);
          if (it != bagOfWords.end())
          {
            bagOfWords.erase(it);
            nmatch++;
          }
        }
        if (nmatch)
        {
          item.detectedPlate = plate;
          break;
        }
      }
      if (nmatch >= 5)
      {
        result.ncorrect2++;
        if (nmatch >= 6)
        {
          result.ncorrect1++;
          if (nmatch == 7)
            result.ncorrect0++;
        }
      }
    }
    result.data = _data;
    return result;
  };
}