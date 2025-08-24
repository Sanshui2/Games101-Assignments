//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        std::cout << "¼ÓÔØÍ¼Æ¬Â·¾¶: " << name << std::endl; // ´òÓ¡Êµ¼ÊÂ·¾¶
        image_data = cv::imread(name);
        if (image_data.empty()) {
            std::cerr << "Í¼Æ¬¼ÓÔØÊ§°Ü: " << name << std::endl;
        }
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        u = std::clamp(u, 0.0f, 1.0f);
        v = std::clamp(v, 0.0f, 1.0f);


        auto u_img = u * width;
        auto v_img = (1 - v) * height;

		int u0 = static_cast<int>(std::floor(u_img));
		int u1 = static_cast<int>(std::ceil(u_img));
		int v0 = static_cast<int>(std::floor(v_img));
		int v1 = static_cast<int>(std::ceil(v_img));

        u0 = std::clamp(u0, 0, width - 1);
        u1 = std::clamp(u1, 0, width - 1);
        v0 = std::clamp(v0, 0, height - 1);
        v1 = std::clamp(v1, 0, height - 1);

		auto color00 = image_data.at<cv::Vec3b>(v0, u0);
		auto color01 = image_data.at<cv::Vec3b>(v0, u1);
		auto color10 = image_data.at<cv::Vec3b>(v1, u0);
		auto color11 = image_data.at<cv::Vec3b>(v1, u1);

		float s = u_img - u0;
		float t = v_img - v0;
		auto top_color = (1 - s) * color00 + s * color01;
		auto bottom_color = (1 - s) * color10 + s * color11;

		auto color = (1 - t) * top_color + t * bottom_color;

        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

};
#endif //RASTERIZER_TEXTURE_H
