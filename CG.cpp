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

//mesh и model во многом взяты с learnopengl, врать не буду, мне было проще по тому коду разобраться, как работает assimp
//модели в контейнерах - это, конечно, прикольно, я сделала их (во второй версии, залитой на гитхаб кибертянка в формате контейнера,
//пришлось самостоятельно писать прогу, которая мне экспортнёт obj в нужный формат),
//но на такие модели сложнее натянуть текстуры - в блендере, например, используется UV-развёртка, и чтобы аккуратно и правильно
//всё экспортировать, я использовала assimp, соответственно, после этого нужно было всё разобрать по моделькам

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
float cb_rot_angl = 0;	//угол поворота
float g_rot_angl = 0;
bool cb_rot_right = true;	//поворот направо

//для того, чтобы поворот был гладкий
vector<float> times(10);	//динамический массив на 10 элементов
chrono::steady_clock::time_point prevFrame;	//время предыдущего фрейма
bool firstFrame;	//флаг первого врейма

void reshape(int w, int h)
{
	projection = glm::perspectiveFovRH(45.0f, float(w), float(h), 1.0f, 20.0f);	//fov, размер экрана, ближняя и дальняя плоскость отсечения
	glViewport(0, 0, w, h);

	//при растяжении экрана меняем очищаем массив и ставим флажок, чтобы не бегать по пустому массиву
	times.clear();
	firstFrame = true;
}

void display()
{
	glm::mat4x4 view = glm::mat4(1.0f);
	glUseProgram/*ObjectARB*/(program);
	glClear(GL_COLOR_BUFFER_BIT);	//очищаем экран
	Models_position[0] = glm::translate(glm::vec3(0.0f, -1.25f, -3.0f)) *
		glm::rotate(cb_rot_angl, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::vec3(1.0f, 1.0f, 2.0f));
	Models_position[1] = glm::translate(glm::vec3(0.0f, -0.5f, -3.0f)) *
		glm::rotate(g_rot_angl, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::vec3(0.8f, 0.8f, 0.8f));

	for (int i = 0; i < Models.size(); i++) {
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, &projection[0][0]);//привязка идентификаторов шейдерной программы и нашей
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);//привязка идентификаторов шейдерной программы и нашей
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &(Models_position[i])[0][0]);//привязка идентификаторов шейдерной программы и нашей
		Models[i].Draw();
	}
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
		if (times.size() >= 10)	//если в times слишком много элементов, выкидываем
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
		g_rot_angl += 0.0000005 * avg;
		if (g_rot_angl > 1000000)
			g_rot_angl = 0;
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
		glutInitDisplayMode(GLUT_RGBA);
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
		Model Wardrobe = Model("obj/my_wb.obj");
		Models.push_back(Wardrobe);
		Models_position.push_back(glm::mat4x4(0.0f));
		Model CyberChan = Model("obj/CyberChan.obj");
		Models.push_back(CyberChan);
		Models_position.push_back(glm::mat4x4(0.0f));

		/*Model Wardrobe("obj/my_wb.obj");

		glGenBuffers(1, &vertexBuffer);	//создаём буффер
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //биндим буффер
		glBufferData(GL_ARRAY_BUFFER, sizeof(CyberChan_vertices), CyberChan_vertices, GL_STATIC_DRAW);//указываем, какие данные будут в буффере

		glGenVertexArrays(1, &vertexArray);	//генерируем массив вершин
		glBindVertexArray(vertexArray);	//биндим его
		int cb_pos = glGetAttribLocation(program, "model_pos");	//получем номер model_pos из связанной с шейдером программы
		glVertexAttribPointer(cb_pos, 3, GL_FLOAT, GL_FALSE/*нормирован, 0, 0);	//указываем, что в model_pos хранится вектор из 3 значений тип float, ненормированный
		glEnableVertexAttribArray(cb_pos);//генерируем массив вершин для передачи в вершинный шейдер
		*/

		glClearColor(0.0, 0.0, 0.0, 0.0);	//задаём цвет очистки
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//задаём тип рисования полигонов

		//задаём функции обработки событий (те, что написаны выше)
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutIdleFunc(idle);

		glutMainLoop();
		while (Models.size() > 0)
			Models.pop_back();
	return 0;
	
}