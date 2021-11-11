#pragma once
#ifndef _img2GC_
#define _img2GC_

//TECHNIQUE IMAGE TO GREY
void calculate_grey_image(unsigned char* Image, int h, int w, int channels, int* histogram);
void calculate_grey_image2(unsigned char* Image, unsigned char* output, int h, int w, int channels);

//TECHNIQUE IMAGE TO BLUR
void calculate_blur_image(unsigned char* Image, unsigned char* output, int h, int w);

#endif