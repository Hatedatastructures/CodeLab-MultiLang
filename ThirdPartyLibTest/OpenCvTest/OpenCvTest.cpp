// #include <iostream>
// import <vector>
// #include <../opencv4/opencv2/opencv.hpp>
// using namespace cv;
// int main()
// {
//     cv::Mat img;
//     char imageName[] = "../headend_based_learning/gallery/flower.webp";   
//     Mat M=imread(imageName,IMREAD_COLOR);   // 读入图片 
         
//     if(M.empty())     // 判断文件是否正常打开  
//     {
//          fprintf(stderr, "Can not load image %s\n", imageName);
//          waitKey(6000);  // 等待6000 ms后窗口自动关闭   
//          return -1;
//     }
    
//     imshow("image",M);  // 显示图片 
//     waitKey();
//     imwrite("pic.bmp",M); // 存为bmp格式图片
//     return 0;
// }
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

// int main() 
// {
//     // 读取图像
//     Mat image1 = imread("../headend_based_learning/gallery/flower.webp");
//     Mat image2 = imread("../headend_based_learning/gallery/flower.webp");
//     if (image1.empty() || image2.empty()) return -1;

//     // 特征点检测与匹配
//     Ptr<Feature2D> detector = ORB::create();
//     vector<KeyPoint> keypoints1, keypoints2;
//     Mat descriptors1, descriptors2;
//     detector->detectAndCompute(image1, noArray(), keypoints1, descriptors1);
//     detector->detectAndCompute(image2, noArray(), keypoints2, descriptors2);

//     BFMatcher matcher(NORM_HAMMING);
//     vector<DMatch> matches;
//     matcher.match(descriptors1, descriptors2, matches);

//     // 计算基础矩阵
//     vector<Point2f> points1, points2;
//     for (const auto& match : matches) 
//     {
//         points1.push_back(keypoints1[match.queryIdx].pt);
//         points2.push_back(keypoints2[match.trainIdx].pt);
//     }
//     Mat fundamentalMatrix = findFundamentalMat(points1, points2, FM_RANSAC);

//     // 输出基础矩阵
//     cout << "Fundamental Matrix:\n" << fundamentalMatrix << endl;

//     return 0;
// }

int main() 
{
    // 打开摄像头
    VideoCapture cap(0);
    if (!cap.isOpened()) 
    {
        cout << "错误：无法打开摄像头！" << endl;
        return -1;
    }

    // 实时处理视频帧
    Mat frame, edges;
    while (true)
    {
        cap >> frame;
        if (frame.empty()) break;

        // 边缘检测
        Canny(frame, edges, 100, 200);

        // 显示结果
        imshow("Original Frame", frame);
        imshow("Edges", edges);

        // 按下 ESC 键退出
        if (waitKey(30) == 27) break;
    }

    // 释放资源
    cap.release();
    destroyAllWindows();

    return 0;
}