#pragma once
#include <vector>
#include <iostream>
#include "Texture.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
 
    vector<Texture> textures_loaded;	
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;


    Model(string const& path, bool bSmoothNormals, bool gamma = false);

  
    void Draw(Shader& shader);

private:
   
    void loadModel(string const& path, bool bSmoothNormals);

  
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);


    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

