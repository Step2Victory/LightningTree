#include "Vector.h"
#include "cmath"
#include <string>

double Vector::x() const
{
    return data[0];
}
double Vector::y() const
{
    return data[1];
}

double Vector::z() const
{
    return data[2];
}

Vector Vector::operator-=(const Vector& rhs)
{
    data[0] -= rhs.data[0];
    data[1] -= rhs.data[1];
    data[2] -= rhs.data[2];

    return *this;
}

Vector Vector::operator-(const Vector& rhs) const
{
    return {{data[0] - rhs.data[0], data[1] - rhs.data[1], data[2] - rhs.data[2]}};
}

Vector Vector::operator+=(const Vector& rhs)
{
    data[0] += rhs.data[0];
    data[1] += rhs.data[1];
    data[2] += rhs.data[2];

    return *this;
}

Vector Vector::operator+(const Vector& rhs) const
{
    return {{data[0] + rhs.data[0], data[1] + rhs.data[1], data[2] + rhs.data[2]}};
}

double Vector::Dot(const Vector& rhs) const // скалярное произведение
{
    return data[0] * rhs.data[0] + data[1] * rhs.data[1] + data[2] * rhs.data[2];
}

Vector Vector::operator-() const
{
    return {-data[0], -data[1], -data[2]};
}

std::ostream& operator<<(std::ostream& out, const Vector& rhs)
{
    return out << rhs.x() << ' ' << rhs.y() << ' ' << rhs.z();
}

double Abs(const Vector& vector)
{
    return std::sqrt(vector.Dot(vector));
}

std::string Vector::tostring() const
{
    return {"(" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]) + ")"};
}