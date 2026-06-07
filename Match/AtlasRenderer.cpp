#include "AtlasRenderer.h"

SpriteRenderer::SpriteRenderer()
{
	const char* vertexSource = "#version 330 core\n"
		"layout(location = 0) in vec2 aPos;\n"
		"layout(location = 1) in vec2 aTexCoords;\n"

		"out vec2 TexCoords;\n"

		"uniform mat4 model;\n"
		"uniform mat4 projection;\n"

		"void main()\n"
		"{\n"
		"gl_Position = projection * model * vec4(aPos, 0.0, 1.0);\n"
		"TexCoords = aTexCoords;\n"
		"}\n";

	const char* fragmentSource = "#version 330 core\n"
		"out vec4 FragColor;\n"

		"in vec2 TexCoords;\n"
		"uniform sampler2D texture2D;\n"

		"void main()\n"
		"{\n"
		"FragColor = texture(texture2D, TexCoords);\n"
		"}\n";


	AtlasShader = glCreateProgram();

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

	glAttachShader(AtlasShader, vertexShader);
	glAttachShader(AtlasShader, fragmentShader);
	glLinkProgram(AtlasShader);

	glGetProgramiv(AtlasShader, GL_LINK_STATUS, &success);


	if (!success) {
		glGetShaderInfoLog(AtlasShader, 512, NULL, log);
		printf("Shader Program:%s\n", log);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	matrixSet = false;
	InitVBO();
	textureName = "";
	current = NULL;
}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteBuffers(1, &texCoordinates);
	glDeleteVertexArrays(1, &AtlasVAO);
	glDeleteProgram(AtlasShader);
}

void SpriteRenderer::changeTileAtlas(TileAtlas* atlas)
{
	tileAtlas = atlas;
}

void SpriteRenderer::SetProjectionMat(glm::mat4& proj)
{
	glUseProgram(AtlasShader);
	glUniformMatrix4fv(glGetUniformLocation(AtlasShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
	matrixSet = true;
}

void SpriteRenderer::Animate(glm::vec2 position, glm::vec2 size, Sprite* animation, int duration, float rotate)
{

	if (!matrixSet)
	{
		printf("Texture matrix not set\n");
		return;
	}

	current = ResourceManager::GetTexture(animation->spritesheet);

	if (current == NULL) {
		printf("Texture: %s not found\n", animation->spritesheet.c_str());
		return;
	}

	int frame = 0;
	frame = (int(SDL_GetTicks() / duration) % animation->maxframe);

	glm::vec2 Position, normalizePosition, normalizeSize;

	normalizeSize = glm::vec2(animation->framewidth / animation->width, animation->frameheight / animation->height);

	if (animation->vertical) {
		Position = glm::vec2(0.0f, frame * animation->frameheight);
	}
	else {
		Position = glm::vec2(frame * animation->framewidth, 0.0f);
	}

	normalizePosition = glm::vec2(Position.x / animation->width, (animation->height - (animation->frameheight + Position.y)) / animation->height);

	float texCoords[] = {
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,

			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
	};

	texCoords[0] = normalizePosition.x;
	texCoords[1] = normalizePosition.y;

	texCoords[2] = normalizePosition.x + normalizeSize.x;
	texCoords[3] = normalizePosition.y + normalizeSize.y;

	texCoords[4] = normalizePosition.x;
	texCoords[5] = normalizePosition.y + normalizeSize.y;

	texCoords[6] = normalizePosition.x;
	texCoords[7] = normalizePosition.y;

	texCoords[8] = normalizePosition.x + normalizeSize.x;
	texCoords[9] = normalizePosition.y;

	texCoords[10] = normalizePosition.x + normalizeSize.x;
	texCoords[11] = normalizePosition.y + normalizeSize.y;


	glUseProgram(AtlasShader);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
	model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 1.0f, 0.0f)); // then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

	model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

	glUniformMatrix4fv(glGetUniformLocation(AtlasShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, current->id);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordinates);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texCoords), texCoords);

	glBindVertexArray(AtlasVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::Tiled(glm::vec2 position, glm::vec2 size, std::string Id, float rotate, glm::vec3 axis)
{
	if (!matrixSet)
	{
		printf("Texture matrix not set\n");
		return;
	}

	if (tileAtlas == NULL) {
		return;
	}

	Quad* q = tileAtlas->GetOffset(Id);
	

	float texCoords[] = {
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,

			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
	};

	texCoords[0] = q->Up_left.x;
	texCoords[1] = q->Up_left.y;

	texCoords[2] = q->Down_right.x;
	texCoords[3] = q->Down_right.y;

	texCoords[4] = q->Down_left.x;
	texCoords[5] = q->Down_left.y;

	texCoords[6] = q->Up_left.x;
	texCoords[7] = q->Up_left.y;

	texCoords[8] = q->Up_right.x;
	texCoords[9] = q->Up_right.y;

	texCoords[10] = q->Down_right.x;
	texCoords[11] = q->Down_right.y;


	glUseProgram(AtlasShader);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
	model = glm::rotate(model, glm::radians(rotate), axis); // then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

	model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

	glUniformMatrix4fv(glGetUniformLocation(AtlasShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tileAtlas->texture->id);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordinates);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texCoords), texCoords);

	glBindVertexArray(AtlasVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::Tiled(glm::vec2 position, glm::vec2 size, std::string Id, std::vector<std::pair<float, glm::vec3>> rotation)
{
	if (!matrixSet)
	{
		printf("Texture matrix not set\n");
		return;
	}

	if (tileAtlas == NULL) {
		return;
	}

	Quad* q = tileAtlas->GetOffset(Id);

	

	float texCoords[] = {
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,

			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
	};

	texCoords[0] = q->Up_left.x;
	texCoords[1] = q->Up_left.y;

	texCoords[2] = q->Down_right.x;
	texCoords[3] = q->Down_right.y;

	texCoords[4] = q->Down_left.x;
	texCoords[5] = q->Down_left.y;

	texCoords[6] = q->Up_left.x;
	texCoords[7] = q->Up_left.y;

	texCoords[8] = q->Up_right.x;
	texCoords[9] = q->Up_right.y;

	texCoords[10] = q->Down_right.x;
	texCoords[11] = q->Down_right.y;


	glUseProgram(AtlasShader);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad

	if (!rotation.empty()) {
		for (auto& value : rotation) {
			model = glm::rotate(model, glm::radians(value.first), value.second); // then rotate
		}
	}
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

	model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

	glUniformMatrix4fv(glGetUniformLocation(AtlasShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tileAtlas->texture->id);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordinates);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texCoords), texCoords);

	glBindVertexArray(AtlasVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}



void SpriteRenderer::InitVBO()
{
	float Atlasvertices[] = {
		// pos      // tex
	   0.0f, 1.0f, 
	   1.0f, 0.0f, 
	   0.0f, 0.0f, 

	   0.0f, 1.0f, 
	   1.0f, 1.0f, 
	   1.0f, 0.0f
	};

	float texCoords[] = {
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,

			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
	};

	unsigned int AtlasVBO;

	glGenBuffers(1, &AtlasVBO);
	glGenBuffers(1, &texCoordinates);

	glGenVertexArrays(1, &this->AtlasVAO);

	glBindBuffer(GL_ARRAY_BUFFER, AtlasVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Atlasvertices), Atlasvertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordinates);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_DYNAMIC_DRAW);

	glBindVertexArray(this->AtlasVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,AtlasVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordinates);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}
