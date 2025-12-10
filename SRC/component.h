/**
 * @file component.h
 * @brief Clase Component que representa un componente electrónico
 * @author Tu Nombre
 * @date 2024
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QDate>
#include <QJsonObject>

/**
 * @class Component
 * @brief Representa un componente electrónico en el inventario
 */
class Component {
public:
    /// Constructor por defecto
    Component();
    
    /**
     * @brief Constructor parametrizado
     * @param id Identificador único
     * @param name Nombre del componente
     * @param type Tipo/categoría
     * @param quantity Cantidad disponible
     * @param location Ubicación física
     * @param purchaseDate Fecha de adquisición
     */
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
    
    /**
     * @brief Convierte el componente a cadena legible
     * @return Representación en texto
     */
    QString toString() const;
    
    /**
     * @brief Serializa a formato JSON
     * @return Objeto JSON con datos del componente
     */
    QJsonObject toJSON() const;
    
    /**
     * @brief Crea Component desde JSON
     * @param json Objeto JSON con datos
     * @return Component instanciado
     */
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
