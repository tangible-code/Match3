#include "BatchRenderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

int BatchRenderer::m_MaxQuadCount = 100000;
int BatchRenderer::m_MaxVertices = BatchRenderer::m_MaxQuadCount * 4;
int BatchRenderer::m_MaxIndex = BatchRenderer::m_MaxQuadCount * 6;
int BatchRenderer::m_MaxTextureUnit = 16;

int BatchRenderer::m_IndexCount;
int BatchRenderer::m_TextureUnit;

std::vector<Vertex> BatchRenderer::m_Vertex;
std::vector<uint32_t> BatchRenderer::m_Textures;

uint32_t BatchRenderer::VAO;
uint32_t BatchRenderer::VBO;
uint32_t BatchRenderer::EBO;
uint32_t BatchRenderer::Shader;

Vertex* BatchRenderer::m_VerticesPtr;
uint32_t BatchRenderer::m_WhiteTexture;

std::vector<Particle> BatchRenderer::m_Particles;
size_t BatchRenderer::m_PoolIndex = 999;

void BatchRenderer::Init()
{

	const char* vertexSource = "#version 420 core\n"

		"layout(location = 0) in vec2 aPos;\n"
		"layout(location = 1) in vec4 aColor;\n"
		"layout(location = 2) in vec2 aTexCoord;\n"
		"layout(location = 3) in float aTextureIndex;\n"

		"out vec4 Color;\n"
		"out vec2 TexCoord;\n"
		"out float TextureIndex;\n"

		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"

		"void main()\n"
		"{\n"
		"gl_Position = projection * view * vec4(aPos.xy, 0.0, 1.0);\n"
		"TexCoord = aTexCoord;\n"
		"Color = aColor;\n"
		"TextureIndex = aTextureIndex;\n"
		"}\n";

	const char* fragmentSource = "#version 420 core\n"

		"out vec4 FragColor;\n"
		"in vec4 Color;\n"
		"in vec2 TexCoord;\n"
		"in float TextureIndex;\n"

		"uniform sampler2D textures[16];\n"

		"void main()\n"
		"{\n"
		"int Index = int(TextureIndex);\n"
		"FragColor = texture(textures[Index], TexCoord) * Color;\n"
		"}\n";

	Shader = glCreateProgram();

	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char log[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, log);
		printf("Vertex Shader:%s\n", log);
	}

	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, log);
		printf("Fragment Shader:%s\n", log);
	}

	glAttachShader(Shader, vertexShader);
	glAttachShader(Shader, fragmentShader);
	glLinkProgram(Shader);

	glGetProgramiv(Shader, GL_LINK_STATUS, &success);


	if (!success) {
		glGetShaderInfoLog(Shader, 512, NULL, log);
		printf("Shader Program:%s\n", log);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//


	m_Vertex.resize(m_MaxVertices);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_MaxVertices, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureId));



	std::vector<unsigned int> IndexBuffer;
	IndexBuffer.resize(m_MaxIndex);


	unsigned int offset = 0;

	for (unsigned int i = 0; i < IndexBuffer.size(); i += 6)
	{
		IndexBuffer[i] = 0 + offset;
		IndexBuffer[i + 1] = 1 + offset;
		IndexBuffer[i + 2] = 2 + offset;
		IndexBuffer[i + 3] = 0 + offset;
		IndexBuffer[i + 4] = 2 + offset;
		IndexBuffer[i + 5] = 3 + offset;

		offset += 4;
	}


	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * IndexBuffer.size(), &IndexBuffer[0], GL_STATIC_DRAW);



	//white Texture
	glGenTextures(1, &m_WhiteTexture);

	glBindTexture(GL_TEXTURE_2D, m_WhiteTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned color = 0xffffffff;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

	m_Textures.resize(m_MaxTextureUnit);

	m_Textures[0] = m_WhiteTexture;
	for (size_t i = 1; i < m_MaxTextureUnit; i++)
	{
		m_Textures[i] = 0;
	}

	m_TextureUnit = 1;

	m_VerticesPtr = m_Vertex.data();

	glUseProgram(Shader);
	GLint sample[16];

	for (uint32_t i = 0; i < 16; i++)
	{
		sample[i] = i;
	}
	glUniform1iv(glGetUniformLocation(Shader, "textures"), 16, sample);

	glUseProgram(0);

	m_Particles.resize(1000);
}

void BatchRenderer::BeginBatch()
{
	m_VerticesPtr = m_Vertex.data();
}

void BatchRenderer::EndBatch()
{
	for (Particle& particle : m_Particles)
	{
		if (!particle.Active)
			continue;
		float life = float(particle.LifeRemaining / particle.Lifetime);

		glm::vec4 Color = glm::lerp(particle.EndColor, particle.BeginColor, life);

		//Color.a = Color.a * life;

		float size = glm::lerp(particle.EndSize, particle.BeginSize, life);

		//rotation

		uint32_t TextureId = particle.TextureId;

		if (!TextureId)
			TextureId = m_WhiteTexture;

		DrawTexture(particle.Position, glm::vec2(size, size), TextureId, Color);
	}

	GLsizeiptr size = m_VerticesPtr - m_Vertex.data();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * size, m_Vertex.data());
}

void BatchRenderer::Flush()
{
	glUseProgram(Shader);

	glm::mat4 view = glm::mat4(1.0f);
	glUniformMatrix4fv(glad_glGetUniformLocation(Shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	
	for (size_t i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
	m_IndexCount = 0;
	
	glUseProgram(0);
}

void BatchRenderer::Destroy()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &m_WhiteTexture);
	glDeleteProgram(Shader);
}

void BatchRenderer::SetProjectionMat(glm::mat4& proj)
{
	glUseProgram(Shader);
	glUniformMatrix4fv(glad_glGetUniformLocation(Shader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
}

void BatchRenderer::DrawTexture(glm::vec2 pPosition, glm::vec2 pSize, uint32_t pTexture, glm::vec4 pColor)
{
	if (m_IndexCount >= m_MaxIndex || m_TextureUnit > m_MaxTextureUnit)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	float TextureIndex = 0.0f;

	for (size_t i = 1; i < m_Textures.size(); i++)
	{
		if (pTexture == m_Textures[i])
		{
			TextureIndex = float(i);
			break;
		}
	}

	if (TextureIndex == 0.0f)
	{
		TextureIndex = (float)m_TextureUnit;
		m_Textures[m_TextureUnit] = pTexture;
		m_TextureUnit++;

	}

	m_VerticesPtr->Position = glm::vec2(0.0f + pPosition.x, 1.0f * pPosition.y + pSize.y);
	m_VerticesPtr->Color = pColor;
	m_VerticesPtr->TexCoord = glm::vec2(0.0f, 0.0f);
	m_VerticesPtr->TextureId = TextureIndex;

	m_VerticesPtr++;

	m_VerticesPtr->Position = glm::vec2(1.0f * pPosition.x + pSize.x, 1.0f * pPosition.y + pSize.y);
	m_VerticesPtr->Color = pColor;
	m_VerticesPtr->TexCoord = glm::vec2(1.0f, 0.0f);
	m_VerticesPtr->TextureId = TextureIndex;

	m_VerticesPtr++;

	m_VerticesPtr->Position = glm::vec2(1.0f * pPosition.x + pSize.x, 0.0f + pPosition.y);
	m_VerticesPtr->Color = pColor;
	m_VerticesPtr->TexCoord = glm::vec2(1.0f, 1.0f);
	m_VerticesPtr->TextureId = TextureIndex;

	m_VerticesPtr++;

	m_VerticesPtr->Position = glm::vec2(0.0f + pPosition.x, 0.0f + pPosition.y);
	m_VerticesPtr->Color = pColor;
	m_VerticesPtr->TexCoord = glm::vec2(0.0f, 1.0f);
	m_VerticesPtr->TextureId = TextureIndex;

	m_VerticesPtr++;

	m_IndexCount += 6;
}

void BatchRenderer::DrawQuad(glm::vec2 pPosition, glm::vec2 pSize, glm::vec4 pColor)
{
	if (m_IndexCount >= m_MaxIndex)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	float TextureIndex = 0.0f;

	m_VerticesPtr->Position = glm::vec2(0.0f + pPosition.x, 1.0f * pPosition.y + pSize.y);
	m_VerticesPtr->Color = pColor;
	m_VerticesPtr->TexCoord = glm::vec2(0.0f, 0.0f);
	m_VerticesPtr->TextureId = TextureIndex;

	m_VerticesPtr++;

	m_VerticesPtr->Position = glm::vec2(1.0f * pPosition.x + pSize.x, 1.0f * pPosition.y + pSize.y);
	m_VerticesPtr->Color = pColor;
	m_VerticesPtr->TexCoord = glm::vec2(1.0f, 0.0f);
	m_VerticesPtr->TextureId = TextureIndex;

	m_VerticesPtr++;

	m_VerticesPtr->Position = glm::vec2(1.0f * pPosition.x + pSize.x, 0.0f + pPosition.y);
	m_VerticesPtr->Color = pColor;
	m_VerticesPtr->TexCoord = glm::vec2(1.0f, 1.0f);
	m_VerticesPtr->TextureId = TextureIndex;

	m_VerticesPtr++;

	m_VerticesPtr->Position = glm::vec2(0.0f + pPosition.x, 0.0f + pPosition.y);
	m_VerticesPtr->Color = pColor;
	m_VerticesPtr->TexCoord = glm::vec2(0.0f, 1.0f);
	m_VerticesPtr->TextureId = TextureIndex;

	m_VerticesPtr++;

	m_IndexCount += 6;
}

void BatchRenderer::Update(double deltams)
{
	for (size_t i = 0; i < m_Particles.size(); i++)
	{
		if (!m_Particles[i].Active)
		{
			continue;
		}

		if (m_Particles[i].LifeRemaining <= 0.0)
		{
			m_Particles[i].Active = false;
			m_Particles[i].LifeRemaining = 0.0;
			continue;
		}

		m_Particles[i].LifeRemaining -= (1.0 / deltams);
		m_Particles[i].Position += m_Particles[i].Velocity * float(1.0 / deltams);
		m_Particles[i].Rotation += 0.01f * float(1.0 / deltams);

	}
}

void BatchRenderer::Emit(ParticleProp& pProp)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<std::mt19937::result_type> distribution;
	float limit = (float)std::numeric_limits<uint32_t>::max();

	Particle& particle = m_Particles[m_PoolIndex];
	particle.Active = true;
	particle.Position = pProp.Position;
	particle.Rotation = (float(distribution(mt)) / limit) * 2.0f * glm::pi<float>();

	particle.Velocity = pProp.Velocity;
	particle.Velocity.x += pProp.VelocityVariation.x * ((float(distribution(mt)) / limit) - 0.5f);
	particle.Velocity.y += pProp.VelocityVariation.y * ((float(distribution(mt)) / limit) - 0.5f);

	particle.BeginColor = pProp.ColorBegin;
	particle.EndColor = pProp.ColorEnd;

	particle.Lifetime = pProp.Lifetime;
	particle.LifeRemaining = pProp.Lifetime;
	particle.BeginSize = pProp.SizeBegin + pProp.SizeVartiation * ((float(distribution(mt)) / limit) - 0.5f);
	particle.EndSize = pProp.SizeEnd;

	particle.TextureId = pProp.TextureId;

	m_PoolIndex = --m_PoolIndex % m_Particles.size();
}
