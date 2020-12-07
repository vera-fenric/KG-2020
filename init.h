#ifndef INIT_H
#define INIT_H

#include<GL/glew.h>
#include<GL/freeglut.h>

//��� ������ �� ������ � ���������
#include<iostream>
#include<fstream>
#include<string>

using namespace std;

GLuint program;
void init() {

	setlocale(LC_ALL, "rus");
	ifstream fv("scene.vert");
	if (!fv.is_open())
		throw exception("�� ������ ��������� ������");
	ifstream ff("scene.frag");
	if (!ff.is_open())
		throw exception("�� ������ ����������� ������");

	//����� ��� ���������� �������� � ����� ���������

	string vsh_src((istreambuf_iterator<char>(fv)), istreambuf_iterator<char>());
	string fsh_src((istreambuf_iterator<char>(ff)), istreambuf_iterator<char>());

	//������ ������� ��������� � �������
	program = glCreateProgram();
	GLenum vertex_shader = glCreateShader(GL_VERTEX_SHADER_ARB);
	GLenum fragment_shader = glCreateShader(GL_FRAGMENT_SHADER_ARB);

	//��������� ������ �������� � �������
	const char* src = vsh_src.c_str();
	glShaderSource(vertex_shader, 1, &src, NULL);
	src = fsh_src.c_str();
	glShaderSource(fragment_shader, 1, &src, NULL);

	//�������� �������
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	//����������� ������� � ���������
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	//������� ���������
	glLinkProgram(program);

	//�������� ��� ������������ ���������
	char log[10000];
	int log_len;
	glGetProgramInfoLog(program, sizeof(log) / sizeof(log[0]) - 1, &log_len, log);
	log[log_len] = 0;
	cout << "Shader compile result: " << log << endl;
}

#endif