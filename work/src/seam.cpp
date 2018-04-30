#include "seam.hpp"

using namespace cv;
using namespace std;

Mat calcDiffernece(const Mat&src1, const Mat&src2) {
	Mat diff = Mat::zeros(src1.rows, src1.cols, CV_32SC1);
	Mat mat1(src1.rows,src1.cols, CV_8UC1), mat2(src2.rows, src2.cols, CV_8UC1);
	cvtColor(src1, mat1, COLOR_BGR2GRAY);
	cvtColor(src2, mat2, COLOR_BGR2GRAY);
	
	mat1.convertTo(mat1, CV_32S);
	mat2.convertTo(mat2, CV_32S);

	absdiff(mat1, mat2, diff);
	return diff;
}

vector<int> getSeam(Mat&err) {
	vector<int> seam;
	copyMakeBorder(err, err, 0, 0, 1, 1, BORDER_CONSTANT, Scalar(INT_MAX));

	for (int row = 1; row < err.rows; row++) {
		for (int col = 1; col < err.cols-1; col++) {
			int up = row - 1;
			int left = col - 1;
			int right = col + 1;
			
			err.at<int>(row, col) += 
			min(min(err.at<int>(up,left), err.at<int>(up, right)),
			err.at<int>(up, col));
		}
	}

	int minVal = INT_MAX;
	int col = 0;
	for (int i = 1; i < err.cols-1; i++) {
		int current = err.at<int>(err.rows - 1, i);
		if (current < minVal) { minVal = current; col = i; }
	}
	seam.push_back(col);
	int row = err.rows - 2;
	while (row >= 0) {
		int left = col-1, up = col, right = col+1;
		int minVal = min(min(err.at<int>(row, left), err.at<int>(row, right)),
			err.at<int>(row, up));
		if (err.at<int>(row, up) == minVal) { col = up; }
		else if (err.at<int>(row, left) == minVal) { col = left; }
		else if (err.at<int>(row, right) == minVal) { col = right; }
		seam.push_back(col-1);
		row--;
	}
	return seam;
}

Mat combineOverlap(const Mat&o1, const Mat&o2, const vector<int>& seam) {
	Mat combined(o1.rows, o1.cols, o1.type());
	//for (int row = o1.rows - 1; row >= 0; row--) {
	for (int row = 0; row < o1.rows; row++) {
		for (int col = 0; col < o1.cols; col++) {
			if (col < seam.at(o1.rows-1-row)) {
				combined.at<Vec3b>(row, col) = o1.at<Vec3b>(row, col);
			}else {
				combined.at<Vec3b>(row, col) = o2.at<Vec3b>(row, col);
			}
		}
	}
	return combined;
}

void drawSeam(Mat&src, const vector<int>& seam) {
	int width = 1;
	//for (int row = src.rows - 1; row >= 0; row--) {
	for (int row = 0; row < src.rows; row++) {
		src.at<Vec3b>(row, seam.at(src.rows-1-row)) = Vec3b(0, 0, 255);
	}
}

