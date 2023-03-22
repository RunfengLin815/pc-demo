#include <librealsense2/rs.hpp>
#include "./example.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <chrono>

// 3rd party header for writing png files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool capture_flag = false;
void on_button_press (int state, void* userdata) {
    capture_flag = true;
}


int main () {

    // create a pipeline
    rs2::pipeline pipe = rs2::pipeline();

    // config
    rs2::config cfg;
    int color_width = 640;
    int color_height = 480;
    int depth_width = 640;
    int depth_height = 480;
    int rate = 30;
    cfg.enable_stream(RS2_STREAM_COLOR, color_width, color_height, RS2_FORMAT_BGR8, rate);
    cfg.enable_stream(RS2_STREAM_DEPTH, depth_width, depth_height, RS2_FORMAT_Z16, rate);

    // start shared pipeline with config
    pipe.start(cfg);

    // create new window and button
    cv::namedWindow("pc-demo");

    // define align object (align to color bcs color scene is smaller)
    rs2::align align_to_color(RS2_STREAM_COLOR);

    // main loop
    int i=0;
    while ( true ) {

        // start time
        auto t_start = std::chrono::high_resolution_clock::now();

        // wait for next set of frames from the camera
        rs2::frameset frameset = pipe.wait_for_frames();

        // Align all frames to depth viewport
        frameset = align_to_color.process(frameset);

        // get color data
        auto color_frame = frameset.get_color_frame();
        // get depth data
        auto depth_frame = frameset.get_depth_frame();

        // time get frame
        auto t_end_1 = std::chrono::high_resolution_clock::now();

        // get cv mat (Note that type is different)
        int rows = color_frame.get_height();
        int cols = color_frame.get_width();

        std::cout << "(" << rows << ", " << cols << ")" << std::endl;

        cv::Mat depth_img(rows, cols, CV_16UC1, (void *)depth_frame.get_data());
        cv::Mat color_img(rows, cols, CV_8UC3, (void *)color_frame.get_data());

        // open window
        cv::imshow("pc-demo", color_img);

        // write to disk
        std::string save_path = "/home/linrunfeng/Lab/data/shelter-demo";

        std::string depth_path = save_path + "/depth/" + std::to_string(i) + ".png";
        cv::imwrite(depth_path, depth_img);
        std::cout << "Saved depth img " << i << ".png" << std::endl;

        std::string color_path = save_path + "/rgb/" + std::to_string(i) + ".png";
        cv::cvtColor(color_img, color_img, cv::COLOR_RGB2BGR);
        cv::imwrite(color_path, color_img);
        std::cout << "Saved color img " << i << ".png" << std::endl;

        // time save
        auto t_end_2 = std::chrono::high_resolution_clock::now();

        // calculate time
        auto dur_get = std::chrono::duration_cast<std::chrono::microseconds>(t_end_1-t_start).count();
        auto dur_save = std::chrono::duration_cast<std::chrono::microseconds>(t_end_2-t_end_1).count();

        std::cout << "Time get: " << dur_get << std::endl;
        std::cout << "Time save: " << dur_save << std::endl;

        // step forward
        i++;

        // quit
        if (cv::waitKey(1) == 'q') break;

    }

    // stop pipeline
    pipe.stop();

    // return
    return 0;

}
