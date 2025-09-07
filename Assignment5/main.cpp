#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm
    if (control_points.size() == 1) 
    {
		return control_points[0];
    }

	std::vector<cv::Point2f> new_control_points;
    for (int i = 0; i < control_points.size() - 1; ++i)
    {
		new_control_points.push_back((1 - t) * control_points[i] + t * control_points[i + 1]);
    }
	return recursive_bezier(new_control_points, t);

}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
	for (double t = 0.0; t <= 1.0; t += 0.001)
	{
		auto point = recursive_bezier(control_points, t);

        //用 floor 锚定 包含该点的像素小方格 的左上角整数坐标(px, py)
        //像素中心在 (i+0.5, j+0.5)，所以后面算距离要加 0.5。
		int px = (int)std::floor(point.x);
		int py = (int)std::floor(point.y);

        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++) 
            {
              //(px + dx, py + dy) 就能枚举这个小方格的四个像素中心：
              //  (px, py)
              //  (px + 1, py)
              //  (px, py + 1)
              //  (px + 1, py + 1)
				int nx = px + dx, ny = py + dy;
				if (0 <= nx && nx < window.cols && 0<= ny && ny < window.rows)
				{
                    float dx_f = point.x - (nx + 0.5f);
					float dy_f = point.y - (ny + 0.5f);
					float dist2 = dx_f * dx_f + dy_f * dy_f;

					const float max_dist = 3.0f * std::sqrt(2.0f)/2.0f;
					const float max_dist2 = max_dist * max_dist;
					float w = 1.0f - dist2 / max_dist2;
                    if (w > 0)
                    {
                        cv::Vec3b& pix = window.at<cv::Vec3b>(ny, nx);
                        pix[1] = (uchar)std::min(255, (int)pix[1] + (int)std::round(255 * w));

                    }
				}
            }
        }
	
	}


}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4) 
        {
            //naive_bezier(control_points, window);
             bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}
