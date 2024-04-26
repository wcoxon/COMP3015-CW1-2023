#pragma once
class Material
{
public:
    bool shadeFlat = false;

    float ambientReflectivity{ .5 };
    float diffuseReflectivity{ .5 };
    float specularReflectivity{ .5 };
    int specularPower{ 10 };

    bool perFragment = true;
};
