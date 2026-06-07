#include "FontRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

FontRenderer::FontRenderer()
{
	const char* vertexSource = "#version 330 core\n"
	"layout(location = 0) in vec4 vertex;\n" // <vec2 pos, vec2 tex>
	"out vec2 TexCoords;\n"
	"uniform mat4 projection;\n"
	"void main()\n"
	"{\n"
		"gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
		"TexCoords = vertex.zw;\n"
	"}\n";

	const char* fragmentSource = "#version 330 core\n"
	"in vec2 TexCoords;\n"
	"out vec4 color;\n"
	"uniform sampler2D text;\n"
	"uniform vec3 textColor;\n"
	"void main()\n"
	"{\n"
		"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
		"color = vec4(textColor, 1.0) * sampled;\n"
	"}\n";

	fontShader = glCreateProgram();

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

	glAttachShader(fontShader, vertexShader);
	glAttachShader(fontShader, fragmentShader);
	glLinkProgram(fontShader);

	glGetProgramiv(fontShader, GL_LINK_STATUS, &success);


	if (!success) {
		glGetShaderInfoLog(fontShader, 512, NULL, log);
		printf("Shader Program:%s\n", log);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	matrixSet = false;
	Init();
}

FontRenderer::~FontRenderer()
{
	glDeleteProgram(fontShader);
	glDeleteBuffers(1,&fontVBO);
	glDeleteVertexArrays(1, &fontVAO);
}

void FontRenderer::RenderText(glm::vec2 position, std::string str, std::map<char, F_Character>* font, glm::vec3 color, float scale)
{
	if (!matrixSet)
	{
		printf("Font Renderer Matrix not set\n");
		return;
	}
	glUseProgram(fontShader);
	glUniform3f(glGetUniformLocation(fontShader, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->fontVAO);
	// iterate through all characters
	std::string::const_iterator c;
	for (c = str.begin(); c != str.end(); c++)
	{
		F_Character ch = (*font)[*c];
		float xpos = position.x + ch.bearing.x * scale;
		float ypos = position.y + ((*font)['H'].bearing.y - ch.bearing.y) * scale;
		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
		{ xpos, ypos + h, 0.0f, 1.0f },
		{ xpos + w, ypos, 1.0f, 0.0f },
		{ xpos, ypos, 0.0f, 0.0f },
		{ xpos, ypos + h, 0.0f, 1.0f },
		{ xpos + w, ypos + h, 1.0f, 1.0f },
		{ xpos + w, ypos, 1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.texture);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, this->fontVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// advance cursors for next glyph (advance is 1/64 pixels)
		position.x += (ch.advance >> 6) * scale; // bitshift by 6 (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

glm::vec2 FontRenderer::GetSize(std::string str, std::map<char, F_Character>* font, float scale)
{
	std::string::const_iterator c;
	float w = 0.0f;
	float h = 0.0f;
	for (c = str.begin(); c != str.end(); c++)
	{
		F_Character ch = (*font)[*c];
		float xpos = ch.bearing.x * scale;
		float ypos = ((*font)['H'].bearing.y - ch.bearing.y) *scale;
		w += (ch.size.x * scale) - xpos;
		h += (ch.size.y*scale) - ypos;
	}
	return glm::vec2(w,h);
}

void FontRenderer::SetProjectionMat(glm::mat4& proj)
{
	glUseProgram(fontShader);
	glUniformMatrix4fv(glGetUniformLocation(fontShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
	matrixSet = true;
}

void FontRenderer::Init()
{
	glGenVertexArrays(1, &this->fontVAO);
	glGenBuffers(1, &this->fontVBO);
	glBindVertexArray(this->fontVAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->fontVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
