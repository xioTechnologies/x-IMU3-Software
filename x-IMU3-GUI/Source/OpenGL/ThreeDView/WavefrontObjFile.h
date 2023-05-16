#pragma once

#include "glm/common.hpp"
#include "glm/gtx/component_wise.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <unordered_map>

class WavefrontObjFile // From JUCE example
{
public:
    WavefrontObjFile()
    {
    }

    juce::Result load(const juce::String& objFileContent, const juce::String& mtlFileContent)
    {
        return parseObjFile(juce::StringArray::fromLines(objFileContent), juce::StringArray::fromLines(mtlFileContent));
    }

    juce::Result load(const juce::File& file)
    {
        sourceFile = file;
        return load(file.loadFileAsString(), {});
    }

    //==============================================================================
    typedef GLuint Index;

    struct Mesh
    {
        juce::Array<glm::vec3> positions, normals;
        juce::Array<glm::vec2> textureCoords;

        // Contains data from positions, normals, and texture coordinates interleaved for all vertices for easy use in OpenGL buffers
        std::vector<GLfloat> vertices;

        // Indexes of vertices ordered for OpenGL rendering by triangles
        juce::Array<Index> indices;
    };

    struct Material
    {
        Material() noexcept
        {
        }

        juce::String name;

        glm::vec3 ambient { 0.25f, 0.25f, 0.25f };
        glm::vec3 diffuse { 0.4f, 0.4f, 0.4f };
        glm::vec3 specular { 0.774597f, 0.774597f, 0.774597f };
        glm::vec3 transmittance, emission;

        float shininess = 0.6f * 128.0f;
        float refractiveIndex = 0.0f;

        juce::String ambientTextureName, diffuseTextureName,
                specularTextureName, normalTextureName;

        juce::StringPairArray parameters;
    };

    struct Shape
    {
        juce::String name;
        Mesh mesh;
        Material material;
    };

    juce::OwnedArray<Shape> shapes;

private:
    //==============================================================================
    juce::File sourceFile;

    struct TripleIndex
    {
        int vertexIndex = -1, textureIndex = -1, normalIndex = -1;

        TripleIndex() noexcept
        {
        }

        bool operator==(const TripleIndex& other) const
        {
            return vertexIndex == other.vertexIndex && textureIndex == other.textureIndex && normalIndex == other.normalIndex;
        }

        struct Hash
        {
            size_t operator()(const TripleIndex& k) const
            {
                size_t h1 = std::hash<int>()(k.vertexIndex);
                size_t h2 = std::hash<int>()(k.textureIndex);
                size_t h3 = std::hash<int>()(k.normalIndex);
                return (h1 ^ (h2 << 1)) ^ h3;
            }
        };
    };

    struct IndexMap
    {
        std::unordered_map<const TripleIndex, Index, TripleIndex::Hash> map;

        Index getIndexFor(TripleIndex i, Mesh& newMesh, const Mesh& srcMesh)
        {
            if (const auto it(map.find(i)); it != map.end())
            {
                return it->second;
            }

            auto index = (Index) newMesh.positions.size();

            if (i.vertexIndex < 0)
            {
                i.vertexIndex = srcMesh.positions.size() + i.vertexIndex + 1;
            }

            if (juce::isPositiveAndBelow(i.vertexIndex, srcMesh.positions.size()))
            {
                newMesh.positions.add(srcMesh.positions.getReference(i.vertexIndex));
            }

            if (i.normalIndex < 0)
            {
                i.normalIndex = srcMesh.normals.size() + i.normalIndex + 1;
            }

            if (juce::isPositiveAndBelow(i.normalIndex, srcMesh.normals.size()))
            {
                newMesh.normals.add(srcMesh.normals.getReference(i.normalIndex));
            }

            if (i.textureIndex < 0)
            {
                i.textureIndex = srcMesh.textureCoords.size() + i.textureIndex + 1;
            }

            if (juce::isPositiveAndBelow(i.textureIndex, srcMesh.textureCoords.size()))
            {
                newMesh.textureCoords.add(srcMesh.textureCoords.getReference(i.textureIndex));
            }

            map[i] = index;
            return index;
        }
    };

    static float parseFloat(juce::String::CharPointerType& t)
    {
        t.incrementToEndOfWhitespace();
        return (float) juce::CharacterFunctions::readDoubleValue(t);
    }

    static glm::vec3 parseVertex(juce::String::CharPointerType t)
    {
        glm::vec3 v;
        v.x = parseFloat(t);
        v.y = parseFloat(t);
        v.z = parseFloat(t);
        return v;
    }

    static glm::vec2 parseTextureCoord(juce::String::CharPointerType t)
    {
        glm::vec2 tc;
        tc.x = parseFloat(t);
        tc.y = parseFloat(t);
        return tc;
    }

    static bool matchToken(juce::String::CharPointerType& t, const char* token)
    {
        auto len = (int) strlen(token);

        if (juce::CharacterFunctions::compareUpTo(juce::CharPointer_ASCII(token), t, len) == 0)
        {
            auto end = t + len;

            if (end.isEmpty() || end.isWhitespace())
            {
                t = end.findEndOfWhitespace();
                return true;
            }
        }

        return false;
    }

    struct Face
    {
        Face(juce::String::CharPointerType t)
        {
            while (!t.isEmpty())
                triples.add(parseTriple(t));
        }

        juce::Array<TripleIndex> triples;

        void addIndices(Mesh& newMesh, const Mesh& srcMesh, IndexMap& indexMap)
        {
            TripleIndex i0(triples[0]), i1, i2(triples[1]);

            for (auto i = 2; i < triples.size(); ++i)
            {
                i1 = i2;
                i2 = triples.getReference(i);

                newMesh.indices.add(indexMap.getIndexFor(i0, newMesh, srcMesh));
                newMesh.indices.add(indexMap.getIndexFor(i1, newMesh, srcMesh));
                newMesh.indices.add(indexMap.getIndexFor(i2, newMesh, srcMesh));
            }
        }

        static TripleIndex parseTriple(juce::String::CharPointerType& t)
        {
            TripleIndex i;

            t.incrementToEndOfWhitespace();
            i.vertexIndex = t.getIntValue32() - 1;
            t = findEndOfFaceToken(t);

            if (t.isEmpty() || t.getAndAdvance() != '/')
            {
                return i;
            }

            if (*t == '/')
            {
                ++t;
            }
            else
            {
                i.textureIndex = t.getIntValue32() - 1;
                t = findEndOfFaceToken(t);

                if (t.isEmpty() || t.getAndAdvance() != '/')
                {
                    return i;
                }
            }

            i.normalIndex = t.getIntValue32() - 1;
            t = findEndOfFaceToken(t);
            return i;
        }

        static juce::String::CharPointerType findEndOfFaceToken(juce::String::CharPointerType t) noexcept
        {
            return juce::CharacterFunctions::findEndOfToken(t, juce::CharPointer_ASCII("/ \t"), juce::String().getCharPointer());
        }
    };

    static Shape* parseFaceGroup(const Mesh& srcMesh,
                                 juce::Array<Face>& faceGroup,
                                 const Material& material,
                                 const juce::String& name)
    {
        if (faceGroup.size() == 0)
        {
            return nullptr;
        }

        auto shape = std::make_unique<Shape>();
        shape->name = name;
        shape->material = material;

        IndexMap indexMap;

        for (auto& f : faceGroup)
            f.addIndices(shape->mesh, srcMesh, indexMap);

        if (shape->mesh.indices.isEmpty() || shape->mesh.positions.isEmpty())
        {
            return nullptr;
        }

        return shape.release();
    }

    juce::Result parseObjFile(const juce::StringArray& objLines, const juce::StringArray& mtlLines)
    {
        Mesh mesh;
        juce::Array<Face> faceGroup;

        // Material names mapped to material objects
        std::unordered_map<std::string, Material> knownMaterials;
        Material lastMaterial;
        juce::String lastName;

        for (auto lineNum = 0; lineNum < objLines.size(); ++lineNum)
        {
            auto l = objLines[lineNum].getCharPointer().findEndOfWhitespace();

            if (matchToken(l, "v"))
            {
                mesh.positions.add(parseVertex(l));
                continue;
            }
            if (matchToken(l, "vn"))
            {
                mesh.normals.add(parseVertex(l));
                continue;
            }
            if (matchToken(l, "vt"))
            {
                mesh.textureCoords.add(parseTextureCoord(l));
                continue;
            }
            if (matchToken(l, "f"))
            {
                faceGroup.add(Face(l));
                continue;
            }

            if (matchToken(l, "usemtl"))
            {
                // Make a new shape from the last material and clear to begin putting a new shape together
                if (auto* shape = parseFaceGroup(mesh, faceGroup, lastMaterial, lastName))
                {
                    shapes.add(shape);
                }
                faceGroup.clear();

                // If new material exists, reference it, otherwise keep using last material
                auto materialName = juce::String(l).trim();
                auto foundMaterial = knownMaterials.find(materialName.toStdString());
                if (foundMaterial != knownMaterials.end())
                {
                    lastMaterial = foundMaterial->second;
                }

                continue;
            }

            if (matchToken(l, "mtllib"))
            {
                auto r = parseMaterial(knownMaterials, juce::String(l).trim(), mtlLines);
                continue;
            }

            if (matchToken(l, "g") || matchToken(l, "o"))
            {
                if (auto* shape = parseFaceGroup(mesh, faceGroup, lastMaterial, lastName))
                {
                    shapes.add(shape);
                }
                faceGroup.clear();

                lastName = juce::StringArray::fromTokens(l, " \t", "")[0];
                continue;
            }
        }

        if (auto* shape = parseFaceGroup(mesh, faceGroup, lastMaterial, lastName))
        {
            shapes.add(shape);
        }

        normalisePositions();

        interleaveVertexData();

        return juce::Result::ok();
    }

    juce::Result parseMaterial(std::unordered_map<std::string, Material>& materials, const juce::String& filename, juce::StringArray lines)
    {
        if (lines.isEmpty())
        {
            jassert(sourceFile.exists());
            auto f = sourceFile.getSiblingFile(filename);

            if (!f.exists())
            {
                return juce::Result::fail("Cannot open file: " + filename);
            }

            lines = juce::StringArray::fromLines(f.loadFileAsString());
        }

        materials.clear();
        Material material;

        for (auto line : lines)
        {
            auto l = line.getCharPointer().findEndOfWhitespace();

            if (matchToken(l, "newmtl"))
            {
                materials.insert({ material.name.toStdString(), material });
                material.name = juce::String(l).trim();
                continue;
            }

            if (matchToken(l, "Ka"))
            {
                material.ambient = parseVertex(l);
                continue;
            }
            if (matchToken(l, "Kd"))
            {
                material.diffuse = parseVertex(l);
                continue;
            }
            if (matchToken(l, "Ks"))
            {
                material.specular = parseVertex(l);
                continue;
            }
            if (matchToken(l, "Kt"))
            {
                material.transmittance = parseVertex(l);
                continue;
            }
            if (matchToken(l, "Ke"))
            {
                material.emission = parseVertex(l);
                continue;
            }
            if (matchToken(l, "Ni"))
            {
                material.refractiveIndex = parseFloat(l);
                continue;
            }
            if (matchToken(l, "Ns"))
            {
                material.shininess = parseFloat(l);
                continue;
            }

            if (matchToken(l, "map_Ka"))
            {
                material.ambientTextureName = juce::String(l).trim();
                continue;
            }
            if (matchToken(l, "map_Kd"))
            {
                material.diffuseTextureName = juce::String(l).trim();
                continue;
            }
            if (matchToken(l, "map_Ks"))
            {
                material.specularTextureName = juce::String(l).trim();
                continue;
            }
            if (matchToken(l, "map_Ns"))
            {
                material.normalTextureName = juce::String(l).trim();
                continue;
            }

            auto tokens = juce::StringArray::fromTokens(l, " \t", "");

            if (tokens.size() >= 2)
            {
                material.parameters.set(tokens[0].trim(), tokens[1].trim());
            }
        }

        // NOTE: Seems odd this would be here. . .
        materials.insert({ material.name.toStdString(), material });
        return juce::Result::ok();
    }

    // Normalizes vertex positions such that x,y,z coordinates of each position is in the range of -1.0 to 1.0 (inclusive)
    void normalisePositions()
    {
        GLfloat maxMagnitudeComponent = 0.0f;
        for (const auto& shape : shapes)
        {
            for (const auto& position : shape->mesh.positions)
            {
                maxMagnitudeComponent = glm::max(glm::compMax(glm::abs(position)), maxMagnitudeComponent);
            }
        }

        // Prevent divide by 0 error
        if (maxMagnitudeComponent == 0.0f)
        {
            jassertfalse; // you tried importing & normalizing a mesh with no positions other than the origin (0.0, 0.0, 0.0)
            return;
        }

        // Vertex positions are already normalized in this case, do not modify them
        if (maxMagnitudeComponent == 1.0f)
        {
            return;
        }

        // Scale all positions uniformly so largest position becomes the bound of one of the sides of the -1.0 to 1.0 range
        for (auto& shape : shapes)
        {
            for (auto& position : shape->mesh.positions)
            {
                position /= maxMagnitudeComponent;
            }
        }
    }

    void interleaveVertexData()
    {
        // Interleave vertex data for all shapes in Mesh -> vertices
        for (const auto& shape : shapes)
        {
            auto& mesh = shape->mesh;
            mesh.vertices.clear(); // pre-clear interleaved vertices so we can fill them

            // All .obj files must have vertex positions
            if (mesh.positions.size() <= 0)
            {
                jassertfalse;
                return;
            }

            // Normals and texture coordinates may optionally exist in a .obj file,
            // but if they are present, there must be the same number of these as positions
            const bool hasNormals = mesh.normals.size() > 0 && mesh.normals.size() == mesh.positions.size();
            const bool hasTextureCoords = mesh.textureCoords.size() > 0 && mesh.textureCoords.size() == mesh.positions.size();

            for (int vertexIndex = 0; vertexIndex < mesh.positions.size(); ++vertexIndex)
            {
                auto position = mesh.positions[vertexIndex];
                mesh.vertices.push_back(position.x);
                mesh.vertices.push_back(position.y);
                mesh.vertices.push_back(position.z);

                auto normal = hasNormals ? mesh.normals[vertexIndex] : glm::vec3 { 0.0f, 0.0f, 0.0f };
                mesh.vertices.push_back(normal.x);
                mesh.vertices.push_back(normal.y);
                mesh.vertices.push_back(normal.z);

                auto textureCoordinate = hasTextureCoords ? mesh.textureCoords[vertexIndex] : glm::vec2 { 0.0f, 0.0f };
                mesh.vertices.push_back(textureCoordinate.x);
                mesh.vertices.push_back(textureCoordinate.y);
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavefrontObjFile)
};
