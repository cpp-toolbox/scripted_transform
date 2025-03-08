#include "scripted_transform.hpp"
#include <chrono>

ScriptedTransform::ScriptedTransform(double duration, float tau)
    : transform{}, interpolator_position{duration, tau}, interpolator_rotation{duration, tau},
      interpolator_scale{duration, tau} {}

int ScriptedTransform::get_num_keyframes() const { return interpolator_position.get_num_points(); }

Transform ScriptedTransform::get_keyframe(int i) const {
    Transform transform;
    transform.position = interpolator_position.get_point(i);
    transform.rotation = interpolator_rotation.get_point(i);
    transform.scale = interpolator_scale.get_point(i);
    return transform;
}

void ScriptedTransform::update(double deltaTime) {
    transform.position = interpolator_position.interpolate(deltaTime);
    transform.rotation = interpolator_rotation.interpolate(deltaTime);
    transform.scale = interpolator_scale.interpolate(deltaTime);
}

void ScriptedTransform::insert_keyframe(int i, Transform transform) {
    interpolator_position.insert_point(i, transform.position);
    interpolator_rotation.insert_point(i, transform.rotation);
    interpolator_scale.insert_point(i, transform.scale);
}

void ScriptedTransform::append_keyframe(Transform transform) {
    interpolator_position.append_point(transform.position);
    interpolator_rotation.append_point(transform.rotation);
    interpolator_scale.append_point(transform.scale);
}

void ScriptedTransform::delete_keyframe(int i) {
    interpolator_position.delete_point(i);
    interpolator_rotation.delete_point(i);
    interpolator_scale.delete_point(i);
}
void ScriptedTransform::set_keyframe(int i, Transform transform) {
    interpolator_position.set_point(i, transform.position);
    interpolator_rotation.set_point(i, transform.rotation);
    interpolator_scale.set_point(i, transform.scale);
}

// localize edits????
// TODO: for next time, do interpolation for rotation and scale and maybe extract catmull-rom to own class?
// make tension and control points editable
// visualize rotation at control points
// export spline
// loop option flag
// follow cam for player
