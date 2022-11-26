#pragma once
#include<vector>
#include<array>
#include<map>
#include<string>

enum FieldType {constField, gaussField};


struct Vector
{
    std::array<double, 3> data;

    // перегружены арифметические операции с векторами
    Vector operator-=(const Vector& rhs)
    {
        data[0] -= rhs.data[0];
        data[1] -= rhs.data[1];
        data[2] -= rhs.data[2];
        return *this;
    }

    Vector operator-(const Vector& rhs) const
    {
        return {{data[0] - rhs.data[0], data[1] - rhs.data[1], data[2] - rhs.data[2]}};
    }

    Vector operator+=(const Vector& rhs)
    {
        data[0] += rhs.data[0];
        data[1] += rhs.data[1];
        data[2] += rhs.data[2];
        return *this;
    }

    Vector operator+(const Vector& rhs) const
    {
        return {{data[0] + rhs.data[0], data[1] + rhs.data[1], data[2] + rhs.data[2]}};
    }
    
    double Dot(const Vector& rhs) const // скалярное произведение
    {
        return data[0] * rhs.data[0] + data[1] * rhs.data[1] + data[2] * rhs.data[2];
    }

    Vector operator-() const
    {
        return {-data[0], -data[1], -data[2]};
    }

};

class ExternalField
{
private:
    FieldType type;
    std::map<std::string, double> params;
public:
    ExternalField(FieldType type, std::map<std::string, double> params) : type(type), params(params)
    {}
    double getValue(const Vector& r)
    {
        if (type == FieldType::constField) // поле направлено вдоль оси z, значение напряженности хранится по ключу electricity
        {
            return r.data[2] * params["electricity"]; // потенциал отсчитываем от начала координат
        }
        return 0;
    }
    ~ExternalField();
};


double Abs(const Vector& vector) // модуль вектора
{
    return std::sqrt(vector.Dot(vector));
}