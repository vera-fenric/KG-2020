#ifndef LAMP_H
#define LAMP_H

#include<GL/glew.h>
#include<GL/freeglut.h>
#include<glm/glm.hpp>	//для матриц
#include<glm/gtx/transform.hpp>	//для преобразований над матрицами

#include <string>
#include <vector>
using namespace std;

class Lamp {
private:
	GLuint VBO;
public:
	GLuint VAO;
	glm::vec3 color;
	glm::vec3 pos;

	Lamp(glm::vec3 c=glm::vec3(1.0f)): color(c), pos(0)
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &pos, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
	void Draw()
	{
		glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (GLuint *)0);
	}
};

#endif