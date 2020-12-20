#include<GL/glew.h>
#include<GL/freeglut.h>
#include<glm/glm.hpp>	//для матриц
#include<glm/gtx/transform.hpp>	//для преобразований над матрицами

//для того, чтобы объекты крутились с нормальной скоростью
#include<vector>
#include<chrono>
#include<numeric>

#include<mesh.h>	//тип вершины и меши
#include<model.h>	//тип модели
#include<program.h>	//компилятор шейдеров
#include<Scene.h>
#include<post_effect_scene.h>

using namespace std;

//матрица проецирования
glm::mat4x4 projection;

//для поворотов
float cb_rot_angl_y = 0;
float g_rot_angl = 0;

//Сцена - для рисования сцены, q - для рисования прямоугольника в постэффекте, IDs - для хранения программ с разными шейдерами
Scene scene;
Q q;
vector<GLuint> IDs;

//для того, чтобы поворот был гладкий
vector<float> times(10);	//динамический массив на 10 элементов
chrono::steady_clock::time_point prevFrame;	//время предыдущего фрейма
bool firstFrame;	//флаг первого врейма

//для фреймбуферов
const unsigned int TEXTURE_WIDTH = 1024, TEXTURE_HEIGHT = 1024;
GLuint FBO_dm;
GLuint FBO_pe;
GLuint depthMap;
GLuint PostEffect;

//положение лампы и её матрицы
glm::vec3 Lamp = glm::vec3(5.0f, 5.0f, 3.0f);
glm::mat4 light_projection, light_view;

//ближняя и дальняя плоскость отсечения
float np = 1.0f, fp = 30.0f;

//размер окна для сохранения
int window_width, window_height;

//для постэффекта
GLuint PE_bool;

void reshape(int w, int h)
{
	window_width = w;
	window_height = h;
	glViewport(0, 0, w, h);
	if (h > 0){
		projection = glm::perspectiveFovRH(45.0f, float(w), float(h), np, fp);	//fov, размер экрана, ближняя и дальняя плоскость отсечения
		light_projection = glm::perspectiveFovRH(45.0f, float(TEXTURE_WIDTH), float(TEXTURE_HEIGHT), np, fp);
	}
	//при растяжении экрана меняем очищаем массив и ставим флажок, чтобы не бегать по пустому массиву
	times.clear();
	firstFrame = true;
}

void display()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	light_view = glm::lookAt(Lamp, glm::vec3(0.0f, -1.25f, -3.0f), glm::vec3(0.0, 1.0, 0.0));
	glUseProgram(IDs[1]);	//используем шейдеры теней
	glUniformMatrix4fv(glGetUniformLocation(IDs[1], "proj"), 1, GL_FALSE, &light_projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(IDs[1], "view"), 1, GL_FALSE, &light_view[0][0]);//привязка идентификаторов шейдерной программы и нашей

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_dm);
		glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		scene.render1(IDs[1], cb_rot_angl_y, g_rot_angl);
		glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, FBO_pe);
		glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4x4 view = glm::mat4(1.0f);
		glUseProgram(IDs[0]);
		glUniform3fv(glGetUniformLocation(IDs[0], "LightPosition"), 1, &Lamp[0]);
		glUniformMatrix4fv(glGetUniformLocation(IDs[0], "proj"), 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(IDs[0], "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(IDs[0], "light_proj"), 1, GL_FALSE, &light_projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(IDs[0], "light_view"), 1, GL_FALSE, &light_view[0][0]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(glGetUniformLocation(IDs[0], "shadowMap"), depthMap);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, PostEffect);
		scene.render1(IDs[0], cb_rot_angl_y, g_rot_angl);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//ТРЕТИЙ ПРОХОД---------------------------------------
		///*
	glViewport(0, 0, window_width, window_height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
		glUseProgram(IDs[2]);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, PostEffect);
		glUniform1i(glGetUniformLocation(IDs[2], "screenTexture"), PostEffect);
		glUniform1i(glGetUniformLocation(IDs[2], "Effect"), PE_bool);

		q.Draw(IDs[2]);
		glBindTexture(GL_TEXTURE_2D, 0);
	//*/

	glFinish();
	glutSwapBuffers();
	glutPostRedisplay();
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
		if (times.size() >= 10)	//если в times слишком много элементов, выкидываем
			times.pop_back();
		times.push_back(delta);	//иначе записываем, сколько прошло

		double avg = accumulate(times.begin(), times.end(), 0.0) / times.size();	//считаем среднее арифметическое в times

		g_rot_angl += 0.0000005 * avg;
		if (g_rot_angl > 1000000.0)
			g_rot_angl = 0;
	}
	glutPostRedisplay();	//перерисовать текущее окно, вызывать display плохо, потому что может накопиться много idle, и мы застрянем
}

void mouse_click(int button, int state, int x, int y)
{
	
}

void key_press(unsigned char key, int x, int y)
{
	if (key == 'd')
		cb_rot_angl_y += 0.03;
	if (key == 'a')
		cb_rot_angl_y -= 0.03;
	if ((cb_rot_angl_y > 100000.0)|| (cb_rot_angl_y < -100000.0))
		cb_rot_angl_y = 0.0;
	if (key == 'w')
		PE_bool = 1;
	if (key == 's')
		PE_bool = 0;

	idle();
	//display();
}

int main(int argc, char** argv)
{
		setlocale(LC_ALL, "rus");
		//подготовка
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
		glutCreateWindow("My Scene");
		glewInit();
		

		//собираем шейдеры
		try {
			IDs.push_back(build_shaders("scene.vert", "scene.frag"));
			IDs.push_back(build_shaders("shadow.vert", "shadow.frag"));
			IDs.push_back(build_shaders("post_effect.vert", "post_effect.frag"));
		}
		catch (exception ex) {
			cout << ex.what() << endl;
			return -1;
		}

		//настройки фреймбуфера для SHADOWMAP

		glGenFramebuffers(1, &FBO_dm);
		glGenTextures(1, &depthMap);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO_dm);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Ошибка: ФБО не завершён" << endl;
		}
		else
			cout << "ФБО завершён" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//настройка фреймбуфера для постэффекта

		glGenFramebuffers(1, &FBO_pe);
		glGenTextures(1, &PostEffect);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, PostEffect);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO_pe);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostEffect, 0);

		//рендербуфер
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 1024);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Ошибка: ФБО не завершён" << endl;
		}
		else
			cout << "ФБО завершён" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Создание объектов

		try {
			vector <const char*> list;
			list.push_back("obj/wb_in.obj");
			list.push_back("obj/wb_out.obj");
			list.push_back("obj/tree_sized.obj");
			scene.CreateScene(list);

		}
		catch (exception ex)
		{
			cout << "Ошибка assimp: " << ex.what() << endl;
			return -2;
		}

		q.Gen();

		glPolygonMode(GL_FRONT, GL_FILL);

		//задаём функции обработки событий (те, что написаны выше)
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutIdleFunc(idle);
		glutMouseFunc(mouse_click);
		glutKeyboardFunc(key_press);

		PE_bool = 0;

		glutMainLoop();
	return 0;
	
}