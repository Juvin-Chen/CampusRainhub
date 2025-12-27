/*
    管理员后台入口
*/
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QSqlDatabase>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 设置Qt插件路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString pluginPath = QDir(appDir).absoluteFilePath("sqldrivers");
    QCoreApplication::addLibraryPath(pluginPath);
    
    qDebug() << "[RainHub Admin] Application dir:" << appDir;
    qDebug() << "[RainHub Admin] Available SQL drivers:" << QSqlDatabase::drivers();
    
    AdminMainWindow w;
    w.show();
    return app.exec();
}

