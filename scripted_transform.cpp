#include "scripted_transform.hpp"

ScriptedTransform::ScriptedTransform(std::vector<ScriptedTransformKeyframe> keyframes, double ms_start_time,
                                     double ms_end_time, float tau)
    : transform{},
      // bad
      interpolator_position{
          {glm::vec3(0.0), glm::vec3(1.0), glm::vec3(2.0), glm::vec3(3.0)}, ms_start_time, ms_end_time, tau},
      interpolator_rotation{
          {glm::vec3(0.0), glm::vec3(1.0), glm::vec3(2.0), glm::vec3(3.0)}, ms_start_time, ms_end_time, tau},
      interpolator_scale{
          {glm::vec3(0.0), glm::vec3(1.0), glm::vec3(2.0), glm::vec3(3.0)}, ms_start_time, ms_end_time, tau} {

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> rotations;
    std::vector<glm::vec3> scales;

    for (const auto &keyframe : keyframes) {
        positions.push_back(keyframe.position);
        rotations.push_back(keyframe.rotation);
        scales.push_back(keyframe.scale);
    }

    // use pointers to avoid having to implement a default constructor for CatmullRomInterpolator
    interpolator_position = CatmullRomInterpolator(positions, ms_start_time, ms_end_time, tau);
    interpolator_rotation = CatmullRomInterpolator(rotations, ms_start_time, ms_end_time, tau);
    interpolator_scale = CatmullRomInterpolator(scales, ms_start_time, ms_end_time, tau);
}

void ScriptedTransform::update(double ms_curr_time) {
    transform.position = interpolator_position.interpolate(ms_curr_time);
    transform.rotation = interpolator_rotation.interpolate(ms_curr_time);
    transform.scale = interpolator_scale.interpolate(ms_curr_time);
}

// localize edits????
// TODO: for next time, do interpolation for rotation and scale and maybe extract catmull-rom to own class?
// make tension and control points editable
// visualize rotation at control points
// export spline
// loop option flag
// follow cam for player
