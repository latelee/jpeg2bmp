// 测试代码，具体图片文件名称自行定义

#include <stdio.h>
#include "bmp_utils.h"
#include "jpeg_utils.h"

int bmp_to_jpg(const char* bmp_file, const char* jpg_file)
{
    unsigned char* buffer = NULL;
    int size;
    int width,height;

    read_bmp_file(bmp_file, &buffer, &size, &width, &height);
    swap_rgb(buffer, size);
    printf("size: %d, width: %d height: %d\n", size, width, height);
    write_jpg_file(jpg_file, buffer, width, height, 50);

    free(buffer);

    return 0;
}

int jpg_to_bmp(const char* jpg_file, const char* bmp_file)
{
    unsigned char* buffer = NULL;
    int size;
    int width, height;

    FILE* fp1;
    unsigned char* jpg_buffer = NULL;
    int jpg_size = 0;

#if 1

    fp1 = fopen(jpg_file, "rb");
    if (fp1 == NULL)
    {
        printf("error open file: %s.\n", jpg_file);
        return -1;
    }

    fseek(fp1, 0, SEEK_END);	// 文件尾
    jpg_size = ftell(fp1);

    fseek(fp1, 0, SEEK_SET);	// 指回文件头

    jpg_buffer = (unsigned char *)malloc(sizeof(char) * jpg_size);
    if (jpg_buffer == NULL)
    {
        printf("malloc error\n");
        return -1;
    }
    memset(jpg_buffer, 0, jpg_size);

    fread(jpg_buffer, 1, jpg_size, fp1);

    printf("==== jpg size: %d\n", jpg_size);

    // 读内存
    read_jpg_buffer(jpg_buffer, jpg_size, &buffer, &size, &width, &height);
#endif

    // 读文件
#if 0
    read_jpg_file(jpg_file, &buffer, &size, &width, &height);
#endif

    swap_rgb(buffer, size);

    write_bmp_file(bmp_file, buffer, width, height);

    free(buffer);

    if (jpg_buffer != NULL)
    {
        free(jpg_buffer);
    }

    return 0;
}

int main(int argc, char* argv[])
{
#if 1
    char src_pic[32] = {0};
    char jpg_pic[32] = {0};
    char bmp_pic[32] = {0};
    int width = 0;

    width = 512;

    sprintf(src_pic, "lenna%d.bmp", width);
    sprintf(jpg_pic, "test%d.jpg", width);
    sprintf(bmp_pic, "b%d.bmp", width);

    bmp_to_jpg(src_pic, jpg_pic);
    jpg_to_bmp(jpg_pic, bmp_pic);
#endif
    // 分析BMP图片
    //analyse_bmp_file("lenna512.bmp");
    return 0;
}