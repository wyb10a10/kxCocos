#include "SimpleShader.h"
#include "ui/UIImageView.h"
#include "ui/UIScale9Sprite.h"
#include "ui/UITextBMFont.h"
#include "ui/UIButton.h"

USING_NS_CC;
using namespace ui;

static const char* HSV = "HSV";
static const char* HSVNoMVP = "HSVNoMVP";
static const char* HUE = "HUE";
static const char* HUENoMVP = "HUENoMVP";
static const char* SimpleGray = "SIMPLE_GRAY";

GLProgramState* CSimpleShader::applyHSVShader(Node* node, const Vec3& hsv)
{
    auto glProgram = ShaderCache::getInstance()->getGLProgram(HSV);
    if (glProgram == nullptr)
    {
        auto fileUtiles = FileUtils::getInstance();
#if USING_ETC1
		// Android平台使用ETC1
        auto fragmentFilePath = fileUtiles->fullPathForFilename("shaders/HSV_ETC1AS.frag");
#else
		auto fragmentFilePath = fileUtiles->fullPathForFilename("shaders/HSV.frag");
#endif
        auto fragSource = fileUtiles->getStringFromFile(fragmentFilePath);
        glProgram = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, fragSource.c_str());
        ShaderCache::getInstance()->addGLProgram(glProgram, HSV);
    }

    auto programState = GLProgramState::create(glProgram);
    programState->setUniformVec3("u_hsv", hsv);
    node->setGLProgramState(programState);

    return programState;
}

GLProgramState* CSimpleShader::applyHSVShaderNoMVP(Node* node, const Vec3& hsv)
{
    auto glProgram = ShaderCache::getInstance()->getGLProgram(HSVNoMVP);
    if (glProgram == nullptr)
    {
        auto fileUtiles = FileUtils::getInstance();
#if USING_ETC1
		// Android平台使用ETC1
		auto fragmentFilePath = fileUtiles->fullPathForFilename("shaders/HSV_ETC1AS.frag");
#else
		auto fragmentFilePath = fileUtiles->fullPathForFilename("shaders/HSV.frag");
#endif
        auto fragSource = fileUtiles->getStringFromFile(fragmentFilePath);
        glProgram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
        ShaderCache::getInstance()->addGLProgram(glProgram, HSVNoMVP);
    }

    auto programState = GLProgramState::create(glProgram);
    programState->setUniformVec3("u_hsv", hsv);
    node->setGLProgramState(programState);

	return programState;
}

GLProgramState* CSimpleShader::applyHueShader(Node* node, const float hue)
{
    auto glProgram = ShaderCache::getInstance()->getGLProgram(HUE);
    if (glProgram == nullptr)
    {
        auto fileUtiles = FileUtils::getInstance();
#if USING_ETC1
		// Android平台使用ETC1
        auto fragmentFilePath = fileUtiles->fullPathForFilename("shaders/Hue_ETC1AS.frag");
#else
		auto fragmentFilePath = fileUtiles->fullPathForFilename("shaders/Hue.frag");
#endif
        auto fragSource = fileUtiles->getStringFromFile(fragmentFilePath);
        glProgram = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, fragSource.c_str());
        ShaderCache::getInstance()->addGLProgram(glProgram, HUE);
    }

    auto programState = GLProgramState::create(glProgram);
    programState->setUniformFloat("u_hue", hue);
    node->setGLProgramState(programState);

    return programState;
}


GLProgramState* CSimpleShader::applyHueShaderNoMVP(Node* node, const float hue)
{
    auto glProgram = ShaderCache::getInstance()->getGLProgram(HUENoMVP);
	if (glProgram == nullptr)
	{
		auto fileUtiles = FileUtils::getInstance();
#if USING_ETC1
		// Android平台使用ETC1
		auto fragmentFilePath = fileUtiles->fullPathForFilename("shaders/Hue_ETC1AS.frag");
#else
		auto fragmentFilePath = fileUtiles->fullPathForFilename("shaders/Hue.frag");
#endif
		auto fragSource = fileUtiles->getStringFromFile(fragmentFilePath);
		glProgram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
        ShaderCache::getInstance()->addGLProgram(glProgram, HUENoMVP);
	}

    auto programState = GLProgramState::create(glProgram);
    programState->setUniformFloat("u_hue", hue);
    node->setGLProgramState(programState);

	return programState;
}

void CSimpleShader::applyGray(Node* node)
{
    for (auto &child : node->getChildren())
    {
        applyGray(child);
    }

    Node *pGrayNode = nullptr;
    Sprite *pSprite = dynamic_cast<Sprite*>(node);
    if (nullptr == pSprite)
    {
        ImageView *pImageView = dynamic_cast<ImageView*>(node);
        if (pImageView)
        {
            auto imageViewNode = dynamic_cast<Scale9Sprite*>(pImageView->getVirtualRenderer());
            if (imageViewNode)
            {
                imageViewNode->setState(Scale9Sprite::State::GRAY);
                return;
            }
        }

        TextBMFont *pLabel = dynamic_cast<TextBMFont*>(node);
        if (pLabel)
        {
            pGrayNode = pLabel->getVirtualRenderer();
        }

		Button *pButton = dynamic_cast<Button*>(node);
		if (pButton)
		{
			applyGray(pButton->getRendererNormal());
			applyGray(pButton->getRendererClicked());
			applyGray(pButton->getRendererDisabled());
		}
    }
    else
    {
        pGrayNode = pSprite;
    }

    if (nullptr == pGrayNode)
    {
        return;
    }

    auto glProgram = ShaderCache::getInstance()->getGLProgram(SimpleGray);
    if (glProgram == nullptr)
    {
        auto fileUtiles = FileUtils::getInstance();
        auto vshFilePath = fileUtiles->fullPathForFilename("shaders/Gray.vsh");
#if USING_ETC1
		// Android平台使用ETC1
        auto fshFilePath = fileUtiles->fullPathForFilename("shaders/Gray_ETC1AS.fsh");
#else
		auto fshFilePath = fileUtiles->fullPathForFilename("shaders/Gray.fsh");
#endif
        glProgram = GLProgram::createWithFilenames(vshFilePath, fshFilePath);
        ShaderCache::getInstance()->addGLProgram(glProgram, SimpleGray);
    }

    pGrayNode->setGLProgram(glProgram);
}

void CSimpleShader::removeGray(Node* node)
{
    for (auto &child : node->getChildren())
    {
        removeGray(child);
    }

    Node *pGrayNode = nullptr;
    Sprite *pSprite = dynamic_cast<Sprite*>(node);
    if (nullptr == pSprite)
    {
        ImageView *pImageView = dynamic_cast<ImageView*>(node);
        if (pImageView)
        {
            auto imageViewNode = dynamic_cast<Scale9Sprite*>(pImageView->getVirtualRenderer());
            if (imageViewNode)
            {
                imageViewNode->setState(Scale9Sprite::State::NORMAL);
                return;
            }
        }

        TextBMFont *pLabel = dynamic_cast<TextBMFont*>(node);
        if (pLabel)
        {
            pGrayNode = pLabel->getVirtualRenderer();
        }

		Button *pButton = dynamic_cast<Button*>(node);
		if (pButton)
		{
			removeGray(pButton->getRendererNormal());
			removeGray(pButton->getRendererClicked());
			removeGray(pButton->getRendererDisabled());
		}
    }
    else
    {
        pGrayNode = pSprite;
    }

    if (pGrayNode != nullptr)
    {
#if USING_ETC1
		// Android平台使用ETC1
		std::string str = GLProgram::SHADER_NAME_ETC1AS_POSITION_TEXTURE_COLOR_NO_MVP;
#else
		std::string str = GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP;
#endif
        GLProgram * pProgram = ShaderCache::getInstance()->getGLProgram(str);
        pGrayNode->setGLProgram(pProgram);
    }
}