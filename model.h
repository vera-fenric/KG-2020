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
    vector<Mesh>    meshes; //простые модели могут состоять из одной меши, но если модель сложная, понадобится вектор
    string directory;

    //в конструкторе указываем путь к нашей .obj модели
    Model(string const& path)
    {
        //импортируем файл через assimp Importer, при этом делаем дополнительную работу, если исходный файл плохой: триангулируем, считаем нормали и касательные
        Assimp::Importer importer;
        const aiScene* assimp_model = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        //проверка на ошибки
        if (!assimp_model || assimp_model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimp_model->mRootNode)
            throw exception(importer.GetErrorString());
        
        //устанавливаем путь, по которому лежит моделька
        directory = path.substr(0, path.find_last_of('/'));

        //запускаем рекурсивное вычисление Nod'ов модельки, начиная с root'а
        processNode(assimp_model->mRootNode, assimp_model);
    }

    //чтобы нарисовать модельку, просто рисуем каждую меш
    void Draw(){
        for (int i = 0; i < meshes.size(); i++)
            meshes[i].Draw();
    }

private:

    //рекурсивное вычисление нодов, чтобы вычислить нод, нужна ссылка на нод и сама исходная моделька
    //в assimpe она называется сцена, и в принципе понятно почему - там могут быть несколько моделей, которые, допустим, двигаются
    //зависимо друг от друга, но мы выгружаем именно модельки и не используем связи в данной программе
    //поэтому я буду называеть aiScene моделькой assimp_model
    void processNode(aiNode* node, const aiScene* assimp_model)
    {
        //в ноде хранятся индексы меши, так что нам нужно достать эти индексы и обратиться по ним к массиву мешей самой модели
        for (int i = 0; i < node->mNumMeshes; i++){
            aiMesh* mesh = assimp_model->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, assimp_model));  //обрабатываем меш и пихаем ей в массив мешей нашего класса Model
        }
        //послет того, как все меши обработаны, запускаем обработку нодов-деток:)
        for (int i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], assimp_model);
        }

    }

    //обработка меши, самое интересное
    Mesh processMesh(aiMesh* mesh, const aiScene* assimp_model)
    {
        //собсна, здесь нам нужны те поля, которые есть в меше, чтобы потом вернуть меш с этими полями
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        //проходимся по всем вершинам меши
        for (int i = 0; i < mesh->mNumVertices; i++){
            Vertex vertex;
            //вектор из assimp не конвертируется в glm::vec3, поэтому делаем это всё копипастой через координаты
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            //также грустно копируем нормали, если они есть,
            if (mesh->HasNormals())
                vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            
            //текстурные координаты...
            if (mesh->mTextureCoords[0]) //если есть, конечно
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            //касательные
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

            //и бикасательные
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

            vertices.push_back(vertex);//ну и пихаем вершину в массив
        }

        //по вершинам пробежались, теперь бежим по граням, и пихаем их вершины в массив))
        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
                indices.push_back(mesh->mFaces[i].mIndices[j]);
        }

        //обрабатываем материалы
        //при обработке занумеруем их особым образом (хотя я буду использовать не больше одной текстуры каждого типа на каждую меш)
        // Загружаем диффузные карты
        vector<Texture> diffuseMaps = loadMaterialTextures(assimp_model->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // карты отражения
        vector<Texture> specularMaps = loadMaterialTextures(assimp_model->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // карты нормалей
        vector<Texture> normalMaps = loadMaterialTextures(assimp_model->mMaterials[mesh->mMaterialIndex], aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // и карты высот
        vector<Texture> heightMaps = loadMaterialTextures(assimp_model->mMaterials[mesh->mMaterialIndex], aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        //ну в возвращаем собранную меш
        return Mesh(vertices, indices, textures);
    }

    //функция загрузки текстур
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (int i = 0; i < mat->GetTextureCount(type); i++)    //бежим по всем текстурам этого материала с данным типом
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            Texture texture;
            try {
                texture.id = TextureFromFile(str.C_Str(), this->directory);
            }
            catch (exception ex) {
                cout << "Не загрузилась текстура по адресу: " << ex.what() << endl;
                continue;
            };
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
        }
        return textures;
    }
};

//функция загрузки одной текстуры
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
