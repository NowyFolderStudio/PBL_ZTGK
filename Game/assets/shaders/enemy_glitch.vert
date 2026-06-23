#version 330 core

// Standardowe wejścia wierzchołków
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Wyjścia do Twojego Fragment Shadera
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

// Macierze z silnika
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Zmienne sterujące
uniform float u_Time;
uniform float u_Aggro; // Wartość od 0.0 (spokojny) do 1.0 (wściekły pościg)

void main() {
    TexCoord = aTexCoords;
    
    // Obliczanie normalnych (dla oświetlenia i kierunku wypychania)
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // --- Parametry bulgotania ---
    // Szybkość pulsowania (rośnie, gdy u_Aggro jest wyższe)
    float speed = 3.0 + (u_Aggro * 15.0); 
    
    // Rozmiar fal na blobie (jak gęste są bąble)
    float frequency = 4.0; 
    
    // Jak mocno wierzchołki są wypychane (glitch rośnie przy ataku)
    float amplitude = 0.15 + (u_Aggro * 0.3);

    // --- Matematyka zniekształcenia ---
    // Kombinacja sinusów w trzech osiach (X, Y, Z) daje świetny, losowy efekt bloba
    float displacement = sin(aPos.x * frequency + u_Time * speed) *
                         sin(aPos.y * frequency + u_Time * speed) *
                         sin(aPos.z * frequency + u_Time * speed);

    // Wypychamy wierzchołek wzdłuż jego normalnej
    vec3 displacedPos = aPos + (aNormal * displacement * amplitude);

    // Przekazanie zniekształconej pozycji w świat i na ekran
    FragPos = vec3(model * vec4(displacedPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}