#include "UI/UIRenderer.hpp"
#include "UI/UIComponents.hpp"
#include "Core/Quad2D.hpp"
#include "Core/Text.hpp"

namespace NFSEngine {
	struct UIRendererData {
		std::unique_ptr<Shader> UIShader;
		std::unique_ptr<Shader> UITextShader;
		std::unique_ptr<Quad2D> UIQuad;
		std::unique_ptr<Texture> DefaultWhiteTexture;
		std::unique_ptr<Text> DefaultFont;

		glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
	};

	static UIRendererData* s_Data = nullptr;

	void UIRenderer::Init() {
		s_Data = new UIRendererData();
		s_Data->UIShader = std::make_unique<Shader>("ui.vert","ui.frag");
		s_Data->UITextShader = std::make_unique<Shader>("text.vert", "text.frag");

		s_Data->UIQuad = std::make_unique<Quad2D>();

		s_Data->DefaultFont = std::make_unique<Text>("assets/fonts/Roboto-Regular.ttf", 48.0f);

		s_Data->DefaultWhiteTexture = std::make_unique<Texture>(1, 1);
		uint32_t whitePixel = 0xFFFFFFFF;
		s_Data->DefaultWhiteTexture->SetData(&whitePixel, sizeof(uint32_t));

	}

	void UIRenderer::Shutdown() {
		delete s_Data;
		s_Data = nullptr;
	}

	void UIRenderer::SetProjection(float windowWidth, float windowHeight) {
		if (!s_Data) {
			return;
		}

		s_Data->ProjectionMatrix = glm::ortho(0.0f, windowWidth, windowHeight, 0.0f, -100.0f, 100.0f);
	}

	void UIRenderer::DrawQuad(const RectTransform& transform, const ImageComponent& image) {
		if (!s_Data) {
			return;
		}

		s_Data->UIShader->use();
		s_Data->UIShader->setMat4("u_projection", s_Data->ProjectionMatrix);
		s_Data->UIShader->setMat4("u_model", transform.GetTransform());
		s_Data->UIShader->setVec4("u_color", image.Color);

		if (image.TexturePtr) {
			image.TexturePtr->Bind();
		}
		else {
			s_Data->DefaultWhiteTexture->Bind();
		}

		s_Data->UIQuad->Draw();
	}

	void UIRenderer::DrawText(const RectTransform& transform, const TextComponent& textComp) {
		if (!s_Data) return;

		Text* fontToUse = textComp.Font ? textComp.Font : s_Data->DefaultFont.get();
		if (!fontToUse) return;

		s_Data->UITextShader->use();
		s_Data->UITextShader->setMat4("projection", s_Data->ProjectionMatrix);
		s_Data->UITextShader->setVec3("textColor", glm::vec3(textComp.Color.r, textComp.Color.g, textComp.Color.b));

		float textWidth = fontToUse->GetTextWidth(textComp.TextString, textComp.Scale);
		float startX = transform.Position.x - textWidth * transform.Pivot.x;
		float startY = transform.Position.y + (fontToUse->GetTextWidth("H", textComp.Scale) * transform.Pivot.y); // potencjalnie do zmiany / nie w kazdym foncie H bedzie najwyzsze

		fontToUse->Draw(textComp.TextString, startX, startY, textComp.Scale);
	}
}