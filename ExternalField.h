#pragma once
#include <cmath>
#include <vector>

#include "Vector.h"
#include "Vertex.h"


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
        return r.z() * electricity;
    }
};

class NormalField : public ExternalField
{
    double sigma, a;
    double mult;
public:
    NormalField(double sigma, double a, double mult) : sigma(sigma), a(a), mult(mult) {};
    virtual double getValue(const Vector& r) override
    {
        return (1 + std::erf((r.z() - a) / (std::sqrt(2) * sigma))) * mult;
        // return 0.5 * (1 + std::erf((r.data[2] - a) / (std::sqrt(2) * sigma)));
    }
};

class ChargeField : public ExternalField
{
    const double eps = 1e-6;
    std::vector<VertexPtr> charges;

public:
    ChargeField(const std::initializer_list<VertexPtr>& charges) : charges(charges) {};
    
    virtual double getValue(const Vector& r) override
    {
        return Potential(charges, r, eps);
    }
};
