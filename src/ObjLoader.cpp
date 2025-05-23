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

static void parseVertexLine(std::istringstream& iss, std::vector<glm::vec3>& temp_positions) {
    glm::vec3 pos;
    iss >> pos.x >> pos.y >> pos.z;
    temp_positions.push_back(pos);
}

static void parseTexcoordLine(std::istringstream& iss, std::vector<glm::vec2>& temp_texcoords) {
    glm::vec2 uv;
    iss >> uv.x >> uv.y;
    uv.y = 1.0f - uv.y;
    temp_texcoords.push_back(uv);
}

static void parseNormalLine(std::istringstream& iss, std::vector<glm::vec3>& temp_normals) {
    glm::vec3 normal;
    iss >> normal.x >> normal.y >> normal.z;
    temp_normals.push_back(normal);
}

static void parseFaceLine(std::istringstream& iss, const std::vector<glm::vec3>& temp_positions,
                          const std::vector<glm::vec3>& temp_normals, const std::vector<glm::vec2>& temp_texcoords,
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
        vertex.normal = (normIdx >= 0 && normIdx < (int)temp_normals.size()) ? temp_normals[normIdx] : glm::vec3(0.0f, 0.0f, 1.0f);
        vertex.texCoord = (texIdx >= 0 && texIdx < (int)temp_texcoords.size()) ? temp_texcoords[texIdx] : glm::vec2((temp_positions[posIdx].x + 1.0f) * 0.5f, (temp_positions[posIdx].y + 1.0f) * 0.5f);

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

// static bool parseMTL(const std::string& mtlFilename, std::string& textureFile) {
//     std::ifstream mtlFile("models/" + mtlFilename);
//     if (!mtlFile.is_open() && ends_with(mtlFilename, ".mtl")) {
//         mtlFile.open("models/mtl/" + mtlFilename);
//     }
//     if (!mtlFile.is_open()) {
//         std::cerr << "Failed to open MTL file: " << mtlFilename << std::endl;
//         return false;
//     }

//     std::string mtlLine;
//     while (std::getline(mtlFile, mtlLine)) {
//         std::istringstream mtlIss(mtlLine);
//         std::string mtlPrefix;
//         mtlIss >> mtlPrefix;

//         if (mtlPrefix == "map_Kd") {
//             std::string texCandidate;
//             mtlIss >> texCandidate;

//             std::ifstream texFile("models/" + texCandidate);
//             if (texFile.good() && ends_with(texCandidate, ".png")) {
//                 textureFile = texCandidate;
//                 return true;
//             }

//             texFile.open("models/tex/" + texCandidate);
//             if (texFile.good() && ends_with(texCandidate, ".png")) {
//                 textureFile = "tex/" + texCandidate;
//                 return true;
//             }

//             std::cerr << "Warning: Texture not found in models/ or models/tex/: " << texCandidate << std::endl;
//         }
//     }
//     return true;
// }

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

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texcoords;

    std::unordered_map<Vertex, uint32_t, VertexHash> uniqueVertices;
    std::string line, mtlFilename;
    std::map<std::string, std::string> materialTextures;
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
            while (std::getline(mtl, mline)) {
                std::istringstream miss(mline);
                std::string token;
                miss >> token;
                if (token == "newmtl") miss >> matName;
                else if (token == "map_Kd") {
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
