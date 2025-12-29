#include "ObjLoader.h"

// ============================================================================
// Vertex Comparison and Hashing
// ============================================================================

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

// ============================================================================
// Utility Functions
// ============================================================================

static bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static bool isValidOBJFilename(const std::string& filename) {
    return !(filename.empty() || 
             !ends_with(filename, ".obj") || 
             ends_with(filename, "/.obj") || 
             filename.size() == 4);
}

// ============================================================================
// OBJ File Parsers
// ============================================================================

static void parseVerticesLine(std::istringstream& iss, std::vector<my_glm::vec3>& temp_positions) {
    my_glm::vec3 pos;
    iss >> pos.x >> pos.y >> pos.z;
    temp_positions.push_back(pos);
}

static void parseTextureCoordinatesLine(std::istringstream& iss, std::vector<my_glm::vec2>& temp_texcoords) {
    my_glm::vec2 uv;
    iss >> uv.x >> uv.y;
    uv.y = 1.0f - uv.y;
    temp_texcoords.push_back(uv);
}

static void parseVertexNormalLine(std::istringstream& iss, std::vector<my_glm::vec3>& temp_normals) {
    my_glm::vec3 normal;
    iss >> normal.x >> normal.y >> normal.z;
    temp_normals.push_back(normal);
}

static void parseFaceLine(std::istringstream& iss, 
                          const std::vector<my_glm::vec3>& temp_positions,
                          const std::vector<my_glm::vec3>& temp_normals, 
                          const std::vector<my_glm::vec2>& temp_texcoords,
                          std::vector<Vertex>& vertices, 
                          std::vector<uint32_t>& indices,
                          std::unordered_map<Vertex, uint32_t, VertexHash>& uniqueVertices) {
    std::vector<uint32_t> faceIndices;
    std::string vertexString;

    // Parse each vertex of the face
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

        // Build vertex with position, normal, and texture coordinates
        Vertex vertex{};
        vertex.position = temp_positions[posIdx];
        vertex.normal = (normIdx >= 0 && normIdx < (int)temp_normals.size()) 
                        ? temp_normals[normIdx] 
                        : my_glm::vec3(0.0f, 0.0f, 1.0f);
        vertex.texCoord = (texIdx >= 0 && texIdx < (int)temp_texcoords.size()) 
                          ? temp_texcoords[texIdx] 
                          : my_glm::vec2((temp_positions[posIdx].x + 1.0f) * 0.5f, 
                                         (temp_positions[posIdx].y + 1.0f) * 0.5f);

        // Add vertex to the list if it's unique
        if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
            vertices.push_back(vertex);
        }
        faceIndices.push_back(uniqueVertices[vertex]);
    }

    // Triangulate the face (fan triangulation)
    for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
        indices.push_back(faceIndices[0]);
        indices.push_back(faceIndices[i]);
        indices.push_back(faceIndices[i + 1]);
    }
}

// ============================================================================
// Material Structures and Functions
// ============================================================================

struct MaterialData {
    std::map<std::string, std::string> textures;
    std::map<std::string, my_glm::vec3> diffuseColors;
    std::map<std::string, my_glm::vec3> ambientColors;
    std::map<std::string, my_glm::vec3> specularColors;
    std::map<std::string, my_glm::vec3> emissiveColors;
    std::map<std::string, float> specularExponents;
    std::map<std::string, float> dissolves;
    std::map<std::string, float> refractionIndices;
    std::map<std::string, int> illumModels;
};

static bool tryOpenTexture(const std::string& texName, std::string& resultPath) {
    std::ifstream texFile("models/" + texName);
    if (texFile.good()) {
        resultPath = texName;
        return true;
    }
    texFile.open("models/tex/" + texName);
    if (texFile.good()) {
        resultPath = "tex/" + texName;
        return true;
    }
    return false;
}

static void parseMaterialFile(const std::string& mtlFilename, 
                               MaterialData& materialData,
                               bool& hasMapKdInitially) {
    std::ifstream mtl("models/" + mtlFilename);
    if (!mtl.is_open() && ends_with(mtlFilename, ".mtl")) {
        mtl.open("models/mtl/" + mtlFilename);
    }
    if (!mtl.is_open()) return;

    std::string mline, matName, texName;
    my_glm::vec3 kdColor(1.0f);

    while (std::getline(mtl, mline)) {
        std::istringstream miss(mline);
        std::string token;
        miss >> token;

        if (token == "newmtl") {
            miss >> matName;
            kdColor = my_glm::vec3(1.0f);
        } 
        else if (token == "map_Kd") {
            hasMapKdInitially = true;
            miss >> texName;
            if (!texName.empty()) {
                std::string resultPath;
                if (tryOpenTexture(texName, resultPath)) {
                    materialData.textures[matName] = resultPath;
                } else {
                    std::cerr << "Warning: Texture not found in models/ or models/tex/: " 
                              << texName << std::endl;
                }
            }
        } 
        else if (token == "Kd") {
            float r, g, b;
            miss >> r >> g >> b;
            kdColor = my_glm::vec3(r, g, b);
            materialData.diffuseColors[matName] = kdColor;
        } 
        else if (token == "Ka") {
            float r, g, b;
            miss >> r >> g >> b;
            materialData.ambientColors[matName] = my_glm::vec3(r, g, b);
        } 
        else if (token == "Ks") {
            float r, g, b;
            miss >> r >> g >> b;
            materialData.specularColors[matName] = my_glm::vec3(r, g, b);
        } 
        else if (token == "Ke") {
            float r, g, b;
            miss >> r >> g >> b;
            materialData.emissiveColors[matName] = my_glm::vec3(r, g, b);
        } 
        else if (token == "Ns") {
            float ns;
            miss >> ns;
            materialData.specularExponents[matName] = ns;
        } 
        else if (token == "d") {
            float d;
            miss >> d;
            materialData.dissolves[matName] = d;
        } 
        else if (token == "Ni") {
            float ni;
            miss >> ni;
            materialData.refractionIndices[matName] = ni;
        } 
        else if (token == "illum") {
            int illum;
            miss >> illum;
            materialData.illumModels[matName] = illum;
        }
    }
}

static void applyMaterialToSubMesh(SubMesh& subMesh, 
                                    const std::string& materialName,
                                    const MaterialData& materialData) {
    // Default values
    const my_glm::vec3 DEFAULT_DIFFUSE(1.0f);
    const my_glm::vec3 DEFAULT_AMBIENT(1.0f);
    const my_glm::vec3 DEFAULT_SPECULAR(0.0f);
    const my_glm::vec3 DEFAULT_EMISSIVE(0.0f);
    const float DEFAULT_SPECULAR_EXP = 1.0f;
    const float DEFAULT_DISSOLVE = 1.0f;
    const float DEFAULT_REFRACTION = 1.0f;
    const int DEFAULT_ILLUM = 2;

    // Apply material properties
    if (materialData.textures.count(materialName)) {
        subMesh.textureFile = materialData.textures.at(materialName);
    }
    
    subMesh.diffuseColor = materialData.diffuseColors.count(materialName) 
                           ? materialData.diffuseColors.at(materialName) 
                           : DEFAULT_DIFFUSE;
    
    subMesh.ambientColor = materialData.ambientColors.count(materialName) 
                           ? materialData.ambientColors.at(materialName) 
                           : DEFAULT_AMBIENT;
    
    subMesh.specularColor = materialData.specularColors.count(materialName) 
                            ? materialData.specularColors.at(materialName) 
                            : DEFAULT_SPECULAR;
    
    subMesh.emissiveColor = materialData.emissiveColors.count(materialName) 
                            ? materialData.emissiveColors.at(materialName) 
                            : DEFAULT_EMISSIVE;
    
    subMesh.specularExponent = materialData.specularExponents.count(materialName) 
                               ? materialData.specularExponents.at(materialName) 
                               : DEFAULT_SPECULAR_EXP;
    
    subMesh.dissolve = materialData.dissolves.count(materialName) 
                       ? materialData.dissolves.at(materialName) 
                       : DEFAULT_DISSOLVE;
    
    subMesh.refractionIndex = materialData.refractionIndices.count(materialName) 
                              ? materialData.refractionIndices.at(materialName) 
                              : DEFAULT_REFRACTION;
    
    subMesh.illumModel = materialData.illumModels.count(materialName) 
                         ? materialData.illumModels.at(materialName) 
                         : DEFAULT_ILLUM;
}

// ============================================================================
// Main OBJ Loader
// ============================================================================

bool loadOBJ(const std::string& filename, std::vector<SubMesh>& submeshes) {
    // Validate filename
    if (!isValidOBJFilename(filename)) {
        std::cerr << "Filename is invalid." << std::endl;
        return false;
    }

    // Open OBJ file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return false;
    }

    // Temporary storage for OBJ data
    std::vector<my_glm::vec3> temp_positions;
    std::vector<my_glm::vec3> temp_normals;
    std::vector<my_glm::vec2> temp_texcoords;
    std::unordered_map<Vertex, uint32_t, VertexHash> uniqueVertices;
    
    // Material data
    MaterialData materialData;
    std::string currentMaterial;
    SubMesh currentSubMesh;

    // Parse OBJ file line by line
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            parseVerticesLine(iss, temp_positions);
        } 
        else if (prefix == "vt") {
            parseTextureCoordinatesLine(iss, temp_texcoords);
        } 
        else if (prefix == "vn") {
            parseVertexNormalLine(iss, temp_normals);
        } 
        else if (prefix == "mtllib") {
            std::string mtlFilename;
            iss >> mtlFilename;
            parseMaterialFile(mtlFilename, materialData, currentSubMesh.hasMapKdInitially);
        } 
        else if (prefix == "usemtl") {
            // Save current submesh if it has vertices
            if (!currentSubMesh.vertices.empty()) {
                submeshes.push_back(currentSubMesh);
                currentSubMesh = SubMesh();
                uniqueVertices.clear();
            }
            
            // Apply new material
            iss >> currentMaterial;
            applyMaterialToSubMesh(currentSubMesh, currentMaterial, materialData);
        } 
        else if (prefix == "f") {
            parseFaceLine(iss, temp_positions, temp_normals, temp_texcoords,
                          currentSubMesh.vertices, currentSubMesh.indices, uniqueVertices);
        }
    }

    // Add the last submesh if it has vertices
    if (!currentSubMesh.vertices.empty()) {
        submeshes.push_back(currentSubMesh);
    }

    return true;
}
