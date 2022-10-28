#include<vector>
#include<array>
#include<map>
#include<string>

enum FieldType {constField, gaussField};


struct Vector;

class ExternalField
{
private:
    FieldType type;
    std::map<std::string, double> params;
public:
    ExternalField(FieldType type, std::map<std::string, double> params) : type(type), params(params)
    {}
    Vector getValue(Vector r);
    ~ExternalField();
};

struct Vector
{
    std::array<double, 3> data;
};