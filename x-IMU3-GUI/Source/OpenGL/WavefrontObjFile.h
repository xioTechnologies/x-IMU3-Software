#pragma once

#include <unordered_map>

struct MeshColour
{
    GLfloat r = 0.0f;
    GLfloat g = 0.0f;
    GLfloat b = 0.0f;
    GLfloat a = 0.0f;
};

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
    typedef juce::uint32 Index;

    struct Vertex
    {
        float x, y, z;
    };
    struct TextureCoord
    {
        float x, y;
    };

    struct Mesh
    {
        juce::Array<Vertex> vertices, normals;
        juce::Array<TextureCoord> textureCoords;
        juce::Array<Index> indices;
        juce::Array<MeshColour> colours;
    };

    struct Material
    {
        Material() noexcept
        {
        }

        juce::String name;

        Vertex ambient { 0.25f, 0.25f, 0.25f };
        Vertex diffuse { 0.4f, 0.4f, 0.4f };
        Vertex specular { 0.774597f, 0.774597f, 0.774597f };
        Vertex transmittance, emission;

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

            auto index = (Index) newMesh.vertices.size();

            if (i.vertexIndex < 0)
            {
                i.vertexIndex = srcMesh.vertices.size() + i.vertexIndex + 1;
            }

            if (juce::isPositiveAndBelow(i.vertexIndex, srcMesh.vertices.size()))
            {
                newMesh.vertices.add(srcMesh.vertices.getReference(i.vertexIndex));
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

    static Vertex parseVertex(juce::String::CharPointerType t)
    {
        Vertex v;
        v.x = parseFloat(t);
        v.y = parseFloat(t);
        v.z = parseFloat(t);
        return v;
    }

    static TextureCoord parseTextureCoord(juce::String::CharPointerType t)
    {
        TextureCoord tc;
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

        std::unique_ptr<Shape> shape(new Shape());
        shape->name = name;
        shape->material = material;

        IndexMap indexMap;

        for (auto& f : faceGroup)
            f.addIndices(shape->mesh, srcMesh, indexMap);

        if (shape->mesh.indices.isEmpty() || shape->mesh.vertices.isEmpty())
        {
            return nullptr;
        }

        return shape.release();
    }

    juce::Result parseObjFile(const juce::StringArray& objLines, const juce::StringArray& mtlLines)
    {
        Mesh mesh;
        juce::Array<Face> faceGroup;

        juce::Array<Material> knownMaterials;
        Material lastMaterial;
        juce::String lastName;

        for (auto lineNum = 0; lineNum < objLines.size(); ++lineNum)
        {
            if (juce::Thread::currentThreadShouldExit())
            {
                return juce::Result::fail("");
            }

            auto l = objLines[lineNum].getCharPointer().findEndOfWhitespace();

            if (matchToken(l, "v"))
            {
                mesh.vertices.add(parseVertex(l));
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
                auto name = juce::String(l).trim();

                for (auto i = knownMaterials.size(); --i >= 0;)
                {
                    if (knownMaterials.getReference(i).name == name)
                    {
                        lastMaterial = knownMaterials.getReference(i);
                        break;
                    }
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
            if (juce::Thread::currentThreadShouldExit())
            {
                return juce::Result::fail("");
            }

            shapes.add(shape);
        }

        normalise();

        return juce::Result::ok();
    }

    juce::Result parseMaterial(juce::Array<Material>& materials, const juce::String& filename, juce::StringArray lines)
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
                materials.add(material);
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

        materials.add(material);
        return juce::Result::ok();
    }

    void normalise()
    {
        Vertex minValues { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
        Vertex maxValues { std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

        for (const auto& shape : shapes)
        {
            for (const auto& vertex : shape->mesh.vertices)
            {
                minValues.x = std::min(minValues.x, vertex.x);
                minValues.y = std::min(minValues.y, vertex.y);
                minValues.z = std::min(minValues.z, vertex.z);

                maxValues.x = std::max(maxValues.x, vertex.x);
                maxValues.y = std::max(maxValues.y, vertex.y);
                maxValues.z = std::max(maxValues.z, vertex.z);
            }
        }

        const Vertex length { maxValues.x - minValues.x, maxValues.y - minValues.y, maxValues.z - minValues.z };

        const auto max = std::max(std::max(length.x, length.y), length.z);

        if (max == 0.0f)
        {
            return;
        }

        const auto fraction = 1.0f / max;

        for (auto& shape : shapes)
        {
            for (auto& vertex : shape->mesh.vertices)
            {
                vertex.x *= fraction;
                vertex.y *= fraction;
                vertex.z *= fraction;
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavefrontObjFile)
};
