#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QDate>
#include <QJsonObject>

class Component {
public:
    /// Constructor por defecto
    Component();
    

    Component(int id, const QString& name, const QString& type, int quantity,
              const QString& location, const QDate& purchaseDate);
    
    // Getters
    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getType() const { return m_type; }
    int getQuantity() const { return m_quantity; }
    QString getLocation() const { return m_location; }
    QDate getPurchaseDate() const { return m_purchaseDate; }
    
    // Setters
    void setName(const QString& name) { m_name = name; }
    void setType(const QString& type) { m_type = type; }
    void setQuantity(int quantity) { m_quantity = quantity; }
    void setLocation(const QString& location) { m_location = location; }
    void setPurchaseDate(const QDate& date) { m_purchaseDate = date; }
    
    QString toString() const;
    
    QJsonObject toJSON() const;

    static Component fromJSON(const QJsonObject& json);
    
private:
    int m_id;                   ///< Identificador único
    QString m_name;            ///< Nombre del componente
    QString m_type;            ///< Tipo/categoría
    int m_quantity;            ///< Cantidad disponible
    QString m_location;        ///< Ubicación física
    QDate m_purchaseDate;      ///< Fecha de adquisición
};

#endif // COMPONENT_H
