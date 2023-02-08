#include "ExternalField.h"
#include <memory>

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
    return {out << '(' << rhs.data[0] << ',' << ' ' << rhs.data[1] << ',' << ' ' << rhs.data[2] << ')'};
}

std::string Vector::tostring() const
{
    return {"(" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]) + ")"};
}

std::string Charge::tostring() const
{
    return {point.tostring() + ", " + std::to_string(Q) + ", " + std::to_string(q)};
}

double Abs(const Vector& _vector) // модуль вектора
{
    return std::sqrt(_vector.Dot(_vector));
}

Charge Charge::GetMirror()
{
    return Charge{Vector{point.data[0], point.data[1], -point.data[2]},-q, -Q};
}

