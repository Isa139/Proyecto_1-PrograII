//Configura y ejecuta la aplicacion y la muestar en la interfaz grafica y se inica el juego
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
