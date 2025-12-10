#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "component.h"
#include "inventory_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void on_addButton_clicked();
    void on_searchEdit_textChanged(const QString &text);
    
private:
    Ui::MainWindow *ui;
    InventoryManager* inventoryManager;
    QStandardItemModel* tableModel;
    void setupTable();
    void refreshTable();
};

#endif // MAINWINDOW_H
