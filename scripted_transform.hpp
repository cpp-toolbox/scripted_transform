#ifndef SCRIPTED_TRANSFORM_HPP
#define SCRIPTED_TRANSFORM_HPP

#include "sbpt_generated_includes.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

class ScriptedTransform {
  public:
    // TODO: instead of start and end time I think we should just have a duration, and then just start it and stop it
    ScriptedTransform(std::vector<Transform> keyframes, double ms_start_time, double ms_end_time, float tau);

    void update(double ms_curr_time);
    void insert_keyframe(int i, Transform transform);
    void append_keyframe(Transform transform);
    void delete_keyframe(int i);
    void update_keyframe(int i, Transform point);

    Transform transform;

    // private:
    CatmullRomInterpolator interpolator_position;
    CatmullRomInterpolator interpolator_rotation;
    CatmullRomInterpolator interpolator_scale;
};

#endif // SCRIPTED_TRANSFORM_HPP
