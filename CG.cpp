#include<GL/glew.h>
#include<GL/freeglut.h>
#include<glm/glm.hpp>	//для матриц
#include<glm/gtx/transform.hpp>	//для преобразований над матрицами

//для того, чтобы объекты крутились с нормальной скоростью
#include<vector>
#include<chrono>
#include<numeric>

#include<init.h>	//собираем шейдеры
#include<mesh.h>	//тип вершины и меши
#include<model.h>	//тип модели
#include<lamp.h>	//тип лампы

using namespace std;

//для рисования
GLuint vertexBuffer;
GLuint vertexArray;
//GLuint program;
vector<Model> Models;
vector<glm::mat4x4> Models_position;

//матрица проецирования
glm::mat4x4 projection;

//для поворота шкафа
//угол поворота
float cb_rot_angl_y = 0;

float g_rot_angl = 0;

//для того, чтобы поворот был гладкий
vector<float> times(10);	//динамический массив на 10 элементов
chrono::steady_clock::time_point prevFrame;	//время предыдущего фрейма
bool firstFrame;	//флаг первого врейма

glm::vec4 Lamp = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	if (h > 0)
		projection = glm::perspectiveFovRH(45.0f, float(w), float(h), 1.0f, 20.0f);	//fov, размер экрана, ближняя и дальняя плоскость отсечения

	//при растяжении экрана меняем очищаем массив и ставим флажок, чтобы не бегать по пустому массиву
	times.clear();
	firstFrame = true;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glm::mat4x4 view = glm::mat4(1.0f);
	glUseProgram/*ObjectARB*/(program);
	Models_position[0] = glm::translate(glm::vec3(0.0f, -1.25f, -3.0f)) *
		glm::rotate(cb_rot_angl_y, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::vec3(1.0f, 1.0f, 2.0f));
	Models_position[1] = glm::translate(glm::vec3(0.0f, -1.25f, -3.0f)) *
		glm::rotate(cb_rot_angl_y, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::vec3(1.0f, 1.0f, 2.0f));
	Models_position[2] = glm::translate(glm::vec3(0.0f, -0.45f, -3.1f)) *
		glm::rotate(g_rot_angl, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(cb_rot_angl_y, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, &Lamp[0]);//привязка идентификаторов шейдерной программы и нашей
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, &projection[0][0]);//привязка идентификаторов шейдерной программы и нашей
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);//привязка идентификаторов шейдерной программы и нашей

	//рисуем девчулю
	glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[2])[0][0]);//привязка идентификаторов шейдерной программы и нашей
	Models[2].Draw();
	
	//"рисуем" внутренность шкафа
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[0])[0][0]);
	Models[0].Draw();
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_EQUAL, 1, 1);
	
	//рисуем девчулю2
	Models_position[2] = glm::translate(glm::vec3(0.0f, -0.45f, -3.1f)) *
		glm::rotate(g_rot_angl, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(cb_rot_angl_y, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::vec3(0.5f, -0.5f, 0.5f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[2])[0][0]);//привязка идентификаторов шейдерной программы и нашей
	Models[2].Draw();
	
	glDisable(GL_STENCIL_TEST);

	//рисуем внутренности шкафа2
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[0])[0][0]);//привязка идентификаторов шейдерной программы и нашей
	Models[0].Draw();
	glDisable(GL_BLEND);
	
	//рисуем внешнюю часть шкафа
	
	glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[1])[0][0]);//привязка идентификаторов шейдерной программы и нашей
	Models[1].Draw();
	

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
		if (g_rot_angl > 1000000)
			g_rot_angl = 0;
	}
	glutPostRedisplay();	//перерисовать текущее окно, вызывать display плохо, потому что может накопиться много idle, и мы застрянем
}

void mouse_click(int button, int state, int x, int y)
{
	double avg = accumulate(times.begin(), times.end(), 0.0) / times.size();	//считаем среднее арифметическое в times
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			cb_rot_angl_y += 0.000005 * avg;
			if (cb_rot_angl_y > 1000000)
				cb_rot_angl_y = 0;
		}
	}
	display();
}

int main(int argc, char** argv)
{

		//подготовка
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
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

		glEnable(GL_DEPTH_TEST);

		Model Wardrobe1 = Model("obj/my_wb1.obj");
		Models.push_back(Wardrobe1);
		Models_position.push_back(glm::mat4x4(0.0f));

		Model Wardrobe2 = Model("obj/my_wb2.obj");
		Models.push_back(Wardrobe2);
		Models_position.push_back(glm::mat4x4(0.0f));

		Model CyberChan = Model("obj/tree.obj");
		Models.push_back(CyberChan);
		Models_position.push_back(glm::mat4x4(0.0f));

		glClearColor(0.0, 0.0, 0.0, 0.0);	//задаём цвет очистки
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);	//задаём тип рисования полигонов

		//задаём функции обработки событий (те, что написаны выше)
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutIdleFunc(idle);
		glutMouseFunc(mouse_click);

		glutMainLoop();
		while (Models.size() > 0)
			Models.pop_back();
	return 0;
	
}