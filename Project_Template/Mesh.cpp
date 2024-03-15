#include "Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture( glm::vec3 defaultColour) {
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    //setup texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL,0);

    //initialise single pixel
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, &defaultColour[0]);

    //unbind
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::load(std::string filePath) {

    glBindTexture(GL_TEXTURE_2D, handle);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels,3);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

std::vector<std::string> split(std::string str, char delimiter) {
    std::vector<std::string> buffer;

    //i moves through the input by hopping to the next delimiter found
    for (int i = 0; i < str.length(); i = str.find(delimiter, i) + 1) {
        //if next delimiter not found, i is null position, push the rest of the string
        if (str.find(delimiter, i) == std::string::npos) {
            buffer.push_back(str.substr(i, str.length() - i));
            break;
        }

        buffer.push_back(str.substr(i, str.find(delimiter, i) - i));
    }

    return buffer;
}

void loadOBJ(std::ifstream* fileStream, std::vector<GLuint>* indices, std::vector<glm::vec3>* vertexPositions, std::vector<glm::vec3>* vertexNormals, std::vector<glm::vec2>* vertexUVs) {

    std::vector<glm::vec3> faceNormals;
    std::vector<glm::vec2> textureCoords;

    std::string line;
    std::vector<std::string> lineSplit;

    bool quads = false;
    bool textured = false;

    while (std::getline(*fileStream, line)) {

        lineSplit = split(line, ' ');

        //add the vertex positions and initialise the vertex's normal
        if (lineSplit[0] == "v") {
            vertexPositions->push_back({
                std::stof(lineSplit[1]),
                std::stof(lineSplit[2]),
                std::stof(lineSplit[3])
                });
            vertexNormals->push_back({
                0,
                0,
                0
                });
            vertexUVs->push_back({
                0,
                0
                });
        }
        //gather up the faces normals
        else if (lineSplit[0] == "vn") {
            faceNormals.push_back({
                std::stof(lineSplit[1]),
                std::stof(lineSplit[2]),
                std::stof(lineSplit[3])
                });
        }
        else if (lineSplit[0] == "vt") {
            if(!textured) textured = true;
            textureCoords.push_back({
                std::stof(lineSplit[1]),
                std::stof(lineSplit[2])
                });
        }
        //generate vertex normals array using face normals
        else if (lineSplit[0] == "f") {
            //bool quads = false;
            //bool textured = false;
            //if (!quads && lineSplit.size() == 4) quads = true;

            //triangle face
            for (int v = 0; v < 3; v++) {

                //add index of face vertex
                indices->push_back(std::stoi(split(lineSplit[1 + v], '/')[0]) - 1);

                //adjust vertex normal from face normal
                (*vertexNormals)[std::stoi(split(lineSplit[1 + v], '/')[0]) - 1] += faceNormals[std::stoi(split(lineSplit[1 + v], '/')[2]) - 1];

                //set vertex texture coordinate
                if(textured) (*vertexUVs)[std::stoi(split(lineSplit[1 + v], '/')[0]) - 1] = textureCoords[std::stoi(split(lineSplit[1 + v], '/')[1]) - 1];
            }
        }
    }
}

Model::Model() {
    glGenVertexArrays(1, &vaoHandle);
    glGenBuffers(2, vboHandles);
}

void Model::loadBufferData(std::vector<glm::vec3> positionData, std::vector<glm::vec3> normalsData, std::vector<glm::vec2> textureData, std::vector<GLuint> indices) {
    //bind the first VBO and initialise empty buffer to store positions and colour data
    glBindBuffer(GL_ARRAY_BUFFER, vboHandles[0]);
    glBufferData(GL_ARRAY_BUFFER, (positionData.size()*3 + normalsData.size()*3 + textureData.size()*2) * sizeof(float), NULL, GL_STATIC_DRAW);
    //load positions into buffer 0
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        positionData.size() * sizeof(glm::vec3),
        &positionData[0]
    );
    //load normals into buffer 0 after positions
    glBufferSubData(
        GL_ARRAY_BUFFER,
        positionData.size() * sizeof(glm::vec3),
        normalsData.size() * sizeof(glm::vec3),
        &normalsData[0]
    );
    glBufferSubData(
        GL_ARRAY_BUFFER,
        (positionData.size()+normalsData.size()) * sizeof(glm::vec3),
        textureData.size() * sizeof(glm::vec2),
        &textureData[0]
    );

    //bind the second VBO and load indices to the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    //bind VAO to the opengl context
    glBindVertexArray(vaoHandle);
    //bind buffer data to vertex shader attributes and enable
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(positionData.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)((positionData.size() + normalsData.size()) * sizeof(glm::vec3)));
    glEnableVertexAttribArray(3);

    //colourTexture.load("./media/textures/blanktexture.jpg");
    //loadTexture("./media/textures/blanktexture.jpg");
    glBindVertexArray(0);
}

void Model::loadFileModel(std::string filePath) {

    std::vector<glm::vec3> positionData;
    std::vector<glm::vec3> normalsData;
    std::vector<glm::vec2> textureData;
    std::vector<GLuint> indices;

    std::cout << "loading model from file '" << filePath << "'" << std::endl;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream) return;
    loadOBJ(&fileStream, &indices, &positionData, &normalsData,&textureData);

    fileStream.close();

    indicesCount = indices.size();
    loadBufferData(positionData, normalsData,textureData, indices);
}

void Model::drawModel() {

    program->setUniform("model", transform);

    //set material uniforms from model info
    program->setUniform("mtl.ambientReflectivity", mtl.ambientReflectivity);
    program->setUniform("mtl.diffuseReflectivity", mtl.diffuseReflectivity);
    program->setUniform("mtl.specularReflectivity", mtl.specularReflectivity);
    program->setUniform("mtl.specularPower", mtl.specularPower);

    //set shading flags
    program->setUniform("mtl.shadeFlat", mtl.shadeFlat);
    program->setUniform("mtl.perFragment", mtl.perFragment);

    //bind colour texture to TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colourTexture.handle);

    //bind normal map to TEXTURE1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap.handle);

    //draw model
    glBindVertexArray(vaoHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[1]);
    glDrawElements(drawMode, indicesCount, GL_UNSIGNED_INT, 0);
}

/*
void Model::loadTexture(std::string filePath) {

    glBindTexture(GL_TEXTURE_2D, textureHandle);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
}*/