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
    void DeduceMult(double E_plus, double h)
    {

        double E = (getValue(Vector{0,0,a + h}) - getValue(Vector{0,0,a})) / h;
        double d = 3 * E_plus / E;
        mult *= d;
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

struct ChargeLayer
{
    double p_0;
    double h;
    double L;
    Vector r;
    double a;
};

class TableField : public ExternalField
{
    std::vector<std::vector<std::vector<double>>> values_;
    Vector r_;
    double h_;

public:
    TableField(const std::vector<ChargeLayer>& layers, 
    double start_x, double start_y, double start_z,
    double end_x, double end_y, double end_z, double h) : 
    values_((end_x - start_x) / h, std::vector<std::vector<double>>((end_y - start_y) / h, std::vector<double>((end_z - start_z) / h))), 
    r_{start_x, start_y, start_z}, h_(h)
    {
        std::vector<VertexPtr> vertices;
        for (int i = 0; i < values_.size(); ++i)
        {
            for (int j = 0; j < values_[0].size(); ++j)
            {
                for (int k = 0; k < values_[0][0].size(); ++k)
                {
                    double q = 0;
                    Vector point = r_ + Vector{h_ * i, h_* j, h_ * k};
                    for (auto& layer: layers)
                    {
                        Vector cur =  point - layer.r;
                        
                        auto expr = -std::pow(std::sqrt(cur.z() * cur.z()) / layer.h, 2 * layer.a)-std::pow(std::sqrt(cur.x() * cur.x() + cur.y() * cur.y()) / layer.L, 2 * layer.a);
                        q += layer.p_0 * std::exp(expr) * h * h * h;
                    }
                    vertices.push_back(std::make_shared<Vertex>(point, q, 0));
                }
            }
        }
        for (int i = 0; i < values_.size(); ++i)
        {
            for (int j = 0; j < values_[0].size(); ++j)
            {
                for (int k = 0; k < values_[0][0].size(); ++k)
                {
                    Vector point = r_ + Vector{h_ * i, h_* j, h_ * k};
                    values_[i][j][k] = Potential(vertices, point, h_);
                }
            }
        }
    }

    virtual double getValue(const Vector& r) override
    {
        Vector shift = r - r_;
        if (
            shift.x() / h_ >= values_.size() || shift.y() / h_ >= values_[0].size() || shift.z() / h_ >= values_[0][0].size() ||
            shift.x() / h_ < 0 || shift.y() / h_ < 0 || shift.z() / h_ < 0)
        {
            throw std::runtime_error{"Выход за границу расчетной области!"};
        }
        return values_[shift.x() / h_][shift.y() / h_][shift.z() / h_];
    }
};
