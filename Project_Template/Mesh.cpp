#include "Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(vec3 defaultColour) 
{
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    //setup texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //initialise single pixel texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, &defaultColour[0]);

    //unbind
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::load(std::string filePath) 
{
    stbi_set_flip_vertically_on_load(true);
    glBindTexture(GL_TEXTURE_2D, handle);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels,3);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else std::cout << "Failed to load texture" << std::endl;
    
    stbi_image_free(data);
}

vector<std::string> split(std::string str, char delimiter) {
    vector<std::string> buffer;

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

void loadOBJ(std::ifstream* fileStream, vector<GLuint>* indices, vector<vec3>* vertexPositions, vector<vec3>* vertexNormals, vector<glm::vec2>* vertexUVs) 
{
    vector<vec3> positions;
    vector<vec3> normals;
    vector<glm::vec2> texCoords;

    std::string line;
    vector<std::string> lineSplit;

    //bool quads = false;
    bool textured = false;

    while (std::getline(*fileStream, line)) {

        lineSplit = split(line, ' ');

        if (lineSplit[0] == "v")
        {
            positions.push_back({
                std::stof(lineSplit[1]),
                std::stof(lineSplit[2]),
                std::stof(lineSplit[3])
            });
        }
        else if (lineSplit[0] == "vn")
        {
            normals.push_back({
                std::stof(lineSplit[1]),
                std::stof(lineSplit[2]),
                std::stof(lineSplit[3])
            });
        }
        else if (lineSplit[0] == "vt")
        {
            if(!textured) textured = true;
            texCoords.push_back({
                std::stof(lineSplit[1]),
                std::stof(lineSplit[2])
            });
        }
        else if (lineSplit[0] == "f")
        {
            //triangle face
            for (int v = 0; v < 3; v++) {

                auto vertexAttributes = split(lineSplit[1 + v], '/');

                int positionIndex = std::stoi(vertexAttributes[0]) - 1;
                int texCoordIndex = std::stoi(vertexAttributes[1]) - 1;
                int normalIndex = std::stoi(vertexAttributes[2]) - 1;

                //add index of face vertex
                indices->push_back(indices->size());

                vertexPositions->push_back(positions[positionIndex]);

                vertexNormals->push_back(normals[normalIndex]);

                vertexUVs->push_back(texCoords[texCoordIndex]);

            }
        }
    }
}

Model::Model() {
    glGenVertexArrays(1, &vaoHandle);
    glGenBuffers(2, vboHandles);
}

void Model::loadBufferData(vector<vec3> positionData, vector<vec3> normalsData, vector<glm::vec2> textureData, vector<GLuint> indices) {
    //bind the first VBO and initialise empty buffer to store positions and colour data
    glBindBuffer(GL_ARRAY_BUFFER, vboHandles[0]);
    glBufferData(GL_ARRAY_BUFFER, (positionData.size()*3 + normalsData.size()*3 + textureData.size()*2) * sizeof(float), NULL, GL_STATIC_DRAW);

    //load positions into buffer 0
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        positionData.size() * sizeof(vec3),
        &positionData[0]
    );

    //load normals
    glBufferSubData(
        GL_ARRAY_BUFFER,
        positionData.size() * sizeof(vec3),
        normalsData.size() * sizeof(vec3),
        &normalsData[0]
    );

    //load UVs
    glBufferSubData(
        GL_ARRAY_BUFFER,
        (positionData.size()+normalsData.size()) * sizeof(vec3),
        textureData.size() * sizeof(glm::vec2),
        &textureData[0]
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[1]); //bind indices buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW); //initialize indices data

    glBindVertexArray(vaoHandle);//bind vao

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //positions to 0
    glEnableVertexAttribArray(0); //enable attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(positionData.size() * sizeof(vec3))); //normals to 2
    glEnableVertexAttribArray(2); //enable attribute
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)((positionData.size() + normalsData.size()) * sizeof(vec3))); //UVs to 3
    glEnableVertexAttribArray(3); //enable attribute

    glBindVertexArray(0); //unbind vao
}

void Model::loadFileModel(std::string filePath) 
{
    vector<vec3> positions;
    vector<vec3> normals;
    vector<glm::vec2> texCoords;
    vector<GLuint> indices;

    std::cout << "loading model from file '" << filePath << "'" << std::endl;
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream) return;

    loadOBJ(&fileStream, &indices, &positions, &normals,&texCoords);

    fileStream.close();

    indicesCount = indices.size();
    loadBufferData(positions, normals,texCoords, indices);
}

void Model::drawModel() {

    program->setUniform("model", transform);

    //set material uniforms from model info
    program->setUniform("mtl.ambientReflectivity", mtl.ambientReflectivity);
    program->setUniform("mtl.diffuseReflectivity", mtl.diffuseReflectivity);
    program->setUniform("mtl.specularReflectivity", mtl.specularReflectivity);
    program->setUniform("mtl.specularPower", mtl.specularPower);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[1]); //bind indices buffer
    glDrawElements(drawMode, indicesCount, GL_UNSIGNED_INT, 0); //draw elements from indices
}
