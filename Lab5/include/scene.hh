#ifndef SCENE_HH
#define SCENE_HH

#include <iostream>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "mesh.hh"

class Scene {
   private:
    Mesh mesh;
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    GLint edge_loc;

   public:
    explicit Scene(GLuint shader_program, const std::string& mesh_file = "")
        : edge_loc(glGetUniformLocation(shader_program, "draw_edge")) {
        if (mesh_file.empty()) {
            load();
        } else {
            load(mesh_file);
        }
    }
    ~Scene() { clean(); }

    void setupBuffers() {
        // we want just one buffer, and we retrieve the name OpenGL assigns to it.
        glGenBuffers(1, &vbo);
        // bind it as the current VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // transfer data from CPU RAM to GPU RAM.
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3),
                     mesh.vertices.data(), GL_STATIC_DRAW);

        // we want just one buffer container, and we retrieve the name OpenGL assigns to it.
        glGenVertexArrays(1, &vao);
        // bind it as the current vao.
        glBindVertexArray(vao);

        // Attribute 0: position (x, y, z)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        // Attribute 1: color (r, g, b)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // element buffer object
        glGenBuffers(1, &ebo);
        // MUST be bound after the VAO's binding!
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(glm::uvec3),
                     mesh.indices.data(), GL_STATIC_DRAW);
    }

    void load() {
        mesh.filename = "";

        // coordinates in clip space! no transformation involved
        mesh.vertices = {
            glm::vec3(0.0, 0.5, 0.0),  // red/magenta mix (kind of...), top center
            glm::vec3(0.8, 0.0, 0.4),  //

            glm::vec3(0.5, -0.5, 0.3),  // green, bottom right, back
            glm::vec3(0.0, 0.8, 0.0),   //

            glm::vec3(-0.5, -0.5, 0.3),  // blue, bottom left, back
            glm::vec3(0.0, 0.0, 0.8),    //

            glm::vec3(0.5, -0.5, -0.3),  // cyan, bottom right, front
            glm::vec3(0.0, 0.8, 0.8),    //

            glm::vec3(-0.5, -0.5, -0.3),  // yellow, bottom left, front
            glm::vec3(0.8, 0.8, 0.0)      //
        };

        // 6 faces, 3 indices per face, CCW order
        mesh.indices = {
            glm::uvec3(0, 3, 4),  //
            glm::uvec3(0, 2, 1),  //
            glm::uvec3(0, 4, 2),  //
            glm::uvec3(0, 1, 3),  //
            glm::uvec3(1, 2, 3),  //
            glm::uvec3(4, 3, 2),  //
        };

        setupBuffers();

        std::cout << "Loaded default mesh with " << mesh.vertices.size() << " vertices and "
                  << mesh.indices.size() << " faces." << std::endl;
    }

    void load(const std::string& mesh_file) {
        mesh = Mesh(mesh_file);
        setupBuffers();

        std::cout << "Loaded " << mesh_file << " mesh with " << mesh.vertices.size()
                  << " vertices and " << mesh.indices.size() << " faces." << std::endl;
    }

    void clean() const {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void reload(const std::string& mesh_file) {
        clean();
        load(mesh_file);
    }

    void reload() {
        clean();
        load();
    }

    void draw() const {
        // clear the buffers
        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw the filled triangles
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUniform1i(edge_loc, 0);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size() * 3), GL_UNSIGNED_INT,
                       nullptr);

        glDisable(GL_POLYGON_OFFSET_FILL);

        // draw the edges
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform1i(edge_loc, 1);

        glLineWidth(2.0f);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size() * 3), GL_UNSIGNED_INT,
                       nullptr);
    }
};

#endif
