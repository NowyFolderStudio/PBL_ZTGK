#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 OutlineColor;
layout (location = 3) out vec4 OutNormal;
layout (location = 4) out vec4 OutOutlineParams;

in vec2 TexCoord;
in vec3 Normal;

uniform float u_Time;

// Możesz zamienić je na uniformy, jeśli chcesz sterować z poziomu C++
float u_NoiseScale = 8.0;   // Skala wzoru (zwiększ, jeśli wzór jest za duży na modelu)
float u_AnimSpeed = 0.1;    // Prędkość falowania
float u_LineDensity = 10.0; // Ilość i gęstość izolinii

// --- Funkcje pomocnicze dla szumu Perlin'a ---

float hash3(vec3 p) {
    return fract(sin(1e3 * dot(p, vec3(1.0, 57.0, -13.7))) * 4375.5453);
}

float noise3(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    
    // Smoothstep dla płynnych pochodnych
    f = f * f * (3.0 - 2.0 * f);
    
    // Trójliniowa interpolacja
    return mix(mix(mix(hash3(p + vec3(0,0,0)), hash3(p + vec3(1,0,0)), f.x),
                   mix(hash3(p + vec3(0,1,0)), hash3(p + vec3(1,1,0)), f.x), f.y),
               mix(mix(hash3(p + vec3(0,0,1)), hash3(p + vec3(1,0,1)), f.x),
                   mix(hash3(p + vec3(0,1,1)), hash3(p + vec3(1,1,1)), f.x), f.y), f.z);
}

float noise(vec3 x) {
    // Poprawka pseudo-perlin (zwiększa jakość szumu)
    return (noise3(x) + noise3(x + 11.5)) * 0.5;
}

// --- Główna pętla ---

void main() {
    // 1. Obliczanie bazy szumu w przestrzeni UV + animacja w osi Z za pomocą czasu
    vec3 noisePos = vec3(TexCoord * u_NoiseScale, u_Time * u_AnimSpeed);
    float n = noise(noisePos);
    
    // 2. Matematyka fal (izolinii)
    float v = sin(6.283185 * u_LineDensity * n);
    
    // Wygładzanie krawędzi (antyaliasing) przy użyciu pochodnych
    v = smoothstep(1.0, 0.0, 0.5 * abs(v) / fwidth(v));
    
    // 3. Generowanie tęczowych kolorów na podstawie szumu (z wektorami przesunięcia fazy)
    vec3 rainbowColor = 0.5 + 0.5 * sin(12.0 * n + vec3(0.0, 2.1, -2.1));
    
    // 4. Mieszanie kolorów
    // Ponieważ usunęliśmy bufor historii (iChannel0), tło będzie ciemne, a izolinie kolorowe.
    vec3 bgColor = vec3(0.02); // Ciemnoszare tło
    vec3 finalColor = mix(bgColor, rainbowColor, v);

    // 5. Zapis do G-Bufferów Twojego silnika
    FragColor = vec4(finalColor, 1.0);
    
    // TIP: Skoro masz bufor BrightColor, możesz sprawić, że te linie będą świecić (Bloom)!
    // Przekazujemy kolor tylko tam, gdzie są wyrysowane linie (v > 0)
    BrightColor = vec4(rainbowColor * v, 1.0); 
    
    OutlineColor = vec4(0.0, 0.0, 0.0, 1.0);
    OutNormal = vec4(normalize(Normal), 1.0);
    OutOutlineParams = vec4(0.08, 1.1, 0.30, 1.0);
}