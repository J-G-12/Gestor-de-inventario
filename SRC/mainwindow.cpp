#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , inventoryManager(new InventoryManager(this))
    , tableModel(new QStandardItemModel(this))
    , currentComponentId(-1)
{
    ui->setupUi(this);
    
    setWindowTitle("Gestor de Inventario - IoT Lab");
    
    if (!inventoryManager->initialize()) {
        QMessageBox::critical(this, "Error de Inicialización",
            "No se pudo inicializar el sistema.\n"
            "Verifique que SQLite esté instalado y tenga permisos de escritura.");
    }
    
    setupTable();
    
    connect(inventoryManager, &InventoryManager::lowStockAlert,
            this, &MainWindow::onLowStockAlert);
    connect(inventoryManager, &InventoryManager::error,
            this, &MainWindow::onError);
    
    refreshTable();
    
    ui->dateEdit->setDate(QDate::currentDate());
    
    checkLowStock();
    
    showStatusMessage("Sistema listo", 3000);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupTable() {

    tableModel->setColumnCount(6);
    tableModel->setHorizontalHeaderLabels(
        {"ID", "Nombre", "Tipo", "Cantidad", "Ubicación", "Fecha Compra"});
    

    ui->tableView->setModel(tableModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
 
    ui->tableView->hideColumn(0);
    

    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    

    connect(ui->tableView, &QTableView::clicked,
            this, &MainWindow::on_tableView_clicked);
}

void MainWindow::refreshTable() {

    tableModel->removeRows(0, tableModel->rowCount());
    

    QVector<Component> components = inventoryManager->getAllComponents();
    

    for (const Component& component : components) {
        QList<QStandardItem*> row;
        

        QStandardItem* idItem = new QStandardItem(QString::number(component.getId()));
        row << idItem;
        

        QStandardItem* nameItem = new QStandardItem(component.getName());
        row << nameItem;
        

        QStandardItem* typeItem = new QStandardItem(component.getType());
        row << typeItem;
        

        QStandardItem* qtyItem = new QStandardItem(QString::number(component.getQuantity()));
        qtyItem->setTextAlignment(Qt::AlignCenter);
        row << qtyItem;
        

        QStandardItem* locItem = new QStandardItem(component.getLocation());
        row << locItem;
        

        QStandardItem* dateItem = new QStandardItem(component.getPurchaseDate().toString("dd/MM/yyyy"));
        row << dateItem;
        

        if (component.getQuantity() <= 5) {
            for (QStandardItem* item : row) {
                item->setBackground(QBrush(QColor(255, 200, 200))); // Rojo claro
                item->setForeground(QBrush(Qt::black));
            }
            qtyItem->setForeground(QBrush(Qt::red));
        }
        
        tableModel->appendRow(row);
    }
    

    showStatusMessage(QString("Total: %1 componentes").arg(components.size()));
}

void MainWindow::on_addButton_clicked() {

    Component component = getComponentFromForm();
    

    if (component.getName().isEmpty()) {
        QMessageBox::warning(this, "Validación", "El nombre es obligatorio");
        ui->nameEdit->setFocus();
        return;
    }
    
    if (component.getType().isEmpty()) {
        QMessageBox::warning(this, "Validación", "El tipo es obligatorio");
        ui->typeEdit->setFocus();
        return;
    }
    
    if (component.getQuantity() < 0) {
        QMessageBox::warning(this, "Validación", "La cantidad no puede ser negativa");
        ui->quantitySpin->setFocus();
        return;
    }
    
    if (component.getLocation().isEmpty()) {
        QMessageBox::warning(this, "Validación", "La ubicación es obligatoria");
        ui->locationEdit->setFocus();
        return;
    }
    

    if (inventoryManager->addComponent(
            component.getName(),
            component.getType(),
            component.getQuantity(),
            component.getLocation(),
            component.getPurchaseDate())) {
        
        QMessageBox::information(this, "Éxito", "Componente agregado correctamente");
        clearForm();
        refreshTable();
        
        if (component.getQuantity() <= 5) {
            checkLowStock();
        }
    }
}

void MainWindow::on_updateButton_clicked() {
    if (currentComponentId == -1) {
        QMessageBox::information(this, "Información", 
            "Seleccione un componente de la tabla para editar");
        return;
    }
    
    Component component = getComponentFromForm();
    component.setId(currentComponentId);
    
    if (component.getName().isEmpty() || component.getType().isEmpty() || 
        component.getLocation().isEmpty()) {
        QMessageBox::warning(this, "Validación", 
            "Nombre, tipo y ubicación son obligatorios");
        return;
    }
    
    if (inventoryManager->updateComponent(component)) {
        QMessageBox::information(this, "Éxito", "Componente actualizado correctamente");
        clearForm();
        refreshTable();
        checkLowStock();
    }
}

void MainWindow::on_deleteButton_clicked() {
    if (currentComponentId == -1) {
        QMessageBox::information(this, "Información", 
            "Seleccione un componente de la tabla para eliminar");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmar Eliminación",
        "¿Está seguro de eliminar este componente?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (inventoryManager->removeComponent(currentComponentId)) {
            QMessageBox::information(this, "Éxito", "Componente eliminado correctamente");
            clearForm();
            refreshTable();
        }
    }
}

void MainWindow::on_searchEdit_textChanged(const QString &text) {
    if (text.isEmpty()) {
        refreshTable();
        return;
    }
    
    tableModel->removeRows(0, tableModel->rowCount());
    
    QVector<Component> components = inventoryManager->searchComponents(text);
    
    for (const Component& component : components) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(component.getId()));
        row << new QStandardItem(component.getName());
        row << new QStandardItem(component.getType());
        
        QStandardItem* qtyItem = new QStandardItem(QString::number(component.getQuantity()));
        qtyItem->setTextAlignment(Qt::AlignCenter);
        row << qtyItem;
        
        row << new QStandardItem(component.getLocation());
        row << new QStandardItem(component.getPurchaseDate().toString("dd/MM/yyyy"));
        
        tableModel->appendRow(row);
    }
    
    showStatusMessage(QString("Búsqueda: %1 resultados").arg(components.size()));
}

void MainWindow::on_tableView_clicked(const QModelIndex &index) {
    if (!index.isValid()) return;
    
    int row = index.row();
    currentComponentId = tableModel->item(row, 0)->text().toInt();
    
    Component component = inventoryManager->getComponentById(currentComponentId);
    if (component.getId() != -1) {
        loadComponentToForm(component);
    }
}

void MainWindow::on_checkStockButton_clicked() {
    checkLowStock();
}

void MainWindow::on_clearButton_clicked() {
    clearForm();
}

void MainWindow::onLowStockAlert(const QVector<Component>& components) {
    if (components.isEmpty()) return;
    
    QString message = QString("⚠️ ALERTA: %1 componentes con stock bajo\n\n").arg(components.size());
    
    for (const Component& comp : components) {
        message += QString("• %1 (%2): %3 unidades\n")
            .arg(comp.getName())
            .arg(comp.getType())
            .arg(comp.getQuantity());
    }
    
    QMessageBox::warning(this, "Alerta de Stock Bajo", message);
    showStatusMessage(QString("Alerta: %1 componentes con stock bajo").arg(components.size()), 10000);
}

void MainWindow::onError(const QString& errorMessage) {
    QMessageBox::critical(this, "Error del Sistema", errorMessage);
    showStatusMessage("Error: " + errorMessage, 10000);
}

void MainWindow::clearForm() {
    ui->nameEdit->clear();
    ui->typeEdit->clear();
    ui->quantitySpin->setValue(1);
    ui->locationEdit->clear();
    ui->dateEdit->setDate(QDate::currentDate());
    currentComponentId = -1;
    
    // Deseleccionar tabla
    ui->tableView->clearSelection();
}

void MainWindow::loadComponentToForm(const Component& component) {
    ui->nameEdit->setText(component.getName());
    ui->typeEdit->setText(component.getType());
    ui->quantitySpin->setValue(component.getQuantity());
    ui->locationEdit->setText(component.getLocation());
    ui->dateEdit->setDate(component.getPurchaseDate());
}

Component MainWindow::getComponentFromForm() const {
    return Component(
        -1, // ID se asignará automáticamente
        ui->nameEdit->text(),
        ui->typeEdit->text(),
        ui->quantitySpin->value(),
        ui->locationEdit->text(),
        ui->dateEdit->date()
    );
}

void MainWindow::showStatusMessage(const QString& message, int timeout) {
    ui->statusbar->showMessage(message, timeout);
}

void MainWindow::checkLowStock() {
    QVector<Component> lowStock = inventoryManager->getLowStockAlert();
    
    if (!lowStock.isEmpty()) {
        QString message = QString("Alerta: %1 componentes con stock bajo").arg(lowStock.size());
        showStatusMessage("⚠️ " + message, 10000);
        
        if (lowStock.size() <= 5) {
            onLowStockAlert(lowStock);
        }
    } else {
        showStatusMessage("✅ Stock en niveles normales", 5000);
    }
}
