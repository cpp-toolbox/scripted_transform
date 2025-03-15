#ifndef SCRIPTED_TRANSFORM_HPP
#define SCRIPTED_TRANSFORM_HPP

#include "sbpt_generated_includes.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

class ScriptedTransform {
  public:
    ScriptedTransform(double duration, float tau);

    void update(double ms_curr_time);

    int get_num_keyframes() const;
    Transform get_keyframe(int i) const;
    Transform interpolate_transform(double delta_time);
    void set_keyframe(int i, Transform point);
    void insert_keyframe(int i, Transform transform);
    void append_keyframe(Transform transform);
    void delete_keyframe(int i);

    Transform transform;

    // private:
    CatmullRomInterpolator interpolator_position;
    CatmullRomInterpolator interpolator_rotation;
    CatmullRomInterpolator interpolator_scale;
};

#endif // SCRIPTED_TRANSFORM_HPP
