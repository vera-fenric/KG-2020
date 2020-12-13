#ifndef MODEL_H
#define MODEL_H

#include<GL/glew.h>
#include<GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FreeImage.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory);

class Model{
public:
    vector<Mesh>    meshes; //������� ������ ����� �������� �� ����� ����, �� ���� ������ �������, ����������� ������
    string directory;

    //� ������������ ��������� ���� � ����� .obj ������
    Model(string const& path)
    {
        //����������� ���� ����� assimp Importer, ��� ���� ������ �������������� ������, ���� �������� ���� ������: �������������, ������� ������� � �����������
        Assimp::Importer importer;
        const aiScene* assimp_model = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        //�������� �� ������
        if (!assimp_model || assimp_model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimp_model->mRootNode)
            throw exception(importer.GetErrorString());
        
        //������������� ����, �� �������� ����� ��������
        directory = path.substr(0, path.find_last_of('/'));

        //��������� ����������� ���������� Nod'�� ��������, ������� � root'�
        processNode(assimp_model->mRootNode, assimp_model);
    }

    //����� ���������� ��������, ������ ������ ������ ���
    void Draw(){
        for (int i = 0; i < meshes.size(); i++)
            meshes[i].Draw();
    }

private:

    //����������� ���������� �����, ����� ��������� ���, ����� ������ �� ��� � ���� �������� ��������
    //� assimpe ��� ���������� �����, � � �������� ������� ������ - ��� ����� ���� ��������� �������, �������, ��������, ���������
    //�������� ���� �� �����, �� �� ��������� ������ �������� � �� ���������� ����� � ������ ���������
    //������� � ���� ��������� aiScene ��������� assimp_model
    void processNode(aiNode* node, const aiScene* assimp_model)
    {
        //� ���� �������� ������� ����, ��� ��� ��� ����� ������� ��� ������� � ���������� �� ��� � ������� ����� ����� ������
        for (int i = 0; i < node->mNumMeshes; i++){
            aiMesh* mesh = assimp_model->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, assimp_model));  //������������ ��� � ������ �� � ������ ����� ������ ������ Model
        }
        //������ ����, ��� ��� ���� ����������, ��������� ��������� �����-�����:)
        for (int i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], assimp_model);
        }

    }

    //��������� ����, ����� ����������
    Mesh processMesh(aiMesh* mesh, const aiScene* assimp_model)
    {
        //������, ����� ��� ����� �� ����, ������� ���� � ����, ����� ����� ������� ��� � ����� ������
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        //���������� �� ���� �������� ����
        for (int i = 0; i < mesh->mNumVertices; i++){
            Vertex vertex;
            //������ �� assimp �� �������������� � glm::vec3, ������� ������ ��� �� ���������� ����� ����������
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            //����� ������� �������� �������, ���� ��� ����,
            if (mesh->HasNormals())
                vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            
            //���������� ����������...
            if (mesh->mTextureCoords[0]) //���� ����, �������
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            //�����������
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

            //� �������������
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

            vertices.push_back(vertex);//�� � ������ ������� � ������
        }

        //�� �������� �����������, ������ ����� �� ������, � ������ �� ������� � ������))
        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
                indices.push_back(mesh->mFaces[i].mIndices[j]);
        }

        //������������ ���������
        //��� ��������� ���������� �� ������ ������� (���� � ���� ������������ �� ������ ����� �������� ������� ���� �� ������ ���)
        // ��������� ��������� �����
        vector<Texture> diffuseMaps = loadMaterialTextures(assimp_model->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // ����� ���������
        vector<Texture> specularMaps = loadMaterialTextures(assimp_model->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // ����� ��������
        vector<Texture> normalMaps = loadMaterialTextures(assimp_model->mMaterials[mesh->mMaterialIndex], aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // � ����� �����
        vector<Texture> heightMaps = loadMaterialTextures(assimp_model->mMaterials[mesh->mMaterialIndex], aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        //�� � ���������� ��������� ���
        return Mesh(vertices, indices, textures);
    }

    //������� �������� �������
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (int i = 0; i < mat->GetTextureCount(type); i++)    //����� �� ���� ��������� ����� ��������� � ������ �����
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            Texture texture;
            try {
                texture.id = TextureFromFile(str.C_Str(), this->directory);
            }
            catch (exception ex) {
                cout << "�� ����������� �������� �� ������: " << ex.what() << endl;
                continue;
            };
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
        }
        return textures;
    }
};

//������� �������� ����� ��������
unsigned int TextureFromFile(const char* path, const string& directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height;
    FIBITMAP * bitmap = FreeImage_Load(FIF_PNG, filename.c_str(), PNG_DEFAULT);
    if (bitmap)
    {
        width = FreeImage_GetWidth(bitmap);
        height = FreeImage_GetHeight(bitmap);
        GLenum format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, bitmap);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        free(bitmap);
    }
    else
    {
        throw exception (path);
    }

    return textureID;
}
#endif
