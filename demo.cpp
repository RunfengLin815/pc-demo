#include <librealsense2/rs.hpp>
#include "./example.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"

#include <iostream>
#include <string>
#include <dirent.h>

// 3rd party header for writing png files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// This example assumes camera with depth and color streams, and direction lets you define the target stream
enum class direction
{
    to_depth,
    to_color
};


void vf2png (rs2::video_frame vf);

int main () {
    // Create a simple OpenGL window
    window app(1280, 720, "RealSense Record");
    ImGui_ImplGlfw_Init(app, false);

    // create two textures on gpu
    texture img_col, img_dep;
    // create color map since depth data usually grey
    rs2::colorizer color_map;

    // Create a shared pointer to a pipeline ("shared is for recording and saving")
    auto pipe = std::make_shared<rs2::pipeline>();

    // create configuration
    rs2::config cfg;
    // init pipeline, get stream from depth and color for alignment
    cfg.enable_stream(RS2_STREAM_DEPTH);
    cfg.enable_stream(RS2_STREAM_COLOR);

    // start shared pipeline with config
    pipe->start(cfg);

    // Initialize a shared pointer to a device with the current device on the pipeline
    rs2::device device = pipe->get_active_profile().get_device();

    // init alignment
    // define align object (out of main loop)
    rs2::align align_to_depth(RS2_STREAM_DEPTH);
    rs2::align align_to_color(RS2_STREAM_COLOR);

    float       alpha = 0.5f;               // Transparancy coefficient

    // main loop, if app existing, keep running
    int i=0;
    while ( i < 1 ) {
        // wait for next set of frames from the camera
        rs2::frameset frameset = pipe->wait_for_frames();

        // Align all frames to depth viewport
        frameset = align_to_color.process(frameset);

        // get color data
        auto color = frameset.get_color_frame();

        // get depth data
        auto depth = frameset.get_depth_frame();
        auto colorized_depth = color_map.colorize(depth);

        // rendering and show in window
        img_dep.render(colorized_depth, { 0, 0, app.width(), app.height() });
        img_col.render(color, { 0, 0, app.width(), app.height() }, alpha);

        // Write images to disk
        vf2png(color);
        vf2png(colorized_depth);

        i++;

    }



    // return
    return EXIT_SUCCESS;

}

void vf2png (const rs2::video_frame vf) {
    std::stringstream png_file;
    png_file << "rs-demo-" << vf.get_profile().stream_name() << ".png";
    stbi_write_png(png_file.str().c_str(), vf.get_width(), vf.get_height(),
                   vf.get_bytes_per_pixel(), vf.get_data(), vf.get_stride_in_bytes());
//    std::string prefix = "/home/linrunfeng/Data/rs_save_demo";
    std::cout << "Saved " << png_file.str() << std::endl;
}
