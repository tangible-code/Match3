#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

struct Vertex
{
	glm::vec2 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TextureId;
};

struct ParticleProp
{
	glm::vec2 Position;
	glm::vec2 Velocity, VelocityVariation;

	float SizeBegin, SizeEnd, SizeVartiation;

	glm::vec4 ColorBegin, ColorEnd;

	double Lifetime = 1.0;

	uint32_t TextureId = 0;
};

struct Particle
{
	glm::vec2 Position;
	glm::vec2 Velocity;

	glm::vec4 BeginColor, EndColor;
	float Rotation = 0.0f;
	float BeginSize, EndSize;

	double Lifetime = 1.0;
	double LifeRemaining = 0.0;

	uint32_t TextureId;

	bool Active = false;
};


class BatchRenderer
{
public:
	static void Init();

	static void BeginBatch();
	static void EndBatch();
	static void Flush();

	static void Update(double deltams);

	static void Destroy();

	static void SetProjectionMat(glm::mat4& proj);

	static void DrawTexture(glm::vec2 pPosition, glm::vec2 pSize, uint32_t pTexture, glm::vec4 pColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawQuad(glm::vec2 pPosition, glm::vec2 pSize, glm::vec4 pColor);

	static void Emit(ParticleProp& pProp);

private:
	static int m_MaxQuadCount;
	static int m_MaxVertices;
	static int m_MaxIndex;
	static int m_MaxTextureUnit;

	static int m_IndexCount;
	static int m_TextureUnit;

	static std::vector<Vertex> m_Vertex;
	static std::vector<uint32_t> m_Textures;

	static uint32_t VAO;
	static uint32_t VBO;
	static uint32_t EBO;
	static uint32_t Shader;

	static Vertex* m_VerticesPtr;

	static uint32_t m_WhiteTexture;

	static std::vector<Particle> m_Particles;
	static size_t m_PoolIndex;
};

