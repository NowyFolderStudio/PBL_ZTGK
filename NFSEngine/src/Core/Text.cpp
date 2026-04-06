#include "Core/Text.hpp"
#include "Core/Shader.hpp"
#include <iostream>
#include <fstream>

namespace NFSEngine {

    Text::Text(const std::string& fontPath, float fontSize) {
		VAO = 0;
		VBO = 0;
		texture = 0;

        std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
        if (!fontFile.is_open()) {
            std::cerr << "ERROR::TEXT: Failed to load font: " << fontPath << std::endl;
            return;
        }
        std::streamsize size = fontFile.tellg();
        fontFile.seekg(0, std::ios::beg);
        std::vector<unsigned char> fontData(size);
        if (!fontFile.read((char*)fontData.data(), size)) {
            std::cerr << "ERROR::TEXT: Failed to read font data!" << std::endl;
            return;
        }
        
        std::vector<unsigned char> atlasData(atlasWidth * atlasHeight);
        charInfo.resize(charCount);
        
        stbtt_pack_context context;
        if (!stbtt_PackBegin(&context, atlasData.data(), atlasWidth, atlasHeight, 0, 1, nullptr)) {
            std::cerr << "ERROR::STB_TRUETYPE: Failed to initialize font packing!" << std::endl;
            return;
        }
        
        stbtt_PackSetOversampling(&context, 2, 2);
        stbtt_PackFontRange(&context, fontData.data(), 0, fontSize, firstChar, charCount, charInfo.data());
        stbtt_PackEnd(&context);
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlasData.data());
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4 * 100, NULL, GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    Text::~Text() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteTextures(1, &texture);
    }
    
    void Text::Draw(std::string text, float x, float y, float scale) {

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        std::vector<float> vertices;
        float cursorX = 0.0f;
        float cursorY = 0.0f;
        
        for (char c : text) {
            if (c < firstChar || c >= firstChar + charCount) continue;
            
            stbtt_aligned_quad quad;
            
            stbtt_GetPackedQuad(charInfo.data(), atlasWidth, atlasHeight, c - firstChar, &cursorX, &cursorY, &quad, 1);
            
            float x0 = x + (quad.x0 * scale);
            float y0 = y + (quad.y0 * scale);
            float x1 = x + (quad.x1 * scale);
            float y1 = y + (quad.y1 * scale);
            
            float charVertices[6][4] = {
                { x0, y1,   quad.s0, quad.t1 },
                { x0, y0,   quad.s0, quad.t0 },
                { x1, y0,   quad.s1, quad.t0 },
                
                { x0, y1,   quad.s0, quad.t1 },
                { x1, y0,   quad.s1, quad.t0 },
                { x1, y1,   quad.s1, quad.t1 }
            };
            
            for (int i = 0; i < 6; ++i) {
                vertices.insert(vertices.end(), charVertices[i], charVertices[i] + 4);
            }
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data()); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDrawArrays(GL_TRIANGLES, 0, text.length() * 6);
        glBindVertexArray(0);
    }


    float Text::GetTextWidth(std::string text, float scale) {
        float width = 0.0f;
        
        for (char c : text) {
            if (c < firstChar || c >= firstChar + charCount) continue;
            stbtt_packedchar& ch = charInfo[c - firstChar];
            width += ch.xadvance * scale;
        }
        
        return width;
	}
}