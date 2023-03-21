#include <librealsense2/rs.hpp>
#include "./example.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

// 3rd party header for writing png files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


int main () {

    // create two textures on gpu
    texture img_col, img_dep;
    // create color map since depth data usually grey
    rs2::colorizer color_map;

    // create a pipeline
    rs2::pipeline pipe = rs2::pipeline();

    // start shared pipeline with config
    pipe.start();

    // init alignment
    // define align object (out of main loop)
    rs2::align align_to_depth(RS2_STREAM_DEPTH);
    rs2::align align_to_color(RS2_STREAM_COLOR);

    // main loop
    int i=0;
    while ( i < 1 ) {
        // wait for next set of frames from the camera
        rs2::frameset frameset = pipe.wait_for_frames();

        // Align all frames to depth viewport
        frameset = align_to_color.process(frameset);

        // get color data
        auto color_frame = frameset.get_color_frame();
        // get depth data
        auto depth_frame = frameset.get_depth_frame();

        // write depth to disk
        cv::Mat depth_img(depth_frame.get_height(), depth_frame.get_width(), CV_16UC1, (void *)depth_frame.get_data());
        cv::imwrite("depth_img.png", depth_img);

        // write color to disk
        cv::Mat color_img(color_frame.get_height(), color_frame.get_width(), CV_8UC3, (void *)color_frame.get_data());
        cv::cvtColor(color_img, color_img, cv::COLOR_RGB2BGR);
        cv::imwrite("color_img.png", color_img);

        i++;

    }



    // return
    return 0;

}
