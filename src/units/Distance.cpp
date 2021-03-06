#include "Distance.h"

namespace sim {

Distance::Distance() : m_meters(0.0) {
}

Distance::~Distance() {
}

float Distance::getMeters() const {
    return m_meters;
}

float Distance::getCentimeters() const {
    return getMeters() * 100;
}

bool Distance::operator==(const Distance& distance) const {
    return getMeters() == distance.getMeters();
}

bool Distance::operator!=(const Distance& distance) const {
    return getMeters() != distance.getMeters();
}

bool Distance::operator<(const Distance& distance) const {
    return getMeters() < distance.getMeters();
}

Seconds Distance::operator/(const Speed& speed) const {
    return Seconds(getMeters()/speed.getMetersPerSecond());
}

} // namespace sim
