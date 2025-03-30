#ifndef DELTA_TIME_ADJUSTOR
#define DELTA_TIME_ADJUSTOR

#include <glm/glm.hpp>
#include <vector>

class DeltaTimeAdjustor {
  public:
    // for point in points, point.x is the time & point.y is the arc length -- sketchy for self loops?
    DeltaTimeAdjustor(double duration, float tau) {}; // TODO: remove
    DeltaTimeAdjustor(double duration, float tau, std::vector<glm::vec2> points);

    glm::vec2 interpolate(double delta_time);
    void reset();

  private:
    struct Polynomial {
        glm::vec2 coef_constant;
        glm::vec2 coef_linear;
        glm::vec2 coef_quadratic;
        glm::vec2 coef_cubic;
        float x_length;
    };
    std::vector<glm::vec2> points;
    std::vector<Polynomial> polynomials;
    double duration;
    double accumulated_time;
    float tau;

    struct Cache {
        int i = 0;
        float t = 0.0f;
        float x_length = 0.0f;
    } cache; // use cached variables to numerically integrate for arc length

    float dt = 0.0001f;
};

#endif // DELTA_TIME_ADJUSTOR
