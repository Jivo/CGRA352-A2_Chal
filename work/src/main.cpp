
// std
#include <iostream>

// opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "seam.hpp"


using namespace cv;
using namespace std;

Mat getRandPatch(const Mat&src, Size s);
vector<Mat> getPatches(const Mat&src, Size s, int num);
Mat overlapLeft(const Mat&src);
Mat overlapRight(const Mat&src);
Mat bestOverlap(const Mat&current, vector<Mat> others);
Mat mergePatches(const Mat& patch1, const Mat& patch2, const Mat&overlap);

const float PROPORTION = 0.25;


// main program
// 
int main( int argc, char** argv ) {

	if( argc != 3) {
		cout << "Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;}

	// read the file
	Mat initPatch, texture;
	initPatch = imread(argv[1], CV_LOAD_IMAGE_COLOR); 
	texture = imread(argv[2], CV_LOAD_IMAGE_COLOR);
	if (!initPatch.data || !texture.data) {cout << "invalid arguments";}

	vector<Mat> patches = getPatches(texture, initPatch.size(), 60);
	Mat rightOverlap = overlapRight(initPatch);
	
	Mat bestPatch = bestOverlap(rightOverlap, patches);
	Mat leftOverlap = overlapLeft(bestPatch);
	
	//Mat diff = calcDiffernece(rightOverlap, leftOverlap);
	vector<int> seam = getSeam(calcDiffernece(rightOverlap, leftOverlap));

	Mat newOverlap = combineOverlap(rightOverlap, leftOverlap, seam);
	Mat merged = mergePatches(initPatch, bestPatch, newOverlap);

	namedWindow("Merged", WINDOW_AUTOSIZE);
	imshow("Merged", merged);

	drawSeam(newOverlap, seam);
	merged = mergePatches(initPatch, bestPatch, newOverlap);

	namedWindow("Merged Seam", WINDOW_AUTOSIZE);
	imshow("Merged Seam", merged);
	
	waitKey(0);
}

/**********************
    GET RAND PATCH
**********************/
Mat getRandPatch(const Mat&src, Size s) {
	int maxX = src.cols - 1-s.width;
	int maxY = src.rows - 1 - s.height;

	int randX = int(((double)rand()/RAND_MAX) * maxX);
	int randY = int(((double)rand()/RAND_MAX) * maxY);

	Rect patch(randX, randY, s.width, s.height);

	return src(patch);
}

/**********************
     GET PATCHES
**********************/
vector<Mat> getPatches(const Mat&src, Size s, int num) {
	vector<Mat> patches;
	for (int i = 0; i < num; i++) {
		patches.push_back(getRandPatch(src, s));
	}
	return patches;
}

/**********************
    OVERLAP RIGHT
**********************/
Mat overlapRight(const Mat&src) {
	Rect r(src.cols - 1 - int((float)src.cols * PROPORTION), 0,
		int((float)src.cols * PROPORTION), src.rows);
	return src(r);
}

/**********************
    OVERLAP LEFT
**********************/
Mat overlapLeft(const Mat&src) {
	Rect r(0, 0,
		int((float)src.cols * PROPORTION), src.rows);
	return src(r);
}
/**********************
    BEST OVERLAP
**********************/
Mat bestOverlap(const Mat&current, vector<Mat> others) {
	float bestNorm = norm(current, Mat::zeros(current.rows, current.cols, current.type()));
	Mat bestMat;
	
	for (Mat m : others) {
		float newNorm = norm(current, overlapLeft(m));
		if (newNorm < bestNorm) {
			bestMat = m;
			bestNorm = newNorm;
		}
	}
	return bestMat;
}

/**********************
    MERGE PATCHES
**********************/
Mat mergePatches(const Mat& patch1, const Mat& patch2, const Mat&overlap) {
	Mat merged(patch1.rows, (patch1.cols * 2) - overlap.cols, patch1.type());
	patch1.copyTo(merged(Rect(0, 0, patch1.cols, patch1.rows)));
	patch2.copyTo(merged(Rect(patch1.cols - overlap.cols, 0, patch2.cols, patch2.rows)));
	overlap.copyTo(merged(Rect(patch1.cols - overlap.cols, 0, overlap.cols, overlap.rows)));
	return merged;
}