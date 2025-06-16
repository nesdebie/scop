#include "ObjLoader.h"

bool operator==(const Vertex& a, const Vertex& b) {
    return a.position == b.position && a.normal == b.normal && a.texCoord == b.texCoord;
}

struct VertexHash {
    size_t operator()(const Vertex& v) const {
        return ((std::hash<float>()(v.position.x) ^ (std::hash<float>()(v.position.y) << 1)) >> 1) ^
               (std::hash<float>()(v.position.z) << 1) ^
               (std::hash<float>()(v.texCoord.x) << 1) ^
               (std::hash<float>()(v.texCoord.y) << 1);
    }
};

static bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size()-suffix.size(), suffix.size(), suffix) == 0;
}

static bool isValidOBJFilename(const std::string& filename) {
    return !(filename.empty() || !ends_with(filename, ".obj") || ends_with(filename, "/.obj") || filename.size() == 4);
}

static void parseVertexLine(std::istringstream& iss, std::vector<my_glm::vec3>& temp_positions) {
    my_glm::vec3 pos;
    iss >> pos.x >> pos.y >> pos.z;
    temp_positions.push_back(pos);
}

static void parseTexcoordLine(std::istringstream& iss, std::vector<my_glm::vec2>& temp_texcoords) {
    my_glm::vec2 uv;
    iss >> uv.x >> uv.y;
    uv.y = 1.0f - uv.y;
    temp_texcoords.push_back(uv);
}

static void parseNormalLine(std::istringstream& iss, std::vector<my_glm::vec3>& temp_normals) {
    my_glm::vec3 normal;
    iss >> normal.x >> normal.y >> normal.z;
    temp_normals.push_back(normal);
}

static void parseFaceLine(std::istringstream& iss, const std::vector<my_glm::vec3>& temp_positions,
                          const std::vector<my_glm::vec3>& temp_normals, const std::vector<my_glm::vec2>& temp_texcoords,
                          std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
                          std::unordered_map<Vertex, uint32_t, VertexHash>& uniqueVertices) {
    std::vector<uint32_t> faceIndices;
    std::string vertexString;

    while (iss >> vertexString) {
        std::istringstream vss(vertexString);
        std::string idxPos, idxTex, idxNorm;
        std::getline(vss, idxPos, '/');
        std::getline(vss, idxTex, '/');
        std::getline(vss, idxNorm, '/');

        int posIdx = idxPos.empty() ? -1 : std::stoi(idxPos) - 1;
        int texIdx = idxTex.empty() ? -1 : std::stoi(idxTex) - 1;
        int normIdx = idxNorm.empty() ? -1 : std::stoi(idxNorm) - 1;

        if (posIdx < 0 || posIdx >= (int)temp_positions.size()) continue;

        Vertex vertex{};
        vertex.position = temp_positions[posIdx];
        vertex.normal = (normIdx >= 0 && normIdx < (int)temp_normals.size()) ? temp_normals[normIdx] : my_glm::vec3(0.0f, 0.0f, 1.0f);
        vertex.texCoord = (texIdx >= 0 && texIdx < (int)temp_texcoords.size()) ? temp_texcoords[texIdx] : my_glm::vec2((temp_positions[posIdx].x + 1.0f) * 0.5f, (temp_positions[posIdx].y + 1.0f) * 0.5f);

        if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
            vertices.push_back(vertex);
        }
        faceIndices.push_back(uniqueVertices[vertex]);
    }

    for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
        indices.push_back(faceIndices[0]);
        indices.push_back(faceIndices[i]);
        indices.push_back(faceIndices[i + 1]);
    }
}

bool loadOBJ(const std::string& filename, std::vector<SubMesh>& submeshes) {
    if (!isValidOBJFilename(filename)) {
        std::cerr << "Filename is invalid." << std::endl;
        return false;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return false;
    }

    std::vector<my_glm::vec3> temp_positions;
    std::vector<my_glm::vec3> temp_normals;
    std::vector<my_glm::vec2> temp_texcoords;
    std::unordered_map<Vertex, uint32_t, VertexHash> uniqueVertices;
    std::map<std::string, std::string> materialTextures;
    std::map<std::string, my_glm::vec3> materialColors;

    std::map<std::string, my_glm::vec3> materialAmbient;
    std::map<std::string, my_glm::vec3> materialSpecular;
    std::map<std::string, my_glm::vec3> materialEmissive;
    std::map<std::string, float> materialSpecularExp;
    std::map<std::string, float> materialDissolve;
    std::map<std::string, float> materialRefraction;
    std::map<std::string, int> materialIllum;

    std::string line, mtlFilename;
    std::string currentMaterial;
    SubMesh currentSubMesh;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") parseVertexLine(iss, temp_positions);
        else if (prefix == "vt") parseTexcoordLine(iss, temp_texcoords);
        else if (prefix == "vn") parseNormalLine(iss, temp_normals);
        else if (prefix == "mtllib") {
            iss >> mtlFilename;
            std::ifstream mtl("models/" + mtlFilename);
            if (!mtl.is_open() && ends_with(mtlFilename, ".mtl"))
                mtl.open("models/mtl/" + mtlFilename);
            std::string mline, matName, texName;
            my_glm::vec3 kdColor(1.0f);
            while (std::getline(mtl, mline)) {
                std::istringstream miss(mline);
                std::string token;
                miss >> token;
                if (token == "newmtl") {
                    miss >> matName;
                    kdColor = my_glm::vec3(1.0f);
                } else if (token == "map_Kd") {
                    currentSubMesh.hasMapKdInitially = true;
                    miss >> texName;
                    if (!texName.empty()) {
                        std::ifstream texFile("models/" + texName);
                        if (texFile.good()) {
                            materialTextures[matName] = texName;
                            continue;
                        }
                        texFile.open("models/tex/" + texName);
                        if (texFile.good()) {
                            materialTextures[matName] = "tex/" + texName;
                            continue;
                        }
                        std::cerr << "Warning: Texture not found in models/ or models/tex/: " << texName << std::endl;
                    }
                } else if (token == "Kd") {
                    float r, g, b;
                    miss >> r >> g >> b;
                    kdColor = my_glm::vec3(r, g, b);
                    materialColors[matName] = kdColor;
                } else if (token == "Ka") {
                    float r, g, b;
                    miss >> r >> g >> b;
                    materialAmbient[matName] = my_glm::vec3(r, g, b);
                } else if (token == "Ks") {
                    float r, g, b;
                    miss >> r >> g >> b;
                    materialSpecular[matName] = my_glm::vec3(r, g, b);
                } else if (token == "Ke") {
                    float r, g, b;
                    miss >> r >> g >> b;
                    materialEmissive[matName] = my_glm::vec3(r, g, b);
                } else if (token == "Ns") {
                    float ns;
                    miss >> ns;
                    materialSpecularExp[matName] = ns;
                } else if (token == "d") {
                    float d;
                    miss >> d;
                    materialDissolve[matName] = d;
                } else if (token == "Ni") {
                    float ni;
                    miss >> ni;
                    materialRefraction[matName] = ni;
                } else if (token == "illum") {
                    int illum;
                    miss >> illum;
                    materialIllum[matName] = illum;
                }
            }
        } else if (prefix == "usemtl") {
            if (!currentSubMesh.vertices.empty()) {
                submeshes.push_back(currentSubMesh);
                currentSubMesh = SubMesh();
                uniqueVertices.clear();
            }
            iss >> currentMaterial;
            currentSubMesh.textureFile = materialTextures[currentMaterial];
            currentSubMesh.diffuseColor = materialColors.count(currentMaterial) ? materialColors[currentMaterial] : my_glm::vec3(1.0f);
            currentSubMesh.ambientColor = materialAmbient.count(currentMaterial) ? materialAmbient[currentMaterial] : my_glm::vec3(1.0f);
            currentSubMesh.specularColor = materialSpecular.count(currentMaterial) ? materialSpecular[currentMaterial] : my_glm::vec3(0.0f);
            currentSubMesh.emissiveColor = materialEmissive.count(currentMaterial) ? materialEmissive[currentMaterial] : my_glm::vec3(0.0f);
            currentSubMesh.specularExponent = materialSpecularExp.count(currentMaterial) ? materialSpecularExp[currentMaterial] : 1.0f;
            currentSubMesh.dissolve = materialDissolve.count(currentMaterial) ? materialDissolve[currentMaterial] : 1.0f;
            currentSubMesh.refractionIndex = materialRefraction.count(currentMaterial) ? materialRefraction[currentMaterial] : 1.0f;
            currentSubMesh.illumModel = materialIllum.count(currentMaterial) ? materialIllum[currentMaterial] : 2;
        } else if (prefix == "f") {
            parseFaceLine(iss, temp_positions, temp_normals, temp_texcoords,
                          currentSubMesh.vertices, currentSubMesh.indices, uniqueVertices);
        }
    }

    if (!currentSubMesh.vertices.empty()) {
        submeshes.push_back(currentSubMesh);
    }

    return true;
}

