#ifndef MESH_HH
#define MESH_HH

#include <fstream>
#include <glm/vec3.hpp>
#include <sstream>
#include <string>
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
            fprintf(stderr, "Error: Failed to open file: %s\n", filename.c_str());
            exit(1);
        }

        std::string line;

        // Read OFF header
        std::getline(file, line);
        if (line != "OFF") {
            fprintf(stderr, "Error: Invalid OFF file: missing OFF header\n");
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
            fprintf(stderr, "Error: Invalid OFF header format\n");
            exit(1);
        }

        if (ednum != 0) {
            fprintf(stderr, "Error: Edges not supported\n");
            exit(1);
        }

        vertices.reserve(vnum * 2);  // reserve space for vertices and dummy colors
        indices.reserve(fnum);

        // Read vertices
        for (unsigned int i = 0; i < vnum; ++i) {
            float x, y, z;  // NOSONAR
            if (!(file >> x >> y >> z)) {
                fprintf(stderr, "Error: Failed to read vertex data at index %u\n", i);
                exit(1);
            }
            vertices.emplace_back(x, y, z);
            vertices.emplace_back(1.0f, 1.0f, 1.0f);  // dummy color attribute
        }

        // Read faces
        for (unsigned int i = 0; i < fnum; ++i) {
            unsigned int vcount;

            if (!(file >> vcount)) {
                fprintf(stderr, "Error: Failed to read face count at face %u\n", i);
                exit(1);
            }

            if (vcount == 3) {
                glm::uvec3 triangle;

                if (!(file >> triangle[0] >> triangle[1] >> triangle[2])) {
                    fprintf(stderr, "Error: Failed to read triangle indices at face %u\n", i);
                    exit(1);
                }
                indices.push_back(triangle);
            } else {
                fprintf(stderr, "Error: Face %u is not a triangle\n", i);
                exit(1);
            }
        }

        file.close();
    }
};

#endif
