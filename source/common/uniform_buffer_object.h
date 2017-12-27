#pragma once

#include "glm/glm.hpp"

namespace VulkanLearning
{

class UniformBufferObject
{
public:
    UniformBufferObject()
    {}

    UniformBufferObject(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) :
        model(model),
        view(view),
        projection(projection)
    {}

    void setModel(const glm::mat4& model)
    {
        this->model = model;
    }

    void setView(const glm::mat4& view)
    {
        this->view = view;
    }

    void setProjection(const glm::mat4& projection)
    {
        this->projection = projection;
    }

    glm::mat4 getModel() const
    {
        return model;
    }

    glm::mat4 getView() const
    {
        return view;
    }
    glm::mat4 getProjection() const
    {
        return projection;
    }

private:
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

} // namespace VulkanLearning
