#include "Quad2D.hpp"

Quad2D::Quad2D() {
    Setup();
}

Quad2D::~Quad2D() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Quad2D::Setup() {
    float vertices[] = {
        // Pozycje (X, Y, Z)    // Tekstury (U, V)
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // Prawy górny
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // Prawy dolny
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // Lewy dolny
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f  // Lewy górny
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Quad2D::Draw(Shader& shader, Texture& texture) {
    shader.use();
    texture.Bind();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}