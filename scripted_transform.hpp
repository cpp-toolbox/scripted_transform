#ifndef SCRIPTED_TRANSFORM_HPP
#define SCRIPTED_TRANSFORM_HPP

#include "sbpt_generated_includes.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

struct ScriptedTransformKeyframe {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

class ScriptedTransform {
  public:
    ScriptedTransform(std::vector<ScriptedTransformKeyframe> keyframes, double ms_start_time, double ms_end_time,
                      float tau);

    void update(double ms_curr_time);

    Transform transform;

    // private:
    CatmullRomInterpolator interpolator_position;
    CatmullRomInterpolator interpolator_rotation;
    CatmullRomInterpolator interpolator_scale;
};

#endif // SCRIPTED_TRANSFORM_HPP
