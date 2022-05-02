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
	The offset for textures. If the texture ID inside the vertex array
	is 0, this vertex isn't supposed to get its color from a texture.
	*/
	float m_TexIDOffset = 1.0f;
};