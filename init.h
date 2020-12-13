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

	GLint success;
	GLchar info_log[1000];
	int log_len;

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
	GLenum vertex_shader = glCreateShader(GL_VERTEX_SHADER/*_ARB*/);
	GLenum fragment_shader = glCreateShader(GL_FRAGMENT_SHADER/*_ARB*/);

	//Загружаем тексты шейдеров в шейдеры
	const char* src = vsh_src.c_str();
	glShaderSource(vertex_shader, 1, &src, NULL);
	src = fsh_src.c_str();
	glShaderSource(fragment_shader, 1, &src, NULL);

	//Компилим шейдеры
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	//проверяем, что всё скомпилилось (больше нужно для отладки)
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		cout << "Ошибка: вершинный шейдер не скомпилировался\n" << info_log << endl;
	};
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		cout << "Ошибка: фрагментный шейдер не скомпилировался\n" << info_log << endl;
	};

	//Прикрепляем шейдеры к программе
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	//Линкуем программу
	glLinkProgram(program);

	//Печатаем лог получившейся программы, если появились ошибки в сборке
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, sizeof(info_log) / sizeof(info_log[0]) - 1, &log_len, info_log);
		cout << "Ошибка: линковка программы\n" << info_log << endl;
	}

	//удаляем шейдеры за ненадобностью
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

#endif