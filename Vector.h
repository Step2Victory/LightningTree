#pragma once
#include <array>
#include <iostream>
#include "Constants.h"

struct Vector
{
    std::array<double,3> data;
    double x() const;
    double y() const;
    double z() const;
    Vector operator-=(const Vector& rhs);

    Vector operator-(const Vector& rhs) const;
    Vector operator+=(const Vector& rhs);

    Vector operator+(const Vector& rhs) const;
    
    double Dot(const Vector& rhs) const; // скалярное произведение

    Vector operator-() const;

};

std::ostream& operator<<(std::ostream& out, const Vector& rhs);
double Abs(const Vector& vector);