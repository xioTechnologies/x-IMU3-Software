#include "GLResources.h"
#include "Model.h"

class Model::ObjectLoader : private juce::Thread
{
public:
    ObjectLoader(const juce::String& objFileContent_, const juce::String& mtlFileContent_)
            : juce::Thread("Object Loader"),
              objFileContent(objFileContent_),
              mtlFileContent(mtlFileContent_)
    {
        startThread();
    }

    ObjectLoader(const juce::File& objFile_)
            : juce::Thread("Object Loader"),
              objFile(objFile_)
    {
        startThread();
    }

    ~ObjectLoader() override
    {
        stopThread(5000); // allow some time for model loader to stop
    }

    std::unique_ptr<WavefrontObjFile> getLoadedObject()
    {
        if (isThreadRunning())
        {
            return nullptr;
        }

        return std::move(object);
    }

private:
    const juce::String objFileContent {}, mtlFileContent {};
    const juce::File objFile {};

    std::unique_ptr<WavefrontObjFile> object = std::make_unique<WavefrontObjFile>();

    void run() override
    {
        if (objFileContent.isNotEmpty())
        {
            object->load(objFileContent, mtlFileContent);
        }
        else
        {
            object->load(objFile);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjectLoader)
};

Model::Model(juce::OpenGLContext& context_) : context(context_)
{
}

Model::~Model()
{
}

void Model::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    if (object == nullptr)
    {
        return;
    }

    for (size_t i = 0; i < (size_t) object->shapes.size(); i++)
    {
        auto& mesh = object->shapes[(int) i]->mesh;
        if (mesh.colours.isEmpty())
        {
            continue;
        }

        for (auto& colour : mesh.colours)
        {
            colour = { r, g, b, a };
        }

        buffers[i]->fillVbo(Buffer::colourBuffer, &mesh.colours.getReference(0).r, GLsizeiptr((size_t) mesh.colours.size() * sizeof(MeshColour)));
    }
}

void Model::render(const GLResources& resources, bool isTextured)
{
    if (std::unique_lock lock(objectLoaderLock); objectLoader != nullptr)
    {
        if (object = objectLoader->getLoadedObject(); object != nullptr)
        {
            objectLoader.reset();
            lock.unlock();
            fillBuffers();
        }
        else
        {
            return;
        }
    }

    //TODO - We need to figure out a way to switch between two lighting shaders
    //One shader will use materials from a .mtl file. The other shader doesn't use materials
    for (size_t index = 0; index < buffers.size(); index++)
    {
        auto& buffer = buffers[index];
        buffer->linkVbo(resources.threeDViewShader.vertexIn.attributeID, Buffer::vertexBuffer, Buffer::Xyz, Buffer::floatingPoint);
        buffer->linkVbo(resources.threeDViewShader.colourIn.attributeID, Buffer::colourBuffer, Buffer::Rgba, Buffer::floatingPoint);
        buffer->linkVbo(resources.threeDViewShader.normalIn.attributeID, Buffer::normalBuffer, Buffer::Xyz, Buffer::floatingPoint);

        if (isTextured)
        {
            buffer->linkVbo(resources.threeDViewShader.textureIn.attributeID, Buffer::textureBuffer, Buffer::UV, Buffer::floatingPoint);
        }

        //TODO - This is only used to link shader variables if we are using an MTL file
        //If this block is enabled then we have lighting issues when using integrated graphics
        /*auto& material = object->shapes[(int) index]->material;
        resources.threeDViewShaderWithMaterial.materialAmbient.set(material.ambient.x, material.ambient.y, material.ambient.z);
        resources.threeDViewShaderWithMaterial.materialDiffuse.set(material.diffuse.x, material.diffuse.y, material.diffuse.z);
        resources.threeDViewShaderWithMaterial.materialSpecular.set(material.specular.x, material.specular.y, material.specular.z);
        resources.threeDViewShaderWithMaterial.materialShininess.set(material.shininess);*/

        buffer->render(Buffer::triangles);
    }
}

void Model::setModel(const juce::String& objFileContent, const juce::String& mtlFileContent)
{
    auto newObjectLoader = std::make_unique<ObjectLoader>(objFileContent, mtlFileContent);

    std::lock_guard _(objectLoaderLock);
    std::swap(objectLoader, newObjectLoader);
}

void Model::setModel(const juce::File& objFile)
{
    auto newObjectLoader = std::make_unique<ObjectLoader>(objFile);

    std::lock_guard _(objectLoaderLock);
    std::swap(objectLoader, newObjectLoader);
}

bool Model::isLoading() const
{
    std::lock_guard _(objectLoaderLock);
    return objectLoader != nullptr;
}

void Model::fillBuffers()
{
    buffers.clear();

    for (auto& shape : object->shapes)
    {
        auto& mesh = shape->mesh;

        auto buffer = std::make_unique<Buffer>(context);
        buffer->create((GLuint) mesh.indices.size(), true);

        buffer->fillEbo(&mesh.indices.getReference(0), (GLsizeiptr) ((size_t) mesh.indices.size() * sizeof(GLuint)));
        buffer->fillVbo(Buffer::vertexBuffer, &mesh.vertices.getReference(0).x, (GLsizeiptr) ((size_t) mesh.vertices.size() * sizeof(juce::Vector3D<GLfloat>)));

        if (!mesh.normals.isEmpty())
        {
            buffer->fillVbo(Buffer::normalBuffer, &mesh.normals.getReference(0).x, (GLsizeiptr) ((size_t) mesh.normals.size() * sizeof(juce::Vector3D<GLfloat>)));
        }

        for (auto& colours : mesh.vertices)
        {
            juce::ignoreUnused(colours);
            mesh.colours.add({ 1.0f, 1.0f, 1.0f, 1.0f });
        }

        if (!mesh.colours.isEmpty())
        {
            buffer->fillVbo(Buffer::colourBuffer, &mesh.colours.getReference(0).r, (GLsizeiptr) ((size_t) mesh.colours.size() * sizeof(MeshColour)));
        }

        if (!mesh.textureCoords.isEmpty())
        {
            buffer->fillVbo(Buffer::textureBuffer, &mesh.textureCoords.getReference(0).x, GLsizeiptr((size_t) mesh.textureCoords.size() * sizeof(juce::Point<GLfloat>)));
        }

        buffers.push_back(std::move(buffer));
    }
}
