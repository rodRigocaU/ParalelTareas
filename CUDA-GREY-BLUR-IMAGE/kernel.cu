
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "img2GC.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
using namespace std;

/*
 -> To develop this homework -> Rodrigo Cayro
 i use two different techniques: one based in histograms and the second based in the formule of the book
*/

//tecchnique #1 based in histograms
__global__ void togrey(unsigned char* image, int* H);

//ALLOCATE MEMORY FOR THE PROCCESS
void calculate_grey_image(unsigned char* Image, int h, int w, int channels, int* histogram)
{
    unsigned char* DEV_img = NULL;
    int* DEV_h = NULL;
    cudaMalloc((void**)&DEV_img, h * w * channels);
    cudaMalloc((void**)&DEV_h, 256 * sizeof(int));

    //TRANSFER DATA FROM CPU TO GPU 
    cudaMemcpy(DEV_img, Image, h * w * channels, cudaMemcpyHostToDevice);
    cudaMemcpy(DEV_img, Image, 256 * sizeof(int), cudaMemcpyHostToDevice);

    //IMAGE HISTOGRAM
    dim3 Grid_image(h, w);
    togrey << <Grid_image, 1 >> >(DEV_img,DEV_h);

    //from GPU => CPU
    cudaMemcpy(histogram, DEV_h, 256 * sizeof(int), cudaMemcpyDeviceToHost);

    //free memory
    cudaFree(DEV_h);
    cudaFree(DEV_img);
}
__global__ void togrey(unsigned char* image, int* H)
{
    int x = blockIdx.x;
    int y = blockIdx.y;

    int Image_Idx = x + y * gridDim.x;
    atomicAdd(&H[image[Image_Idx]], 1);
}


//tecchnique #2 based in formule
__global__ void togrey2(unsigned char* image, unsigned char* output, int h, int w, int CHANNELS)
{
    int col = threadIdx.x + blockIdx.x * blockDim.x;
    int row = threadIdx.y + blockIdx.y * blockDim.y;

    //USING THE FORMULE WE COMPUTE FOR EACH PIXEL HIS GREY VERSION
    if (col < w && row < h)
    {
        int grey_offset = row * w + col;
        int rgb_offset = grey_offset * CHANNELS;

        unsigned char r = image[rgb_offset + 0];
        unsigned char g = image[rgb_offset + 1];
        unsigned char b = image[rgb_offset + 2];

        output[grey_offset] = r * 0.299f + g * 0.587f + b * 0.114f;
    }
}


void calculate_grey_image2(unsigned char* Image, unsigned char* output, int h, int w, int channels)
{
    unsigned char* DEV_img = NULL;
    unsigned char* DEV_output = NULL;

    cudaMalloc((void**)&DEV_img, h * w * channels);
    cudaMalloc((void**)&DEV_output, h * w);

    //TRANSFER DATA FROM CPU TO GPU 
    cudaMemcpy(DEV_img, Image, h * w * channels, cudaMemcpyHostToDevice);

    //WE DIVIDE THE GRID IN  16 PARTS
    //dim3 Grid_image((int)ceil(h / 16), (int)ceil(w / 16));
    dim3 Grid_image(w, h);
    //dim3 dimBlock(16, 16);
    togrey2 << <Grid_image, 1 >> > (DEV_img, DEV_output, h, w, channels);

    //TRANSFER DATA FROM GPU TO CPU 
    cudaMemcpy(output, DEV_output, h * w, cudaMemcpyDeviceToHost);


    //free memory
    cudaFree(DEV_output);
    cudaFree(DEV_img);

}

//tecchnique #2 based in formule
__global__ void toblur(unsigned char* image, unsigned char* output, int h, int w)
{
    int col = threadIdx.x + blockIdx.x * blockDim.x;
    int row = threadIdx.y + blockIdx.y * blockDim.y;
    int BLUR_SIZE = 20;

    //USING THE FORMULE WE COMPUTE FOR EACH PIXEL HIS GREY VERSION
    if (col < w && row < h)
    {
        int pixVal = 0;
        int pixels = 0;

        for (int blurRow = -BLUR_SIZE; blurRow < BLUR_SIZE + 1; ++blurRow)
        {
            for (int blurCol = -BLUR_SIZE; blurCol < BLUR_SIZE + 1; ++blurCol)
            {
                int curRow = row + blurRow;
                int curCol = col + blurCol;
                if (curRow > -1 && curRow < h && curCol > -1 && curCol < w)
                {
                    pixVal += image[curRow * w + curCol];
                    pixels++;
                }
            }
        }
        output[row * w + col] = (unsigned char) (pixVal / pixels);

    }
}


void calculate_blur_image(unsigned char* Image, unsigned char* output, int h, int w)
{
    //cout << "ENTRE" << '\n';
    unsigned char* DEV_img = NULL;
    unsigned char* DEV_output = NULL;

    cudaMalloc((void**)&DEV_img, h * w );
    cudaMalloc((void**)&DEV_output, h * w );

    //TRANSFER DATA FROM CPU TO GPU 
    cudaMemcpy(DEV_img, Image, h * w, cudaMemcpyHostToDevice);

    //WE DIVIDE THE GRID IN  16 PARTS
    //dim3 Grid_image(w, h);
    dim3 Grid_image((int)ceil(w / 16), (int)ceil(h / 16));
    dim3 dimBlock(16, 16);
    toblur << <Grid_image, dimBlock >> > (DEV_img, DEV_output, h, w);

    //TRANSFER DATA FROM GPU TO CPU 
    cudaMemcpy(output, DEV_output, h * w, cudaMemcpyDeviceToHost);


    //free memory
    cudaFree(DEV_output);
    cudaFree(DEV_img);

}


