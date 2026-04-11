#include "UI/UIComponents.hpp"
#include "UI/UIObject.hpp"
#include "UI/Canvas.hpp"
#include "UI/UIRenderer.hpp"
#include "UI/ButtonLogic.hpp"
#include "UI/UIFactory.hpp"

namespace NFSEngine
{

namespace UI
{
    UIObject& Image(Canvas& canvas, const ImageParameters& params)
    {
        UIObject& obj = canvas.CreateUIObject();

        obj.Transform.Position = params.position;
        obj.Transform.Width = params.width;
        obj.Transform.Height = params.height;

        auto& imgComp = obj.AddComponent<ImageComponent>(params.color);
        imgComp.TexturePtr = params.texture;

        return obj;
    }

    UIObject& Button(Canvas& canvas, const ButtonParameters& params)
    {
        UIObject& obj = canvas.CreateUIObject();

        obj.Transform.Position = params.position;
        obj.Transform.Width = params.width;
        obj.Transform.Height = params.height;

        auto& imgComp = obj.AddComponent<ImageComponent>(params.color);
        imgComp.TexturePtr = params.texture;

        auto& logic = obj.AddComponent<ButtonLogic>();
        logic.OnClick = params.onClick;

        logic.SetColor(params.color);

        obj.AddComponent<TextComponent>(params.text, params.font, params.textColor, params.textScale);

        return obj;
    };

    UIObject& Label(Canvas& canvas, const LabelParameters& params)
    {
        UIObject& obj = canvas.CreateUIObject();

        obj.Transform.Position = params.position;

        obj.AddComponent<TextComponent>(params.text, params.font, params.color, params.scale);

        return obj;
    }

    UIObject& Checkbox(Canvas& canvas, const CheckboxParameters& params)
    {
        UIObject& obj = canvas.CreateUIObject();

        obj.Transform.Position = params.position;
        obj.Transform.Width = params.size;
        obj.Transform.Height = params.size;

        obj.AddComponent<ImageComponent>(params.color);

        auto& logic = obj.AddComponent<ButtonLogic>();

        ButtonLogic* logicPtr = &logic;

        logic.OnClick = [logicPtr, params, isChecked = false]() mutable
        {
            isChecked = !isChecked;

            if (isChecked)
            {
                logicPtr->SetColor(params.checkColor);
            }
            else
            {
                logicPtr->SetColor(params.color);
            }

            if (params.onToggle)
            {
                params.onToggle(isChecked);
            }
        };

        return obj;
    }
}
}