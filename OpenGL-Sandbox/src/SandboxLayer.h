#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

class SandboxLayer : public GLCore::Layer
{
public:
	SandboxLayer();
	virtual ~SandboxLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:
	GLCore::Utils::Shader* m_Shader;
	GLCore::Utils::OrthographicCameraController m_CameraController;

	GLuint m_QuadVA = 0, m_QuadVB = 0, m_QuadIB = 0;

	GLuint m_IvessTex, m_HeartTex;

	/* 
	The offset for textures inside of the vertex array. If the texture ID
	inside the vertex array is 0.0f, this vertex is supposed to be colored.
	If it's >m_TexIDOffset, then the color should be retrieved from the
	specified texture.
	*/
	float m_TexIDOffset = 1.0f;
};