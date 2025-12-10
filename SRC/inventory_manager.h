#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include <QObject>
#include <QVector>
#include "component.h"
#include "databasemanager.h"


class InventoryManager : public QObject {
    Q_OBJECT
    
public:

    explicit InventoryManager(QObject* parent = nullptr);
    

    ~InventoryManager();
    

    bool initialize();
    
    bool addComponent(const QString& name, const QString& type, int quantity,
                     const QString& location, const QDate& purchaseDate);
    bool updateComponent(const Component& component);
    bool removeComponent(int id);
    QVector<Component> getAllComponents();
    QVector<Component> searchComponents(const QString& searchText);
    QVector<Component> getLowStockAlert(int threshold = 5);
    

    bool adjustQuantity(int id, int delta, const QString& reason = "");
    

    Component getComponentById(int id);
    
signals:

    void inventoryChanged();
    

    void lowStockAlert(const QVector<Component>& components);
    

    void error(const QString& errorMessage);
    
private:
    DatabaseManager* dbManager;  ///< Gestor de base de datos
};

#endif // INVENTORY_MANAGER_H
