/**
 * @file component.cpp
 * @brief Implementación de la clase Component
 */

#include "component.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

Component::Component() 
    : m_id(-1), m_quantity(0) {
}

Component::Component(int id, const QString& name, const QString& type, int quantity,
                   const QString& location, const QDate& purchaseDate)
    : m_id(id), m_name(name), m_type(type), m_quantity(quantity),
      m_location(location), m_purchaseDate(purchaseDate) {
}

QString Component::toString() const {
    return QString("Componente [ID: %1, Nombre: %2, Tipo: %3, Cantidad: %4, Ubicación: %5, Fecha: %6]")
        .arg(m_id)
        .arg(m_name)
        .arg(m_type)
        .arg(m_quantity)
        .arg(m_location)
        .arg(m_purchaseDate.toString("dd/MM/yyyy"));
}

QJsonObject Component::toJSON() const {
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["type"] = m_type;
    json["quantity"] = m_quantity;
    json["location"] = m_location;
    json["purchaseDate"] = m_purchaseDate.toString(Qt::ISODate);
    return json;
}

Component Component::fromJSON(const QJsonObject& json) {
    return Component(
        json["id"].toInt(),
        json["name"].toString(),
        json["type"].toString(),
        json["quantity"].toInt(),
        json["location"].toString(),
        QDate::fromString(json["purchaseDate"].toString(), Qt::ISODate)
    );
}
