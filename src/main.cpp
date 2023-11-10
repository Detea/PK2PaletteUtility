#include "PK2PaletteUtility.h"
#include <qapplication.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    PK2PaletteUtility w;
    w.show();

    return a.exec();
}
