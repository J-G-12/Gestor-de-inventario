#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
#include <QMutex>
#include "component.h"

class DatabaseManager : public QObject {
    Q_OBJECT
    
public:
   
    static DatabaseManager* getInstance();
    
   
    bool initialize();
    
    bool addComponent(const Component& component);
    bool updateComponent(const Component& component);
    bool deleteComponent(int id);
    Component getComponentById(int id);
    QVector<Component> getAllComponents();
    QVector<Component> searchComponents(const QString& searchText);
    QVector<Component> getLowStockComponents(int threshold = 5);
    

    bool updateQuantity(int id, int delta);
    
signals:

    void errorOccurred(const QString& errorMessage);
    
private:
    // Constructor privado para Singleton
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();
    
    // Eliminar operaciones de copia
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    bool createTables();
    Component queryToComponent(const QSqlQuery& query);
    
    static DatabaseManager* instance;   ///< Instancia única
    static QMutex mutex;               ///< Mutex para thread-safety
    QSqlDatabase db;                   ///< Conexión a BD
    QString dbPath;                    ///< Ruta del archivo de BD
};

#endif // DATABASEMANAGER_H
