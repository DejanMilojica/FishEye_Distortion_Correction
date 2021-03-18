# Program specification

Input image have to be yuv image
Input and output image, have to be in data folder!
Lens specification file, have to be in data folder!

## Lens Specification file

Lens Spec file have to be csv file, with just two column usage.

##Organisation description:

- First field, represent focal length of camera in milimeteres,
- Second field, represent sensor pixel pitch in mm,
- Third field, represent scaling factor, that can be combinated with focal length.
- After three fields, every in separate row, next two columns represents parameters that are unique
  for every lens. That columns are field angle / hieght space in mm.

# Run program:
ldc_tool.out -i ../data/image.YUV -o ../data/image_COPY.YUV -p ../data/LensSpec.csv -w 1920 -h 1080 -f 1

ldc_tool.out -i ../data/image.YUV -o ../data/image_COPY.YUV -p ../data/LensSpec.csv -w 1920 -h 1080 -f 2
