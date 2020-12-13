#ifndef MESH_H
#define MESH_H

#include<GL/glew.h>
#include<GL/freeglut.h>
#include<glm/glm.hpp>	//��� ������
#include<glm/gtx/transform.hpp>	//��� �������������� ��� ���������

#include <string>
#include <vector>
using namespace std;

struct Vertex { //� ������� 5 ����������: �������, �������, ���������� ����������, ����������� � �������������
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;    //����� ��������
    string type;    //� ���
    string path;    //� �����, �� �������� ��� �����
};

class Mesh {
public:
    vector<Vertex>       vertices;  //������ ������
    vector<unsigned int> indices;   //������ ������
    vector<Texture>      textures;  //������ �������    
    GLuint VAO;   //����������� vertex array object

    Mesh(vector<Vertex> v, vector<unsigned int> i, vector<Texture> t)
    {
        vertices = v;
        indices = i;
        textures = t;

        setupMesh();
    }

    void Draw()
    {
        //����� ����������, ���� ������� ��������� ��������
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int heightNr = 1;
        unsigned int ambientNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // ���������� ���������
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = to_string(specularNr++);
            else if (name == "texture_height")
                number = to_string(heightNr++);
            else if (name == "texture_ambient")
                number = to_string(ambientNr++);

            glUniform1i(glGetUniformLocation(program, (name + number).c_str()), i); //��������� texture_typeN � i-�� ���������
            glBindTexture(GL_TEXTURE_2D, textures[i].id);   //����������� ������� i-�� ��������� � �������� �� ������� �������
        }
        
        //������ ���
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

private:
    GLuint VBO, EBO;

    //����� ������ ��������� � �������� ���� �������� ��� ���������
    void setupMesh()
    {
        //����������
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        //����������� VAO
        glBindVertexArray(VAO);
        //����������� VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //������ � ������ ���� �������
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        //������ � ������ ���� �����, ����� DrawElements ���� ����� ������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        //��������� �������, ��� � ��� ����� ��������� ������ �����
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        //���������� VAO, ����� ������ �� ��������� (�� ����� ���� �� �����������, ������ "������� ���")
        glBindVertexArray(0);
    }
};
#endif