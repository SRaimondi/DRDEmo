//
// Created by simon on 08.05.17.
//

#include <scene.hpp>
#include <derivative.hpp>
#include <pinhole_camera.hpp>
#include <simple_renderer.hpp>
#include <direct_integrator.hpp>
#include <box_film.hpp>
#include <clamp_tonemapper.hpp>
#include <directional_light.hpp>
#include <triangle_mesh.hpp>

#define WIDTH   512
#define HEIGHT  512

#define MAX_ITERS 400

// Compute gradient norm, considering the gradient as a float vector
float GradNorm(std::vector<float> const &grad) {
    float norm = 0.f;
    for (auto const &e : grad) {
        norm += e * e;
    }

    return std::sqrt(norm);
}

// Print gradient
void PrintGradient(std::vector<float> const &grad) {
    std::cout << "(";
    for (size_t i = 0; i < grad.size(); ++i) {
        std::cout << grad[i];
        if (i != grad.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << ")" << std::endl;
}


//// Spherical function, minimization test
//drdemo::Float Spherical(std::vector<drdemo::Float> const &x) {
//    drdemo::Float res = x[0] * x[0];
//    for (size_t i = 1; i < x.size(); i++) {
//        res += x[i] * x[i];
//    }
//
//    return res;
//}

//// Matyas function
//Float Matyas(std::vector<Float> const &x) {
//    return 0.26f * (x[0] * x[0] + x[1] * x[1]) - 0.48f * x[0] * x[1];
//}

int main(void) {

    // Set namespace used
    using namespace drdemo;

    /**
     * Triangle mesh loading + simple minimization against black image
     */

    // Derivatives computation class
    Derivatives derivatives;

    // Try to load sphere mesh
    auto mesh = std::make_shared<TriangleMesh>("../objs/sphere.obj");

    // Get list of triangles
    // std::vector<std::shared_ptr<Shape> > triangles;
    // mesh.CreateTriangles(triangles);

    // Build BVH
    // auto bvh = std::make_shared<BVH>(triangles);

    // Create scene
    Scene scene;

    // Add sphere
    scene.AddShape(mesh);
    // Add lights
    scene.AddLight(std::make_shared<DirectionalLight>(Vector3F(0.f, 0.f, 1.f), Spectrum(0.9f)));

    // Create camera
    auto camera = PinholeCamera(Vector3F(0.f, 0.f, 5.f), Vector3F(), Vector3F(0.f, 1.f, 0.f), 60.f, WIDTH, HEIGHT);

    // Create renderer
    auto render = SimpleRenderer(std::make_shared<DirectIntegrator>());

    // Render target image
    BoxFilterFilm test(WIDTH, HEIGHT);
    render.RenderImage(&test, scene, camera);

    // Create tone-mapper and process target image
    ClampTonemapper tonemapper;
    tonemapper.Process("obj_test.ppm", test);

    return EXIT_SUCCESS;

    // Gradient
    std::vector<float> gradient(mesh->GetNumVars(), 0.f);
    std::vector<float> delta(mesh->GetNumVars(), 0.f);

    // Number of iterations
    size_t iters = 0;
    // Energy value
    float energy = 0.f;

    // Try to minimize squared norm of image
    do {
        // Clear derivatives
        derivatives.Clear();
        // Store current variables of the tape
        default_tape.Push();

        // Render current image
        BoxFilterFilm x(WIDTH, HEIGHT);
        render.RenderImage(&x, scene, camera);

        // Output image of current rendering
        tonemapper.Process("iters_" + std::to_string(iters) + ".ppm", x);

        // Compute difference
        // BoxFilterFilm difference = x - raw_target;

        // Compute squared norm of difference
        Float x_2_norm = x.SquaredNorm();
        energy = x_2_norm.GetValue();
        std::cout << "Energy: " << energy << std::endl;

        // Create difference image
        // difference.Abs();
        // tonemapper.Process("iters_" + std::to_string(iters) + "_difference.ppm", difference);

        // Compute derivatives
        derivatives.ComputeDerivatives(x_2_norm);

        // Get differentiable variables from scene's shapes, hardcoded TODO Fix this
        std::vector<Float const *> vars;
        scene.GetShapes()[0]->GetDiffVariables(vars);

        // Compute gradient and deltas
        for (size_t i = 0; i < vars.size(); i++) {
            gradient[i] = derivatives.Dwrt(x_2_norm, *vars[i]);
            delta[i] = -0.000001f * gradient[i];    // Learning rate
        }

        std::cout << "Iteration: " << iters << std::endl;
        std::cout << "Gradient norm: " << GradNorm(gradient) << std::endl;
        // std::cout << "Gradient values: ";
        // PrintGradient(gradient);

        // Update scene vars, hardcoded for the moment
        scene.GetShapes()[0]->UpdateDiffVariables(delta);

        std::cout << "Tape size before pop: " << default_tape.Size() << std::endl;
        // Pop variables
        default_tape.Pop();

        // Print tape size
        std::cout << "Tape size after pop: " << default_tape.Size() << std::endl;
        std::cout << std::endl;

        // std::cout << "Sphere data" << std::endl;
        // std::cout << scene.GetShapes()[0]->ToString() << std::endl << std::endl;
        iters++;
        // Stop when gradient is almost zero, "energy" is almost zero or maximum iterations reached
    } while (GradNorm(gradient) > 0.001f && energy > 0.01f && iters < MAX_ITERS);

    std::cout << "Final energy: " << energy << std::endl;
    std::cout << "Total iterations: " << iters << std::endl;
    std::cout << "Gradient norm: " << GradNorm(gradient) << std::endl;

    // default_tape.Disable();
    BoxFilterFilm final(WIDTH, HEIGHT);

    render.RenderImage(&final, scene, camera);
    tonemapper.Process("final.ppm", final);




    /**
     * Function minimisation test
     */
//    // Minimization test
//    std::vector<Float> x(10);
//
//    // Initialize with some random data
//    for (size_t i = 0; i < x.size(); i++) {
//        x[i] = 6.f;
//    }
//
//    Derivatives derivatives;
//
//    std::vector<float> gradient(x.size(), 0.f);
//    size_t iters = 0;
//    float delta = 0.1f;
//
//    // Get index of the variable we need to keep
//    // size_t clear_index = default_tape.Size();
//
//    do {
//        // Push current index of tape to remove nodes after
//        default_tape.Push();
//        // Clear derivatives
//        derivatives.Clear();
//        // Compute spherical function value
//        Float y = Spherical(x);
//        // Float y = Matyas(x);
//        // Compute derivatives
//        derivatives.ComputeDerivatives(y);
//        // Compute gradient and update
//        for (int i = 0; i < x.size(); i++) {
//            gradient[i] = derivatives.Dwrt(y, x[i]);
//            x[i].SetValue(x[i].GetValue() - delta * gradient[i]);
//        }
//        std::cout << "Gradient norm: " << GradNorm(gradient) << std::endl;
//        // Increase number of iterations
//        iters++;
//        // Clear tape
//        // default_tape.Clear(clear_index);
//        default_tape.Pop();
//        std::cout << "Tape size: " << default_tape.Size() << std::endl;
//    } while (GradNorm(gradient) > 0.0001f && iters < 10000);
//
//    std::cout << std::endl << "Final gradient norm: " << GradNorm(gradient) << std::endl;
//    std::cout << "Iterations: " << iters << std::endl;
//
//    std::cout << "#### Final x ####" << std::endl;
//    for (auto const & x_i : x) {
//        std::cout << x_i << " ";
//    }
//    std::cout << std::endl;


    /**
     * Geometric sphere description minimisation against target sphere image
     */

//    // Derivatives computation class
//    Derivatives derivatives;
//
//    // Disable tape to render target image
//    // default_tape.Disable();
//
//    // Create scene
//    Scene scene;
//
//    // Add sphere
//    scene.AddShape(std::make_shared<Sphere>(Vector3F(1.f, 0.f, 0.f), Float(2.f)));
//    // Add lights
//    scene.AddLight(std::make_shared<DirectionalLight>(Vector3F(0.f, 0.f, 1.f), Spectrum(0.9f)));
//
//    // Create camera
//    auto camera = PinholeCamera(Vector3F(0.f, 0.f, 10.f), Vector3F(), Vector3F(0.f, 1.f, 0.f), 60.f, WIDTH, HEIGHT);
//
//    // Create renderer
//    auto render = SimpleRenderer(std::make_shared<DirectIntegrator>());
//
//    // Push status of tape before rendering target image
//    default_tape.Push();
//
//    // Render target image
//    BoxFilterFilm target(WIDTH, HEIGHT);
//    render.RenderImage(&target, scene, camera);
//
//    // Convert image to raw
//    Vector<float> raw_target = target.Raw();
//
//    // Create tone-mapper and process target image
//    ClampTonemapper tonemapper;
//    tonemapper.Process("target.ppm", target);
//
//    // Re-enable tape to compute derivatives
//    // default_tape.Enable();
//
//    // Remove from tape rendering variables
//    default_tape.Pop();
//
//    // Change sphere position to center and try to match the images
//    scene.ClearShapes();
//    scene.AddShape(std::make_shared<Sphere>(Vector3F(0.f, 0.f, 0.f), Float(1.f)));
//
//    // Gradient
//    std::vector<float> gradient(4, 0.f);
//    std::vector<float> delta(4, 0.f);
//
//    // Number of iterations
//    size_t iters = 0;
//    // Energy value
//    float energy;
//
//    // Try to minimize squared norm of image
//    do {
//        // Clear derivatives
//        derivatives.Clear();
//        // Store current variables of the tape
//        default_tape.Push();
//
//        // Render current image
//        BoxFilterFilm x(WIDTH, HEIGHT);
//        render.RenderImage(&x, scene, camera);
//
//        // Output image of current rendering
//        tonemapper.Process("iters_" + std::to_string(iters) + ".ppm", x);
//
//        // Compute difference
//        BoxFilterFilm difference = x - raw_target;
//
//        // Compute squared norm of difference
//        Float x_2_norm = difference.SquaredNorm();
//        energy = x_2_norm.GetValue();
//        std::cout << "Energy: " << energy << std::endl;
//
//        // Create difference image
//        difference.Abs();
//        tonemapper.Process("iters_" + std::to_string(iters) + "_difference.ppm", difference);
//
//        // Compute derivatives
//        derivatives.ComputeDerivatives(x_2_norm);
//
//        // Get differentiable variables from scene's shapes
//        std::vector<Float const *> vars;
//        scene.GetShapes()[0]->GetDiffVariables(vars);
//
//        // Compute gradient and deltas
//        for (size_t i = 0; i < vars.size(); i++) {
//            gradient[i] = derivatives.Dwrt(x_2_norm, *vars[i]);
//            delta[i] = -0.000001f * gradient[i];    // Learning rate
//        }
//
//        std::cout << "Iteration: " << iters << std::endl;
//        std::cout << "Gradient norm: " << GradNorm(gradient) << std::endl;
//        std::cout << "Gradient values: ";
//        PrintGradient(gradient);
//
//        // Update scene vars, hardcoded for the moment
//        scene.GetShapes()[0]->UpdateDiffVariables(delta);
//
//        std::cout << "Tape size before pop: " << default_tape.Size() << std::endl;
//        // Pop variables
//        default_tape.Pop();
//
//        // Print tape size
//        std::cout << "Tape size after pop: " << default_tape.Size() << std::endl;
//
//        std::cout << "Sphere data" << std::endl;
//        std::cout << scene.GetShapes()[0]->ToString() << std::endl << std::endl;
//        iters++;
//        // Stop when gradient is almost zero, "energy" is almost zero or maximum iterations reached
//    } while (GradNorm(gradient) > 0.001f && energy > 0.01f && iters < MAX_ITERS);
//
//    std::cout << "Final energy: " << energy << std::endl;
//    std::cout << "Total iterations: " << iters << std::endl;
//    std::cout << "Gradient norm: " << GradNorm(gradient) << std::endl;
//
//    // default_tape.Disable();
//    BoxFilterFilm final(WIDTH, HEIGHT);
//
//    render.RenderImage(&final, scene, camera);
//    tonemapper.Process("final.ppm", final);
//
//    std::cout << "Final sphere data" << std::endl;
//    std::cout << scene.GetShapes()[0]->ToString() << std::endl;


    return EXIT_SUCCESS;
}
