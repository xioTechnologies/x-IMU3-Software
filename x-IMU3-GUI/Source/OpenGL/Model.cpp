#include "GLResources.h"
#include "Model.h"

Model::Model(juce::OpenGLContext& context_) : context(context_)
{
}

void Model::setColour(const juce::Colour& colour_)
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
            colour = { colour_.getFloatRed(), colour_.getFloatGreen(), colour_.getFloatBlue(), colour_.getFloatAlpha() };
        }

        buffers[i]->fillVbo(Buffer::colourBuffer, &mesh.colours.getReference(0).r, GLsizeiptr((size_t) mesh.colours.size() * sizeof(MeshColour)));
    }
}

void Model::render(const GLResources& resources, bool isTextured)
{
    std::lock_guard _(objectLock);
    if (object != nullptr && fillBuffersPending)
    {
        fillBuffers();
        fillBuffersPending = false;
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
    loading = true;
    juce::Thread::launch([&, self = juce::WeakReference(this), objFileContent, mtlFileContent]
                         {
                             auto newObject = std::make_shared<WavefrontObjFile>();
                             newObject->load(objFileContent, mtlFileContent);

                             juce::MessageManager::callAsync([&, self, newObject]() mutable
                                                             {
                                                                 if (self == nullptr)
                                                                 {
                                                                     return;
                                                                 }

                                                                 std::lock_guard _(objectLock);
                                                                 std::swap(object, newObject);
                                                                 fillBuffersPending = true;
                                                                 loading = false;
                                                             });
                         });
}

void Model::setModel(const juce::File& objFile)
{
    loading = true;
    juce::Thread::launch([&, self = juce::WeakReference(this), objFile]
                         {
                             auto newObject = std::make_shared<WavefrontObjFile>();
                             newObject->load(objFile);

                             juce::MessageManager::callAsync([&, self, newObject]() mutable
                                                             {
                                                                 if (self == nullptr)
                                                                 {
                                                                     return;
                                                                 }

                                                                 std::lock_guard _(objectLock);
                                                                 std::swap(object, newObject);
                                                                 fillBuffersPending = true;
                                                                 loading = false;
                                                             });
                         });
}

bool Model::isLoading() const
{
    return loading;
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
