#pragma once
#include<vector>
#include<array>
#include<map>
#include<string>
#include<iostream>
#include<cmath>
#include<memory>

#ifndef ExternalField_cpp
    #define ExternalField_cpp

enum FieldType {constField, gaussField};


struct Vector
{
    std::array<double, 3> data;

    // перегружены арифметические операции с векторами
    Vector operator-=(const Vector& rhs);

    Vector operator-(const Vector& rhs) const;
    Vector operator+=(const Vector& rhs);

    Vector operator+(const Vector& rhs) const;
    
    double Dot(const Vector& rhs) const; // скалярное произведение

    Vector operator-() const;

    friend std::ostream& operator<<(std::ostream& out, const Vector& rhs);

    std::string tostring() const;
};

enum ChargeType {positive, negative, both};

struct Charge
{
    Vector point;
    double q;
    double Q;
    ChargeType type;

    Charge(const Vector& point, double q, double Q, ChargeType type = both) : point(point), q(q), Q(Q), type(type) {};
    Charge GetMirror(); 
    // auto operator<=>(const Charge& rhs) const = default;

    std::string tostring() const;
};

double Abs(const Vector& _vector);

using ChargePtr = std::shared_ptr<Charge>;

class ExternalField
{
public:
    virtual double getValue(const Vector& r) = 0;
    virtual ~ExternalField() = default;
};

class ConstField : public ExternalField
{
    double electricity;
public:
    ConstField(double electricity) : electricity(electricity) {};
    virtual double getValue(const Vector& r) override
    {
        return r.data[2] * electricity;
    }
};

class NormalField : public ExternalField
{
    double sigma, a;
public:
    NormalField(double sigma, double a) : sigma(sigma), a(a) {};
    virtual double getValue(const Vector& r) override
    {
        return 0.5 * (1 + std::erf((r.data[2] - a) / (std::sqrt(2) * sigma)));
    }
};

class ChargeField : public ExternalField
{
    double eps = 1e-6;
    double epsilon_0 = 8.85418781762 * 1e-12;
    double pi = 3.1415926535;
    std::vector<Charge> charges;

public:
    ChargeField(const std::initializer_list<Charge>& charges) : charges(charges) {};
    
    double ValueInPoint(Charge charge, const Vector& r)
    {
            if (Abs(charge.point - r) < eps)
            {
                return charge.q / (4 * pi * epsilon_0 * (eps/2));
            }
            return  charge.q / (4 * pi * epsilon_0 * (Abs(charge.point - r)));
    }

    virtual double getValue(const Vector& r) override
    {
        double ans = 0;
        for (auto charge: charges)
        {
            ans += ValueInPoint(charge, r) + ValueInPoint(charge.GetMirror(), r);
        }
        return ans;
    }
};
