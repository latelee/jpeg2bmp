#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>
// jpeg库头文件必须放到stdio.h后面
#include "include/libjpeg/jpeglib.h"
#include "include/libjpeg/jerror.h"

#include "jpeg_utils.h"
#include "bmp_utils.h"

#pragma comment(lib, "lib/libjpeg.lib")
#pragma warning(disable:4146)

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

void my_error_exit (j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    (*cinfo->err->output_message) (cinfo);

    longjmp(myerr->setjmp_buffer, 1);
}

// 读取JPG图片数据，并解压到内存中，*rgb_buffer需要自行释放
int read_jpg_file(const char* jpg_file, unsigned char** rgb_buffer, int* size, int* width, int* height)
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    FILE* fp;

    JSAMPARRAY buffer;
    int row_stride = 0;
    unsigned char* tmp_buffer = NULL;
    int rgb_size;

    fp = fopen(jpg_file, "rb");
    if (fp == NULL)
    {
        printf("open file %s failed.\n", jpg_file);
        return -1;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return -1;
    }

    jpeg_create_decompress(&cinfo);

    // libjpeg指定图片文件
    jpeg_stdio_src(&cinfo, fp);

    jpeg_read_header(&cinfo, TRUE);

    //cinfo.out_color_space = JCS_RGB; //JCS_YCbCr;  // 设置输出格式

    jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;
    *width = cinfo.output_width;
    *height = cinfo.output_height;

    rgb_size = row_stride * cinfo.output_height; // 总大小
    *size = rgb_size;

    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    
    *rgb_buffer = (unsigned char *)malloc(sizeof(char) * rgb_size);    // 分配总内存
    
    printf("debug--:\nrgb_size: %d, size: %d w: %d h: %d row_stride: %d \n", rgb_size,
                cinfo.image_width*cinfo.image_height*3,
                cinfo.image_width, 
                cinfo.image_height,
                row_stride);
    tmp_buffer = *rgb_buffer;
    while (cinfo.output_scanline < cinfo.output_height) // 解压每一行
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        // 复制到内存
        memcpy(tmp_buffer, buffer[0], row_stride);
        tmp_buffer += row_stride;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(fp);

    return 0;
}

int read_jpg_buffer(unsigned char* jpg_buffer, int jpg_size, unsigned char** rgb_buffer, 
                    int* size, int* width, int* height)
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;

    JSAMPARRAY buffer;
    int row_stride = 0;
    unsigned char* tmp_buffer = NULL;
    int rgb_size;

    //unsigned char* jpg_buffer = NULL;
    //unsigned int jpg_size = 0;
    //size_t ret = 0;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);
        return -1;
    }

    jpeg_create_decompress(&cinfo);

    // libjpeg指定图片内存
    jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);

    jpeg_read_header(&cinfo, TRUE);

    //cinfo.out_color_space = JCS_RGB; //JCS_YCbCr;  // 设置输出格式

    jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;
    *width = cinfo.output_width;
    *height = cinfo.output_height;

    rgb_size = row_stride * cinfo.output_height; // 总大小
    *size = rgb_size;

    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    *rgb_buffer = (unsigned char *)malloc(sizeof(char) * rgb_size);    // 分配总内存

    printf("debug--:\nrgb_size: %d, size: %d w: %d h: %d row_stride: %d \n", rgb_size,
        cinfo.image_width*cinfo.image_height*3,
        cinfo.image_width, 
        cinfo.image_height,
        row_stride);
    tmp_buffer = *rgb_buffer;
    while (cinfo.output_scanline < cinfo.output_height) // 解压每一行
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        // 复制到内存
        memcpy(tmp_buffer, buffer[0], row_stride);
        tmp_buffer += row_stride;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return 0;
}

int write_jpg_file(const char* jpg_file, unsigned char* rgb_buffer, int width, int height, int quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    int row_stride = 0;
    FILE* fp = NULL;
    JSAMPROW row_pointer[1];

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);
    fp = fopen(jpg_file, "wb");
    if (fp == NULL)
    {
        printf("open file %s failed.\n", jpg_file);
        return -1;
    }
    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, 1);  // todo 1 == true
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = width * cinfo.input_components;

    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = &rgb_buffer[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);

    return 0;
}
