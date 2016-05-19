#include "myallheader.h"
#include "myutil.h"

void DetectOCR(char *img)
{
	char *outText;
	//const char *path = "/tessdate";
		TessBaseAPI *api = new TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	// Open input image with leptonica library
	Pix *image = pixRead(img);
	//Pix *image = pixRead("cleanroi.jpg");
	api->SetImage(image);
	// Get OCR result

	outText = api->GetUTF8Text();

	printf("OCR output:\n%s", outText);

	printf("¹øÈ£ÆÇ : ");
	for(int i=0; i<strlen(outText); i++){
		if(outText[i] >= '0' && outText[i] <= '9'){
			printf("%c ",outText[i]);
		}
	}printf("\n");


	// Destroy used object and release memory
	api->End();
	//delete[] outText;
	//pixDestroy(&image);
}