#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <glm/ext/vector_uint3.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Mesh {
   private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::uvec3> triangles;

   public:
    explicit Mesh(const std::string& filename) {
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

        vertices.reserve(vnum);
        normals.reserve(vnum);
        triangles.reserve(fnum);

        // Read vertices
        for (unsigned int i = 0; i < vnum; ++i) {
            float x, y, z;  // NOSONAR
            if (!(file >> x >> y >> z)) {
                std::cerr << "Error: Failed to read vertex data at index " << i << std::endl;
                exit(1);
            }
            vertices.emplace_back(x, y, z);
            normals.emplace_back(0.f, 0.f, 0.f);
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
                    std::cerr << "Error: Failed to read triangle indices at face " << i
                              << std::endl;
                    exit(1);
                }
                triangles.push_back(triangle);
            } else {
                std::cerr << "Error: Face " << i << " is not a triangle" << std::endl;
                exit(1);
            }
        }

        file.close();

        rescale();
        compute_normals();
    }

    void pack4gpu(std::vector<float>& points, std::vector<unsigned int>& indices) const {
        points = {};
        // fill up flat points
        for (unsigned int i = 0; i < vertices.size(); ++i) {
            // coords
            points.push_back(vertices[i].x);
            points.push_back(vertices[i].y);
            points.push_back(vertices[i].z);
            // normal
            points.push_back(normals[i].x);
            points.push_back(normals[i].y);
            points.push_back(normals[i].z);
        }

        indices = {};
        // fill up flat triangles
        for (auto t : triangles)
            for (unsigned i = 0; i < 3; i++) indices.push_back(t[i]);
    }

   private:
    void rescale() {
        if (vertices.empty()) return;

        // Find bounding box
        glm::vec3 min_bounds = vertices[0];
        glm::vec3 max_bounds = vertices[0];

        for (const auto& vertex : vertices) {
            // glm/glsl min and max work component-wise
            min_bounds = glm::min(min_bounds, vertex);
            max_bounds = glm::max(max_bounds, vertex);
        }

        // Calculate centers and extents along x, y, and z
        glm::vec3 centers = (min_bounds + max_bounds) * 0.5f;
        //// glm::vec3 extents = (max_bounds - min_bounds) * 0.5f;

        // Find the maximum extent to preserve proportions
        //// float max_extent = std::max ({extents.x, extents.y, extents.z});
        float max_extent = glm::distance(max_bounds, min_bounds) * 0.5f;

        // Normalize vertices: translate to center, then scale uniformly
        for (auto& vertex : vertices) {
            vertex = (vertex - centers) / max_extent;
        }
    }

    void compute_normals() {
        for (const auto& triangle : triangles) {
            const glm::vec3& v0 = vertices[triangle[0]];
            const glm::vec3& v1 = vertices[triangle[1]];
            const glm::vec3& v2 = vertices[triangle[2]];

            // Compute the normal of the triangle
            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 normal = glm::cross(edge1, edge2);

            // Accumulate the normal for each vertex
            normals[triangle[0]] += normal;
            normals[triangle[1]] += normal;
            normals[triangle[2]] += normal;
        }

        // Normalize the accumulated normals
        for (auto& normal : normals) {
            normal = glm::normalize(normal);
        }
    }
};
