#ifndef SCENE_H
#define SCENE_H

#include<GL/glew.h>
#include<GL/freeglut.h>
#include<glm/glm.hpp>	//��� ������
#include<glm/gtx/transform.hpp>	//��� �������������� ��� ���������

#include<vector>
#include<model.h>	//��� ������

class Scene {
public:
	vector<Model> Models;
	vector<glm::mat4x4> Models_position;

	Scene() {};

	void CreateScene(vector<const char*>list) {

		for (int i = 0; i < list.size(); i++) {
			Models.push_back(Model(list[i]));

			Models_position.push_back(glm::mat4x4(0.0f));

		}
	};

	~Scene() {
		Models.clear();
		Models_position.clear();
	};

	void render(GLuint program) {
		for (int i = 0; i < Models_position.size(); i++)
		{
			glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[i])[0][0]);
			Models[i].Draw(program);
		};
		return;
	};

	void render1(GLuint program, float rot_angl_1, float rot_angl_2) {

		Models_position[0] = glm::translate(glm::vec3(0.0f, -1.25f, -3.0f)) *
			glm::rotate(rot_angl_1, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::scale(glm::vec3(1.0f, 1.0f, 2.0f));
		Models_position[1] = glm::translate(glm::vec3(0.0f, -1.25f, -3.0f)) *
			glm::rotate(rot_angl_1, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::scale(glm::vec3(1.0f, 1.0f, 2.0f));
		Models_position[2] = glm::translate(glm::vec3(0.0f, -1.25f, -3.0f)) *
			glm::rotate(rot_angl_2, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(rot_angl_1, glm::vec3(0.0f, 1.0f, 0.0f));

		//������ ������
		glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[2])[0][0]);//�������� ��������������� ��������� ��������� � �����
		Models[2].Draw(program);

		//������ ������������ �����
		glEnable(GL_STENCIL_TEST);	//�������� ���� ���������
		glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);	//����������� ���� ��������� � �������
		glStencilFunc(GL_ALWAYS, 1, 1);
		glDepthMask(GL_FALSE);
		glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[0])[0][0]);	//������ � ���������� �� �������
		Models[0].Draw(program);
		glDepthMask(GL_TRUE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 1);

		//������ ��������� ������
		glDepthMask(GL_FALSE);
		Models_position[2] = glm::translate(glm::vec3(0.0f, -0.95f, 0.0f)) * glm::scale(glm::vec3(1.0f, -1.0f, 1.0f)) * Models_position[2];
		glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[2])[0][0]);//�������� ��������������� ��������� ��������� � �����
		Models[2].Draw(program);
		glDepthMask(GL_TRUE);

		//��������� ���� ���������
		glDisable(GL_STENCIL_TEST);

		//������ ������������ �����2
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[0])[0][0]);//�������� ��������������� ��������� ��������� � �����
		Models[0].Draw(program);

		glDisable(GL_BLEND);

		//������ ������� ����� �����

		glUniformMatrix4fv(glGetUniformLocation(program, "model_pos"), 1, GL_FALSE, &(Models_position[1])[0][0]);//�������� ��������������� ��������� ��������� � �����
		Models[1].Draw(program);
		
	};

};

#endif