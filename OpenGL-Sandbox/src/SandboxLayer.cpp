#include "SandboxLayer.h"

#include "stb_image/stb_image.h"

using namespace GLCore;
using namespace GLCore::Utils;

SandboxLayer::SandboxLayer()
	: m_CameraController(16.0f / 9.0f)
{

}

SandboxLayer::~SandboxLayer()
{

}

static GLuint LoadTexture(const std::string& path)
{
	int width, height, bytes;

	stbi_set_flip_vertically_on_load(1);
	auto* pixels = stbi_load(path.c_str(), &width, &height, &bytes, STBI_rgb_alpha);
	GLuint textureID;
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (pixels)
		stbi_image_free(pixels);

	return textureID;
}

void SandboxLayer::OnAttach()
{
	EnableGLDebugging();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/shaders/test.vert.glsl",
		"assets/shaders/test.frag.glsl"
	);

	glUseProgram(m_Shader->GetRendererID());
	auto location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
	int samplers[2] = {0, 1};
	glUniform1iv(location, 2, samplers);

	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	float vertices[] = {
		// Position			// Color				// TexPos	//TexID
		-1.5f, -0.5f, 0.0f, 0.6f, 0.6f, 0.8f, 1.0f, 0.0f, 0.0f,	 0.0f,
		-0.5f, -0.5f, 0.0f, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, 0.0f,	 0.0f,
		-0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f, 1.0f,	 0.0f,
		-1.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,	 0.0f,

		 0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.3f, 1.0f, 0.0f, 0.0f,	 1.0f,
		 1.5f, -0.5f, 0.0f, 0.5f, 0.0f, 0.3f, 1.0f, 1.0f, 0.0f,	 1.0f,
		 1.5f,  0.5f, 0.0f, 0.8f, 0.2f, 0.3f, 1.0f, 1.0f, 1.0f,	 1.0f,
		 0.5f,  0.5f, 0.0f, 0.8f, 0.6f, 0.3f, 1.0f, 0.0f, 1.0f,	 1.0f
	};

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	int offset = 0;
	// Define the position attribute (3 floats)
	glEnableVertexArrayAttrib(m_QuadVB, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)offset);
	offset += 3 * sizeof(float);
	// Define the color attribute (4 floats)
	glEnableVertexArrayAttrib(m_QuadVB, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)offset);
	offset += 4 * sizeof(float);
	// Define the texture position attribute (2 floats)
	glEnableVertexArrayAttrib(m_QuadVB, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)offset);
	offset += 2 * sizeof(float);
	// Define the texture ID (1 float)
	glEnableVertexArrayAttrib(m_QuadVB, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)offset);
	offset += 1 * sizeof(float);

	uint32_t indices[] = {
		0, 1, 2, 
		2, 3, 0,
		
		4, 5, 6,
		6, 7, 4
	};
	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	m_IvessTex = LoadTexture("assets/ivess_circle.png");
	m_HeartTex = LoadTexture("assets/heart.png");
}

void SandboxLayer::OnDetach()
{
	glDeleteVertexArrays(1, &m_QuadVA);
	glDeleteBuffers(1, &m_QuadVB);
	glDeleteBuffers(1, &m_QuadIB);
}

void SandboxLayer::OnEvent(Event& event)
{
	m_CameraController.OnEvent(event);

	EventDispatcher dispatcher(event);
}

static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
{
	int location = glGetUniformLocation(shader, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

static void SetUniformVec4(uint32_t shader, const char* name, const glm::vec4& vec)
{
	int location = glGetUniformLocation(shader, name);
	glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

static void SetUniformVec3(uint32_t shader, const char* name, const glm::vec3& vec)
{
	int location = glGetUniformLocation(shader, name);
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());
	glBindTextureUnit(0, m_IvessTex);
	glBindTextureUnit(1, m_HeartTex);
	
	SetUniformMat4(m_Shader->GetRendererID(), "u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
	SetUniformMat4(m_Shader->GetRendererID(), "u_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	
	glBindVertexArray(m_QuadVA);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
}

void SandboxLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");
	ImGui::End();
}