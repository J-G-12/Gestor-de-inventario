#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Gestor de Inventario IoT");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
