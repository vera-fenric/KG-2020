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

	GLint success;
	GLchar info_log[1000];
	int log_len;

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
	GLenum vertex_shader = glCreateShader(GL_VERTEX_SHADER/*_ARB*/);
	GLenum fragment_shader = glCreateShader(GL_FRAGMENT_SHADER/*_ARB*/);

	//��������� ������ �������� � �������
	const char* src = vsh_src.c_str();
	glShaderSource(vertex_shader, 1, &src, NULL);
	src = fsh_src.c_str();
	glShaderSource(fragment_shader, 1, &src, NULL);

	//�������� �������
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	//���������, ��� �� ������������ (������ ����� ��� �������)
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		cout << "������: ��������� ������ �� ���������������\n" << info_log << endl;
	};
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		cout << "������: ����������� ������ �� ���������������\n" << info_log << endl;
	};

	//����������� ������� � ���������
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	//������� ���������
	glLinkProgram(program);

	//�������� ��� ������������ ���������, ���� ��������� ������ � ������
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, sizeof(info_log) / sizeof(info_log[0]) - 1, &log_len, info_log);
		cout << "������: �������� ���������\n" << info_log << endl;
	}

	//������� ������� �� �������������
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

#endif