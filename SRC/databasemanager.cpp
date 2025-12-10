#include "databasemanager.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QSqlError>
#include <QDebug>

DatabaseManager* DatabaseManager::instance = nullptr;
QMutex DatabaseManager::mutex;

DatabaseManager::DatabaseManager(QObject* parent) 
    : QObject(parent) {

    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    dbPath = dataDir + "/inventory.db";
    qDebug() << "Ruta de base de datos:" << dbPath;
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

DatabaseManager* DatabaseManager::getInstance() {
    QMutexLocker locker(&mutex);
    if (!instance) {
        instance = new DatabaseManager();
    }
    return instance;
}

bool DatabaseManager::initialize() {

    db = QSqlDatabase::addDatabase("QSQLITE", "inventory_connection");
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        QString error = "No se pudo abrir la base de datos: " + db.lastError().text();
        qCritical() << error;
        emit errorOccurred(error);
        return false;
    }
    
    qDebug() << "Base de datos abierta exitosamente";
    return createTables();
}

bool DatabaseManager::createTables() {
    QSqlQuery query(db);
    
    QString createTable = 
        "CREATE TABLE IF NOT EXISTS componentes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "type TEXT NOT NULL,"
        "quantity INTEGER NOT NULL CHECK(quantity >= 0),"
        "location TEXT NOT NULL,"
        "purchase_date TEXT NOT NULL)";
    
    if (!query.exec(createTable)) {
        QString error = "Error creando tabla: " + query.lastError().text();
        qCritical() << error;
        emit errorOccurred(error);
        return false;
    }
    
    qDebug() << "Tabla 'componentes' creada/verificada";
    return true;
}

bool DatabaseManager::addComponent(const Component& component) {
    QSqlQuery query(db);
    
    query.prepare(
        "INSERT INTO componentes (name, type, quantity, location, purchase_date) "
        "VALUES (:name, :type, :quantity, :location, :date)"
    );
    
    query.bindValue(":name", component.getName());
    query.bindValue(":type", component.getType());
    query.bindValue(":quantity", component.getQuantity());
    query.bindValue(":location", component.getLocation());
    query.bindValue(":date", component.getPurchaseDate().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QString error = "Error agregando componente: " + query.lastError().text();
        qCritical() << error;
        emit errorOccurred(error);
        return false;
    }
    
    qDebug() << "Componente agregado, ID:" << query.lastInsertId();
    emit dataChanged();
    return true;
}

bool DatabaseManager::updateComponent(const Component& component) {
    QSqlQuery query(db);
    
    query.prepare(
        "UPDATE componentes SET "
        "name = :name, type = :type, quantity = :quantity, "
        "location = :location, purchase_date = :date "
        "WHERE id = :id"
    );
    
    query.bindValue(":id", component.getId());
    query.bindValue(":name", component.getName());
    query.bindValue(":type", component.getType());
    query.bindValue(":quantity", component.getQuantity());
    query.bindValue(":location", component.getLocation());
    query.bindValue(":date", component.getPurchaseDate().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QString error = "Error actualizando componente: " + query.lastError().text();
        qCritical() << error;
        emit errorOccurred(error);
        return false;
    }
    
    bool updated = query.numRowsAffected() > 0;
    if (updated) {
        emit dataChanged();
    }
    return updated;
}

bool DatabaseManager::deleteComponent(int id) {
    QSqlQuery query(db);
    
    query.prepare("DELETE FROM componentes WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        QString error = "Error eliminando componente: " + query.lastError().text();
        qCritical() << error;
        emit errorOccurred(error);
        return false;
    }
    
    bool deleted = query.numRowsAffected() > 0;
    if (deleted) {
        emit dataChanged();
    }
    return deleted;
}

Component DatabaseManager::getComponentById(int id) {
    QSqlQuery query(db);
    
    query.prepare("SELECT * FROM componentes WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec() || !query.next()) {
        qWarning() << "Componente no encontrado, ID:" << id;
        return Component();
    }
    
    return queryToComponent(query);
}

QVector<Component> DatabaseManager::getAllComponents() {
    QVector<Component> components;
    QSqlQuery query(db);
    
    if (!query.exec("SELECT * FROM componentes ORDER BY name")) {
        QString error = "Error obteniendo componentes: " + query.lastError().text();
        qCritical() << error;
        emit errorOccurred(error);
        return components;
    }
    
    while (query.next()) {
        components.append(queryToComponent(query));
    }
    
    qDebug() << "Obtenidos" << components.size() << "componentes";
    return components;
}

QVector<Component> DatabaseManager::searchComponents(const QString& searchText) {
    QVector<Component> components;
    
    if (searchText.isEmpty()) {
        return getAllComponents();
    }
    
    QSqlQuery query(db);
    
    query.prepare(
        "SELECT * FROM componentes WHERE "
        "name LIKE :search OR type LIKE :search OR location LIKE :search "
        "ORDER BY name"
    );
    
    QString likeTerm = "%" + searchText + "%";
    query.bindValue(":search", likeTerm);
    
    if (!query.exec()) {
        QString error = "Error buscando componentes: " + query.lastError().text();
        qCritical() << error;
        emit errorOccurred(error);
        return components;
    }
    
    while (query.next()) {
        components.append(queryToComponent(query));
    }
    
    qDebug() << "Búsqueda '" << searchText << "':" << components.size() << "resultados";
    return components;
}

QVector<Component> DatabaseManager::getLowStockComponents(int threshold) {
    QVector<Component> components;
    QSqlQuery query(db);
    
    query.prepare("SELECT * FROM componentes WHERE quantity <= :threshold ORDER BY quantity");
    query.bindValue(":threshold", threshold);
    
    if (!query.exec()) {
        QString error = "Error obteniendo stock bajo: " + query.lastError().text();
        qCritical() << error;
        emit errorOccurred(error);
        return components;
    }
    
    while (query.next()) {
        components.append(queryToComponent(query));
    }
    
    if (!components.isEmpty()) {
        qWarning() << "¡ALERTA!" << components.size() << "componentes con stock bajo";
    }
    
    return components;
}

bool DatabaseManager::updateQuantity(int id, int delta) {
    QSqlQuery query(db);
    
    // Obtener cantidad actual
    query.prepare("SELECT quantity FROM componentes WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec() || !query.next()) {
        QString error = "Componente no encontrado para actualizar cantidad, ID: " + QString::number(id);
        qCritical() << error;
        emit errorOccurred(error);
        return false;
    }
    
    int currentQty = query.value(0).toInt();
    int newQty = currentQty + delta;
    
    if (newQty < 0) {
        QString error = "No se puede tener cantidad negativa para componente ID: " + QString::number(id);
        qWarning() << error;
        emit errorOccurred(error);
        return false;
    }
    
    query.prepare("UPDATE componentes SET quantity = :quantity WHERE id = :id");
    query.bindValue(":quantity", newQty);
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        QString error = "Error actualizando cantidad: " + query.lastError().text();
        qCritical() << error;
        emit errorOccurred(error);
        return false;
    }
    
    qDebug() << "Cantidad actualizada, ID:" << id << "de" << currentQty << "a" << newQty;
    emit dataChanged();
    return true;
}

Component DatabaseManager::queryToComponent(const QSqlQuery& query) {
    return Component(
        query.value("id").toInt(),
        query.value("name").toString(),
        query.value("type").toString(),
        query.value("quantity").toInt(),
        query.value("location").toString(),
        query.value("purchase_date").toDate()
    );
}
