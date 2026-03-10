#include <glad/glad.h>   // GLAD ZAWSZE NA SAMEJ GÓRZE!
#include <GLFW/glfw3.h>
#include <iostream>

// Funkcja, która pozwala zamknąć okno klawiszem ESC
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // 1. Inicjalizacja GLFW
    glfwInit();
    // Mówimy GLFW, że używamy OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Tworzenie okna
    GLFWwindow* window = glfwCreateWindow(800, 600, "Echoes Engine - Core", nullptr, nullptr);
    if (!window) {
        std::cerr << "Nie udalo sie utworzyc okna GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 3. INICJALIZACJA GLAD (Musi być PO utworzeniu okna, a PRZED jakimkolwiek rysowaniem)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Nie udalo sie zainicjowac GLAD!" << std::endl;
        return -1;
    }

    // Ustawienie obszaru rysowania
    glViewport(0, 0, 800, 600);

    // 4. Główna pętla gry (Core Loop)
    while (!glfwWindowShouldClose(window)) {

        // Sprawdzanie klawiatury
        processInput(window);

        // Czyszczenie ekranu (Ustawiamy mroczny, nocny kolor z Twojej gry!)
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Wymiana buforów i eventy
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Sprzątanie po wyjściu z gry
    glfwTerminate();
    return 0;
}