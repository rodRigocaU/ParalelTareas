#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "img2GC.h"

using namespace std;
using namespace cv;

//SEPARETE FUNCTIONS BECAUSE IT GENERATES ERRORS
//OUTPUT -> FIRST TECHNIQUE BASED IN FORMULE BOOK 
void to_output(unsigned char* output, int h, int w)
{
	Mat outData(h,w,CV_8UC1,(void *) output);
	imwrite("result2.jpg", outData);
}
//OUTPUT -> SECOND TECHNIQUE BASED IN BLUR 
void to_output2(unsigned char* output, int h, int w)
{
	Mat outData(h, w, CV_8UC1, (void*)output);
	imwrite("result3.jpg", outData);
}

int main()
{
	Mat input_img = imread("test_image.jpg", 0);

	//IMAGE INFORMATION
	//FIRST  TECHNIQUE

    cout << "H: " << input_img.rows << " W: " << input_img.cols << '\n';
	
	int img_histo[256] = {};

	
	calculate_grey_image(input_img.data, input_img.rows, input_img.cols, input_img.channels(), img_histo);
	imwrite("result.jpg", input_img);


	//SECOND TECHNIQUE
	Mat input_img2 = imread("test_image.jpg");
	unsigned char* output;
	output = (unsigned char*)malloc(sizeof(unsigned char*) * input_img2.rows * input_img2.cols);
	calculate_grey_image2(input_img2.data,output, input_img2.rows, input_img2.cols, input_img2.channels());
	to_output(output, input_img2.rows, input_img2.cols);


	//BLUR IMAGE
	Mat input_img3 = imread("test_image.jpg", 0);
	unsigned char* output2;
	output2 = (unsigned char*)malloc(sizeof(unsigned char*) * input_img3.rows * input_img3.cols);
	calculate_blur_image(input_img3.data, output2, input_img3.rows, input_img3.cols);
	to_output2(output2, input_img3.rows, input_img3.cols);

	system("pause");
	return 0;
}