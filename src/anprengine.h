#ifndef ANPRENGINE
#define ANPRENGINE

#include <QString>
#include <QImage>

class ANPREngine
{
public:
    ANPREngine();
    void run(const std::string& picPath);
    QImage original();
};

#endif // ANPRENGINE

