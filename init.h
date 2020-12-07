#ifndef INIT_H
#define INIT_H

#include<GL/glew.h>
#include<GL/freeglut.h>

//для чтения из файлов с шейдерами
#include<iostream>
#include<fstream>
#include<string>

using namespace std;

GLuint program;
void init() {

	setlocale(LC_ALL, "rus");
	ifstream fv("scene.vert");
	if (!fv.is_open())
		throw exception("Не найден вершинный шейдер");
	ifstream ff("scene.frag");
	if (!ff.is_open())
		throw exception("Не найден шрагментный шейдер");

	//Кусок для скрепления шейдеров и нашей программы

	string vsh_src((istreambuf_iterator<char>(fv)), istreambuf_iterator<char>());
	string fsh_src((istreambuf_iterator<char>(ff)), istreambuf_iterator<char>());

	//Создаём объекты программа и шейдеры
	program = glCreateProgram();
	GLenum vertex_shader = glCreateShader(GL_VERTEX_SHADER_ARB);
	GLenum fragment_shader = glCreateShader(GL_FRAGMENT_SHADER_ARB);

	//Загружаем тексты шейдеров в шейдеры
	const char* src = vsh_src.c_str();
	glShaderSource(vertex_shader, 1, &src, NULL);
	src = fsh_src.c_str();
	glShaderSource(fragment_shader, 1, &src, NULL);

	//Компилим шейдеры
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	//Прикрепляем шейдеры к программе
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	//Линкуем программу
	glLinkProgram(program);

	//Печатаем лог получившейся программы
	char log[10000];
	int log_len;
	glGetProgramInfoLog(program, sizeof(log) / sizeof(log[0]) - 1, &log_len, log);
	log[log_len] = 0;
	cout << "Shader compile result: " << log << endl;
}

#endif