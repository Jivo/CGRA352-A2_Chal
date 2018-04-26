
// std
#include <iostream>

// opencv
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

vector<int> getSeam(Mat&err);
Mat combineOverlap(const Mat&o1, const Mat&o2, const vector<int>& seam);
void drawSeam(Mat&src, const vector<int>& seam);
Mat calcDiffernece(const Mat&src1, const Mat&src2);

