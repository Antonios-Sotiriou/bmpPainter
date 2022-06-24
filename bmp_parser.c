#include <stdio.h>
#include <stdlib.h>
#include "header_files/bmp.h"
#include "header_files/bmp_parser.h"

BMP_Image bmp_parser() {

    BMP_Header header;
    BMP_Info info;
    Pixels pixel, *pixel_array;
    pixel_array = malloc(sizeof(Pixels));
    if (pixel_array == NULL) {
        fprintf(stderr, "Error while initializing a Pixels array!\n");
    }

    FILE *fp;
    char fname;
    char *image_data;
    BMP_Image bmp_image;
    
    printf("Enter file absolut path: \n");
    scanf("%s", &fname);
    
    fp = fopen(&fname, "rb");
    if (fp != NULL) {
        fread(&header, sizeof(BMP_Header), 1, fp);
        if (header.Type == 0x4d42) {
            printf("Header type: %0x\n", header.Type);
            fseek(fp, 14, SEEK_SET);
            fread(&info, sizeof(BMP_Info), 1, fp);
            if (info.Size != 0) {
                printf("Aquired Header info size: %d\n", info.Size);
                fseek(fp, 14 + info.Size, SEEK_SET);
                bmp_image.width = info.Width;
                bmp_image.height = info.Height;
                /* Allocate the amount of memory needed to store the pixel array */
                image_data = calloc(1, sizeof(char) * info.Height * info.Width * 4);
                if (image_data != NULL) {
                    int i = 0, x = 0, dynamic_inc = 2;               
                    while (1) {
                        fread(&pixel, sizeof(Pixels), 1, fp);
                        if (feof(fp)) {
                            break;
                        } else {
                            pixel_array = realloc(pixel_array, sizeof(Pixels) * dynamic_inc);
                            if (pixel_array != NULL) {
                                pixel_array[x] = pixel;
                                image_data[i] = pixel_array[x].RedPixel;
                                image_data[i + 1] = pixel_array[x].BluePixel;
                                image_data[i + 2] = pixel_array[x].GreenPixel;
                                i += 4;
                                x++;
                                dynamic_inc++;
                            } else {
                                fprintf(stderr, "Failed to allocate memory for the image data.\n");
                                bmp_image.data = "Error 4";
                                free(image_data);
                                free(pixel_array);
                                return bmp_image;
                            }
                        }
                    }
                } else {
                    fprintf(stderr, "Failed to allocate memory for the image data.\n");
                    bmp_image.data = "Error 3";
                    free(image_data);
                    return bmp_image; 
                }
            } else {
                fprintf(stderr, "File doesn't provide informations about the Header and info size.\n");
                bmp_image.data = "Error 2";
                return bmp_image;
            }
        } else {
            fprintf(stderr, "Unsuported file Extension.Provide a bmp file\n");
            bmp_image.data = "Error 1";
            return bmp_image;
        }
    }
    fclose(fp);
    free(pixel_array);
    /* Fixing image rotation and orientation */
    bmp_image.data = calloc(1, sizeof(char) * info.Height * info.Width * 4);
    int z = 0;
    int starting_point = (sizeof(char) * info.Height * info.Width * 4) - (info.Width * 4);
    int step_point = info.Width * 4;
    int end_point = sizeof(char) * info.Height * info.Width * 4;
    while (end_point != 0) {
        for (int i = starting_point; i <= end_point; i++) {
            bmp_image.data[z] = image_data[i];
            z++;
            if (z == step_point) {
                end_point = starting_point;
                starting_point = end_point - (info.Width * 4);
                step_point += info.Width * 4;
            }
        }
    }
    free(image_data);

    return bmp_image;
}

