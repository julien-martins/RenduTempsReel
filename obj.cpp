#include "obj.h"

void load_obj(const char* fileName, std::vector<glm::vec4>& vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3>&normals, std::vector<GLushort>& elements)
{
    std::ifstream in(fileName, std::ios::in);
    if (!in)
    {
        std::cerr << "Cannot open " << fileName << std::endl; exit(1);
    }

    std::string line;
    while (getline(in, line))
    {
        if (line.substr(0, 2) == "v ")
        {
            std::istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
        }
        else if (line.substr(0, 2) == "vt ") {
            std::istringstream s(line.substr(2));
            glm::vec2 uv; s >> uv.x; s >> uv.y;
            uvs.push_back(uv);
        }
        else if (line.substr(0, 2) == "f ")
        {
            std::istringstream s(line.substr(2));
            GLushort a, b, c;
            s >> a; s >> b; s >> c;
            a--; b--; c--;
            elements.push_back(a); elements.push_back(b); elements.push_back(c);
        }
        /* anything else is ignored */
    }

    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
    for (int i = 0; i < elements.size(); i += 3)
    {
        GLushort ia = elements[i];
        GLushort ib = elements[i + 1];
        GLushort ic = elements[i + 2];
        glm::vec3 normal = glm::normalize(glm::cross(
            glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
            glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
    }
}