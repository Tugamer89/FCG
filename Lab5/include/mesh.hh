#ifndef MESH_HH
#define MESH_HH

#include <fstream>
#include <glm/vec3.hpp>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>

struct Mesh {
   public:
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec3> indices;
    std::string filename = "";

    Mesh() = default;

    explicit Mesh(const std::string& filename) : filename(filename) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error: Failed to open file: " << filename << std::endl;
            exit(1);
        }

        std::string line;

        // Read OFF header
        std::getline(file, line);
        if (line != "OFF") {
            std::cerr << "Error: Invalid OFF file: missing OFF header" << std::endl;
            exit(1);
        }

        // Skip comments and empty lines
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }
            break;
        }

        // Parse header: vnum fnum ednum
        std::istringstream headerStream(line);
        unsigned int vnum, fnum, ednum;  // NOSONAR
        if (!(headerStream >> vnum >> fnum >> ednum)) {
            std::cerr << "Error: Invalid OFF header format" << std::endl;
            exit(1);
        }

        if (ednum != 0) {
            std::cerr << "Error: Edges not supported" << std::endl;
            exit(1);
        }

        vertices.reserve(vnum * 2);  // reserve space for vertices and dummy colors
        indices.reserve(fnum);

        // Read vertices
        for (unsigned int i = 0; i < vnum; ++i) {
            float x, y, z;  // NOSONAR
            if (!(file >> x >> y >> z)) {
                std::cerr << "Error: Failed to read vertex data at index " << i << std::endl;
                exit(1);
            }
            vertices.emplace_back(x, y, z);
            vertices.emplace_back(1.0f, 1.0f, 1.0f);  // dummy color attribute
        }

        // Read faces
        for (unsigned int i = 0; i < fnum; ++i) {
            unsigned int vcount;

            if (!(file >> vcount)) {
                std::cerr << "Error: Failed to read face count at face " << i << std::endl;
                exit(1);
            }

            if (vcount == 3) {
                glm::uvec3 triangle;

                if (!(file >> triangle[0] >> triangle[1] >> triangle[2])) {
                    std::cerr << "Error: Failed to read triangle indices at face " << i << std::endl;
                    exit(1);
                }
                indices.push_back(triangle);
            } else {
                std::cerr << "Error: Face " << i << " is not a triangle" << std::endl;
                exit(1);
            }
        }

        file.close();
    }
};

#endif
