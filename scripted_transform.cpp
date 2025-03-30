#include "scripted_transform.hpp"
#include <chrono>

ScriptedTransform::ScriptedTransform(double duration, float tau)
    : transform{}, interpolator_position{duration, tau}, interpolator_rotation{duration, tau},
      interpolator_scale{duration, tau}, duration{duration}, interpolator_time_v_arc_length_rotation{duration, 0.5},
      interpolator_time_v_arc_length_scale{duration, 0.5} {}

// travel at a constant velocity for one spline, then another
/**
 * To smoothly interpolate between control points while also reaching the control points at a specific
 * time, you basically have the constraint where you need to have travelled certain arc lengths at
 * certain times.
 *
 * This gives you a set of pairs (time1, arclength1 [of rotation]), (time2, arclength2 [of rotation]), ...
 * Now, imagine these pairs are plotted.
 * There are multiple ways you can interpolate between these points. But try Catmull-Rom again.
 * This Catmull-Rom spline gives you time -> arc length you have traversed (so you know where on the curve you are)
 * Then, you have the arc length vs. time smooth interpolated graph.
 * The derivative of this for the speed vs. time graph will be continuous so good.
 *
 * Catmull Rom DOES GIVE YOU the arc length you should've traversed up until that point.
 * 
 * 
 * suppose you are working in 2d and doing catmull rom as you see in the pdf, the resulting spline is not
 * scale invariant, the reason why is simple if you have a collection of points that you wish to run catmull rom
 * spline on then when you compute before scaling you have p{i} - p_{i - 2} and after scaling you have alpha * (p_{i} - p_ {i - 2})
 */

int ScriptedTransform::get_num_keyframes() const { return interpolator_position.get_num_points(); }

Transform ScriptedTransform::get_keyframe(int i) const {
    Transform transform;
    transform.set_position(interpolator_position.get_point(i));
    transform.set_rotation(interpolator_rotation.get_point(i));
    transform.set_scale(interpolator_scale.get_point(i));
    return transform;
}

Transform ScriptedTransform::interpolate_transform(double delta_time) {
    float arc_length_rotation = interpolator_time_v_arc_length_rotation.interpolate(delta_time).y;
    float arc_length_scale = interpolator_time_v_arc_length_scale.interpolate(delta_time).y;

    std::vector<float> accumulated_arc_lengths_rotation = interpolator_rotation.get_accumulated_arc_lengths();
    std::vector<float> accumulated_arc_lengths_scale = interpolator_scale.get_accumulated_arc_lengths();

    int n = accumulated_arc_lengths_rotation.size();

    float adjusted_delta_time_rotation = arc_length_rotation / accumulated_arc_lengths_rotation[n - 1] * duration -
                                         interpolator_rotation.accumulated_time;
    float adjusted_delta_time_scale =
        arc_length_scale / accumulated_arc_lengths_scale[n - 1] * duration - interpolator_scale.accumulated_time;

    Transform transform;
    transform.set_position(interpolator_position.interpolate(delta_time));
    // transform.set_rotation(interpolator_rotation.interpolate(delta_time));
    transform.set_rotation(interpolator_rotation.interpolate(adjusted_delta_time_rotation));
    // transform.set_scale(interpolator_scale.interpolate(delta_time));
    transform.set_scale(interpolator_scale.interpolate(adjusted_delta_time_scale));

    glm::vec3 rotation = transform.get_rotation();
    std::cout << rotation.x << "," << rotation.y << "," << rotation.z << std::endl;

    return transform;
}

void ScriptedTransform::update(double delta_time) {
    float arc_length_rotation = interpolator_time_v_arc_length_rotation.interpolate(delta_time).y;
    float arc_length_scale = interpolator_time_v_arc_length_scale.interpolate(delta_time).y;

    std::vector<float> accumulated_arc_lengths_rotation = interpolator_rotation.get_accumulated_arc_lengths();
    std::vector<float> accumulated_arc_lengths_scale = interpolator_scale.get_accumulated_arc_lengths();

    int n = accumulated_arc_lengths_rotation.size();

    float adjusted_delta_time_rotation = arc_length_rotation / accumulated_arc_lengths_rotation[n - 1] * duration -
                                         interpolator_rotation.accumulated_time;
    float adjusted_delta_time_scale =
        arc_length_scale / accumulated_arc_lengths_scale[n - 1] * duration - interpolator_scale.accumulated_time;

    transform.set_position(interpolator_position.interpolate(delta_time));
    // transform.set_rotation(interpolator_rotation.interpolate(delta_time));
    transform.set_rotation(interpolator_rotation.interpolate(adjusted_delta_time_rotation));
    // transform.set_scale(interpolator_scale.interpolate(delta_time));
    transform.set_scale(interpolator_scale.interpolate(adjusted_delta_time_scale));
}

void ScriptedTransform::reset() {
    interpolator_position.reset();
    interpolator_rotation.reset();
    interpolator_scale.reset();

    interpolator_time_v_arc_length_rotation.reset();
    interpolator_time_v_arc_length_scale.reset();
}

void ScriptedTransform::insert_keyframe(int i, Transform transform) {
    interpolator_position.insert_point(i, transform.get_translation());
    interpolator_rotation.insert_point(i, transform.get_rotation());
    interpolator_scale.insert_point(i, transform.get_scale());

    init_delta_time_adjustor();
}

void ScriptedTransform::append_keyframe(Transform transform) {
    interpolator_position.append_point(transform.get_translation());
    interpolator_rotation.append_point(transform.get_rotation());
    interpolator_scale.append_point(transform.get_scale());

    init_delta_time_adjustor();
}

void ScriptedTransform::delete_keyframe(int i) {
    interpolator_position.delete_point(i);
    interpolator_rotation.delete_point(i);
    interpolator_scale.delete_point(i);

    init_delta_time_adjustor();
}

void ScriptedTransform::set_keyframe(int i, Transform transform) {
    interpolator_position.set_point(i, transform.get_translation());
    interpolator_rotation.set_point(i, transform.get_rotation());
    interpolator_scale.set_point(i, transform.get_scale());

    init_delta_time_adjustor();
}


// tau's is also effected 


void ScriptedTransform::init_delta_time_adjustor() {
    std::vector<float> accumulated_arc_lengths_position = interpolator_position.get_accumulated_arc_lengths();
    std::vector<float> accumulated_arc_lengths_rotation = interpolator_rotation.get_accumulated_arc_lengths();
    std::vector<float> accumulated_arc_lengths_scale = interpolator_scale.get_accumulated_arc_lengths();

    std::vector<glm::vec2> points_rotation;
    std::vector<glm::vec2> points_scale;

    points_rotation.push_back(glm::vec2(-0.1, accumulated_arc_lengths_rotation[0] - 0.1));
    points_scale.push_back(glm::vec2(-0.1, accumulated_arc_lengths_scale[0] - 0.1));

    float accumulated_time;

    int n = accumulated_arc_lengths_position.size();
    for (int i = 0; i < n; i++) {
        // not all points??
        accumulated_time = duration * accumulated_arc_lengths_position[i] / accumulated_arc_lengths_position[n - 1];

        points_rotation.push_back(glm::vec2(accumulated_time, accumulated_arc_lengths_rotation[i]));
        points_scale.push_back(glm::vec2(accumulated_time, accumulated_arc_lengths_scale[i]));
    }

    points_rotation.push_back(glm::vec2(accumulated_time + 0.1, accumulated_arc_lengths_rotation[n - 1] + 0.1));
    points_scale.push_back(glm::vec2(accumulated_time + 0.1, accumulated_arc_lengths_scale[n - 1] + 0.1));

    interpolator_time_v_arc_length_rotation = DeltaTimeAdjustor(duration, 0.5, points_rotation);
    interpolator_time_v_arc_length_scale = DeltaTimeAdjustor(duration, 0.5, points_scale);
}
