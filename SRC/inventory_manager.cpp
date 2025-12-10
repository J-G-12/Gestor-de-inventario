#include "inventory_manager.h"
#include <QDebug>

InventoryManager::InventoryManager(QObject* parent) 
    : QObject(parent), dbManager(DatabaseManager::getInstance()) {
    
    connect(dbManager, &DatabaseManager::dataChanged,
            this, &InventoryManager::inventoryChanged);
    connect(dbManager, &DatabaseManager::errorOccurred,
            this, &InventoryManager::error);
}

InventoryManager::~InventoryManager() {
    
}

bool InventoryManager::initialize() {
    bool success = dbManager->initialize();
    if (!success) {
        emit error("No se pudo inicializar el sistema de base de datos");
    }
    return success;
}

bool InventoryManager::addComponent(const QString& name, const QString& type, int quantity,
                                  const QString& location, const QDate& purchaseDate) {

    if (name.isEmpty() || type.isEmpty() || location.isEmpty()) {
        emit error("Nombre, tipo y ubicación son obligatorios");
        return false;
    }
    
    if (quantity < 0) {
        emit error("La cantidad no puede ser negativa");
        return false;
    }
    
    Component component(-1, name, type, quantity, location, purchaseDate);
    bool success = dbManager->addComponent(component);
    
    if (success && quantity <= 5) {
        // Verificar si hay alerta de stock bajo
        checkLowStock();
    }
    
    return success;
}

bool InventoryManager::updateComponent(const Component& component) {

    if (component.getName().isEmpty() || component.getType().isEmpty() || 
        component.getLocation().isEmpty()) {
        emit error("Nombre, tipo y ubicación son obligatorios");
        return false;
    }
    
    if (component.getQuantity() < 0) {
        emit error("La cantidad no puede ser negativa");
        return false;
    }
    
    bool success = dbManager->updateComponent(component);
    
    if (success && component.getQuantity() <= 5) {

        checkLowStock();
    }
    
    return success;
}

bool InventoryManager::removeComponent(int id) {
    return dbManager->deleteComponent(id);
}

QVector<Component> InventoryManager::getAllComponents() {
    return dbManager->getAllComponents();
}

QVector<Component> InventoryManager::searchComponents(const QString& searchText) {
    return dbManager->searchComponents(searchText);
}

QVector<Component> InventoryManager::getLowStockAlert(int threshold) {
    QVector<Component> lowStock = dbManager->getLowStockComponents(threshold);
    
    if (!lowStock.isEmpty()) {
        emit lowStockAlert(lowStock);
    }
    
    return lowStock;
}

bool InventoryManager::adjustQuantity(int id, int delta, const QString& reason) {
    bool success = dbManager->updateQuantity(id, delta);
    
    if (success) {
        // Verificar stock después del ajuste
        Component comp = getComponentById(id);
        if (comp.getId() != -1 && comp.getQuantity() <= 5) {
            checkLowStock();
        }
    }
    
    return success;
}

Component InventoryManager::getComponentById(int id) {
    return dbManager->getComponentById(id);
}

void InventoryManager::checkLowStock() {
    QVector<Component> lowStock = getLowStockAlert();
    if (!lowStock.isEmpty()) {
        qWarning() << "⚠️  Alerta: Componentes con stock bajo detectados";
    }
}
