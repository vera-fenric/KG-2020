#ifndef Q_H
#define Q_H
#include<GL/glew.h>
#include<GL/freeglut.h>

using namespace std;

class Q {
public:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	GLfloat vertices[12] = {
	 0.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,
	 1.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f
	};
	GLuint indices[6] =
	{
		0, 1, 2,
		2, 3, 0
	};

	void Gen() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//����������� VAO
		glBindVertexArray(VAO);
		//����������� VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//������ � ������ ���� �������
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//������ � ������ ���� �����, ����� DrawElements ���� ����� ������
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLint), &indices[0], GL_STATIC_DRAW);

		//��������� �������, ��� � ��� ����� ��������� ������ �����
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		//���������� VAO, ����� ������ �� ��������� (�� ����� ���� �� �����������, ������ "������� ���")
		glBindVertexArray(0);

	};
	void Draw(GLuint program) {
		//������ ���
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	};


};
#endif