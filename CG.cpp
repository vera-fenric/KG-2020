#include<GL/glew.h>
#include<GL/freeglut.h>
#include<glm.hpp>	//для матриц
#include<gtx/transform.hpp>	//для преобразований над матрицами

//для нормального билда._________.
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glew32.lib")

//для того, чтобы объекты крутились с нормальной скоростью
#include<vector>
#include<chrono>
#include<numeric>

#include"CyberChan.h"	//отсюда берём модельку
#include"init.h" //в файле init собираем шейдеры

#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glew32.lib")

using namespace std;

//массив вершин шкафа (пока ручками)
/*
Vertex cb_vertices[] = {
	{ -0.5f, -0.8f, -0.5f },
	{ -0.5f, 0.8f, -0.5f },
	{ 0.5f, 0.8f, -0.5f },
	{ 0.5f, -0.8f, -0.5f },
	{ -0.5f, -0.8f, 0.5f },
	{ -0.5f, 0.8f, 0.5f },
	{ 0.5f, 0.8f, 0.5f },
	{ 0.5f, -0.8f, 0.5f },
	
	{ -0.45f, -0.75f, -0.45f },
	{ -0.45f, 0.75f, -0.45f },
	{ 0.45f, 0.75f, -0.45f },
	{ 0.45f, -0.75f, -0.45f },
	{ -0.45f, -0.75f, 0.5f },
	{ -0.45f, 0.75f, 0.5f },
	{ 0.45f, 0.75f, 0.5f },
	{ 0.45f, -0.75f, 0.5f },

	//передняя грань
	{ -0.5f, -0.75f, 0.5f },
	{ -0.5f, 0.75f, 0.5f },
	{ 0.5f, 0.75f, 0.5f },
	{ 0.5f, -0.75f, 0.5f }
};

//массив граней
GLuint cb_indices[] = {
	0,1,2,3,
	//4,5,6,7,
	0,1,5,4,
	1,2,6,5,
	2,3,7,6,
	3,0,4,7,

	8,9,10,11,
	//12,13,14,15,
	8,9,13,12,
	9,13,14,10,
	10,14,15,11,
	11,15,12,8,

	//переднюю грань состоит из 4-х кусков
	4,7,19,16,
	16,12,13,17,
	17,18,6,5,
	15,19,18,14
};*/

//для рисования
GLuint vertexBuffer;
GLuint vertexArray;
//GLuint program;

//матрица проецирования
glm::mat4x4 proj;

//для поворота шкафа
float cb_rot_angl = 0;	//угол поворота
bool cb_rot_right = true;	//поворот направо

//для того, чтобы поворот был гладкий
vector<float> times(32);	//динамический массив на 32 элемента
chrono::steady_clock::time_point prevFrame;	//время предыдущего фрейма
bool firstFrame;	//флаг первого врейма

void reshape(int w, int h)
{
	proj = glm::perspectiveFovRH(45.0f, float(w), float(h), 1.0f, 20.0f);	//fov, размер экрана, ближняя и дальняя плоскость отсечения
	glViewport(0, 0, w, h);

	//при растяжении экрана меняем очищаем массив и ставим флажок, чтобы не бегать по пустому массиву
	times.clear();
	firstFrame = true;
}

void display(void)
{
	glm::mat4x4 mvp = proj *
		glm::translate(glm::vec3(0.0f, -1.0f, -2.0f)) *
		glm::rotate(cb_rot_angl, glm::vec3(0.0f, 1.0f, 0.0f));	//считаем матрицу преобразования из координат куба в координаты зрителя translate - перенос, rotate - поворот
	//сначала применяется поворот, потом перенос, а потом проецирование

	glUseProgramObjectARB(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);//привязка идентификаторов шейдерной программы и нашей

	glClear(GL_COLOR_BUFFER_BIT);	//очищаем экран
	glDrawElements(GL_TRIANGLES, sizeof(CyberChan_indices) / sizeof(CyberChan_indices[0]), GL_UNSIGNED_INT, CyberChan_indices);	//рисуем(что, сколько, какого размера,откуда)
	glFlush();	//сброс буффера
}
void idle(void)
{
	if (firstFrame) {
		prevFrame = chrono::steady_clock::now(); //запоминаем время
		firstFrame = false;	//больше не первый фрейм
	}
	else {
		chrono::steady_clock::time_point currFrame = chrono::steady_clock::now();	//запоминаем текущее время
		double delta = double(chrono::duration_cast<std::chrono::microseconds>(currFrame - prevFrame).count());	//сколько микросекунд прошло
		prevFrame = currFrame;	//записываем текущее время в prevFrame
		if (times.size() >= 32)	//если в times слишком много элементов, выкидываем
			times.pop_back();
		times.push_back(delta);	//иначе записываем, сколько прошло

		double avg = accumulate(times.begin(), times.end(), 0.0) / times.size();	//считаем среднее арифметическое в times
		//cout << xrot;
		if (cb_rot_angl > 0.5)	//если повернули на 0,5рад вправо, то начинаем двигать влево
			cb_rot_right = false;
		if (cb_rot_angl < -0.5)		//если повернули на 0,5рад влево, начинаем двигать вправо
			cb_rot_right = true;
		if (cb_rot_right)
			cb_rot_angl += 0.0000005 * avg;	//двигаем, собсна (меняем угол поворота)
		else
			cb_rot_angl -= 0.0000005 * avg;
	}
	glutPostRedisplay();	//перерисовать текущее окно, вызывать display плохо, потому что может накопиться много idle, и мы застрянем
}

/*void pressKey(int key, int xx, int yy) {
	switch (key) {
	case GLUT_KEY_UP: deltaMove = 0.5f; break;
	case GLUT_KEY_DOWN: deltaMove = -0.5f; break;
	}
	glutPostRedisplay();
}*/

int main(int argc, char** argv)
{

		//подготовка
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGB);
		glutCreateWindow("My Scene");
		glewInit();

		//собираем шейдеры
		try {
			init();
		}
		catch (exception ex) {
			cout << ex.what();
			return 1;
		}

		glGenBuffers(1, &vertexBuffer);	//создаём буффер
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //биндим буффер
		glBufferData(GL_ARRAY_BUFFER, sizeof(CyberChan_vertices), CyberChan_vertices, GL_STATIC_DRAW);//указываем, какие данные будут в буффере

		glGenVertexArrays(1, &vertexArray);	//генерируем массив вершин
		glBindVertexArray(vertexArray);	//биндим его
		int cb_pos = glGetAttribLocation(program, "model_pos");	//получем номер model_pos из связанной с шейдером программы
		glVertexAttribPointer(cb_pos, 3, GL_FLOAT, GL_FALSE/*нормирован*/, 0, 0);	//указываем, что в model_pos хранится вектор из 3 значений тип float, ненормированный
		glEnableVertexAttribArray(cb_pos);//генерируем массив вершин для передачи в вершинный шейдер

		glClearColor(0.0, 0.0, 0.0, 0.0);	//задаём цвет очистки
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//задаём цвет рисования полигонов

		//задаём функции обработки событий (те, что написаны выше)
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutIdleFunc(idle);

		glutMainLoop();
	return 0;
	
}