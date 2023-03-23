#include <librealsense2/rs.hpp>
#include "./example.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

// 3rd party header for writing png files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

enum class sts {
    rgb,
    depth
};

void save2disk (std::vector<cv::Mat> vec_mat, std::string stream_name, std::string parent_path);

int main () {

    // create vector for frames
    std::vector<cv::Mat> vec_color;
    std::vector<cv::Mat> vec_depth;

    // create a pipeline
    rs2::pipeline pipe = rs2::pipeline();

    // config
    rs2::config cfg;
    int color_width = 640;
    int color_height = 480;
    int depth_width = 640;
    int depth_height = 480;
    int rate = 30;
    cfg.enable_stream(RS2_STREAM_COLOR, color_width, color_height, RS2_FORMAT_RGB8, rate);
    cfg.enable_stream(RS2_STREAM_DEPTH, depth_width, depth_height, RS2_FORMAT_Z16, rate);

    // start shared pipeline with config
    pipe.start(cfg);

    // create new window and button
    cv::namedWindow("pc-demo");

    // define align object (align to color bcs color scene is smaller)
    rs2::align align_to_color(RS2_STREAM_COLOR);

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
//        auto t_end_1 = std::chrono::high_resolution_clock::now();

        // get cv mat (Note that type is different)
        int rows = color_frame.get_height();
        int cols = color_frame.get_width();

        std::cout << "(" << rows << ", " << cols << ")" << std::endl;

        cv::Mat depth_img(rows, cols, CV_16UC1, (void *)depth_frame.get_data());
        cv::Mat color_img(rows, cols, CV_8UC3, (void *)color_frame.get_data());

        // append to vector
        vec_color.push_back(color_img);
        vec_depth.push_back(depth_img);

        // open window
        cv::imshow("pc-demo", color_img);

        // quit
        if (cv::waitKey(1) == 'q') break;

    }

    // write to disk
    std::string parent_path = "/home/linrunfeng/Lab/data/shelter-demo/";
    save2disk(vec_color, "rgb", parent_path);
    save2disk(vec_depth, "depth", parent_path);

    // stop pipeline
    pipe.stop();

    // return
    return 0;

}

/*
 *
 */
void save2disk (std::vector<cv::Mat> vec_mat, std::string stream_name, std::string parent_path) {
    int i = 0;
    for (auto& mat : vec_mat) {
        std::string save_path = parent_path + stream_name + "/" + std::to_string(i) + ".png";
        cv::imwrite(save_path, mat);
        std::cout << "Saved " << stream_name << " img " << i << ".png" << std::endl;
        i ++;
    }
}