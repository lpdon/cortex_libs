#include "BMP.h"

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
  FILE *filePtr; //our file pointer
  BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
  unsigned char *bitmapImage;  //store image data
  int imageIdx=0;  //image index counter
  unsigned char tempRGB;  //our swap variable

  long inputSize;
  unsigned char* fileBuffer;
  unsigned char* bufferPtr;

  //open filename in read binary mode
  filePtr = fopen(filename,"rb");
  if (filePtr == NULL)
      return NULL;

  //yolo
  fseek(filePtr, 0, SEEK_END);
	inputSize = ftell(filePtr);
	rewind(filePtr);

	fileBuffer = malloc(inputSize);//new unsigned char[inputSize];
	fread(fileBuffer, 1, inputSize, filePtr);
	fclose(filePtr);

	bufferPtr = fileBuffer;
	memcpy(&bitmapFileHeader, bufferPtr, sizeof(BITMAPFILEHEADER));
	bufferPtr += sizeof(BITMAPFILEHEADER);

	memcpy(bitmapInfoHeader, bufferPtr, sizeof(BITMAPINFOHEADER));
	bufferPtr += sizeof(BITMAPINFOHEADER);
	bufferPtr += sizeof(bitmapFileHeader.bOffBits);

  //allocate enough memory for the bitmap image data
  bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

  //read in the bitmap image data
  //fread(bitmapImage,bitmapInfoHeader->biSizeImage,filePtr);
  memcpy(bitmapImage, bufferPtr, bitmapInfoHeader->biSizeImage);

  //make sure bitmasp image data was read
  if (bitmapImage == NULL)
  {
      //fclose(filePtr);
      return NULL;
  }

  //swap the r and b values to get RGB (bitmap is BGR)
  for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx+=3)
  {
      tempRGB = bitmapImage[imageIdx];
      bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
      bitmapImage[imageIdx + 2] = tempRGB;
  }

  //close file and return bitmap iamge data
  //fclose(filePtr);
  return bitmapImage;
}
