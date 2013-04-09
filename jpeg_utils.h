/**
 * @file   jpeg_utils.h
 * @author Late Lee 
 * @date   2012-7-1 12:47:56
 * @brief  
 *         使用libjpeg库读写JPEG图片文件。
 *
 * @note   解压出的数据排序为RGB
 *
 */

#ifndef _JPEG_BMP_H
#define _JPEG_BMP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 利用libjpeg读取JPEG图片文件
 * 
 * @param[IN]   jpg_fle    JPEG图片文件名称
 * 
 * @param[OUT]  rgb_buffer JPEG图片RGB数据
 * @param[OUT]  size       RGB数据大小
 * @param[OUT]  width      图片宽
 * @param[OUT]  height     图片高
 *
 * @return 
 *         0：成功
 *         -1：读取文件失败，或申请内存失败
 * @note
 *        1、rgb_buffer为二级指针，内存由该函数分配，需要自行释放
*         2、rgb_buffer的数据排序为RGB，如需要保存成BMP，需要自行调整为BGR
 */
int read_jpg_file(const char* jpg_file, unsigned char** rgb_buffer, int* size, int* width, int* height);

/**
 * 利用libjpeg读取JPEG图片内存
 * 
 * @param[IN]   jpg_buffer JPEG图片数据
 * @param[IN]   jpg_size   JPEG图片数据大小
 * @param[OUT]  rgb_buffer RGB格式数据
 * @param[OUT]  size       RGB数据大小
 * @param[OUT]  width      图片宽
 * @param[OUT]  height     图片高
 *
 * @return 
 *         0：成功
 *         -1：读取文件失败，或申请内存失败
 * @note
 *         1、rgb_buffer为二级指针，内存由该函数分配，需要自行释放
 *         2、rgb_buffer的数据排序为RGB，如需要保存成BMP，需要自行调整为BGR
 */
int read_jpg_buffer(unsigned char* jpg_buffer, int jpg_size, unsigned char** rgb_buffer, 
                    int* size, int* width, int* height);

/**
 * 利用libjpeg转换为JPEG图片并保存
 * 
 * @param[IN]  jpg_fle    JPEG图片文件名称
 * 
 * @param[IN]  rgb_buffer JPEG图片RGB数据
 * @param[IN]  width      图片宽
 * @param[IN]  height     图片高
 * @param[IN]  quality    图片质量
 *
 * @return 
 *         0：成功
 *         -1：打开文件失败
 * @note
 *         1、图片质量未实际测试
 *         2、rgb_buffer数据排序为RGB
 */
int write_jpg_file(const char* jpg_file, unsigned char* rgb_buffer, int width, int height, int quality);

#ifdef __cplusplus
};
#endif

#endif /* _JPEG_BMP_H */