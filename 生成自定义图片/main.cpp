
#include <windows.h>
#include <gdiplus.h>
#include <stdint.h>
#include <string>
#include <cstdlib>

#pragma comment(lib, "gdiplus.lib")
int GetEncoderClsid(const WCHAR* format, CLSID& clsid);
bool SaveBitmapToPng(Gdiplus::Bitmap* bitmap, const char* filename) {

    // Create the GDI+ encoder parameters
    CLSID pngClsid;
    int pngEncoder = GetEncoderClsid(L"image/png", pngClsid);
    if (pngEncoder == -1) {
        return false; // PNG encoder not found
    }

    WCHAR buf[256];
    mbstowcs_s(nullptr, buf, 256, filename, 255);

    // Save the bitmap to the file
    Gdiplus::Status status = bitmap->Save(buf, &pngClsid);
    return (status == Gdiplus::Ok);
}

int GetEncoderClsid(const WCHAR* format, CLSID& clsid) {
    UINT num = 0;          // Number of image encoders
    UINT size = 0;         // Size of the image encoder array

    Gdiplus::ImageCodecInfo* imageCodecInfo = nullptr;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) {
        return -1;  // Failure
    }

    imageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (imageCodecInfo == nullptr) {
        return -1;  // Failure
    }

    Gdiplus::GetImageEncoders(num, size, imageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(imageCodecInfo[j].MimeType, format) == 0) {
            clsid = imageCodecInfo[j].Clsid;
            free(imageCodecInfo);
            return j;  // Success
        }
    }

    free(imageCodecInfo);
    return -1;  // Failure
}

void mainaction() {
    Gdiplus::Bitmap bmp(1864, 1864, PixelFormat32bppARGB);
    size_t w = bmp.GetWidth();
    size_t h = bmp.GetHeight();
    size_t midx = w / 2, midy = h / 2;
    size_t edgedst = midx * midx;//midx * midx + midy * midy;
    Gdiplus::Color edgecolor(0xff000000);
    Gdiplus::Color midcolor(0xffffffff);
    for (size_t y = 0; y < w; ++y) {
        for (size_t x = 0; x < h; ++x) {
            uint8_t a, r, g, b;
            double edgeweight = (double)((x - midx) * (x - midx) + (y - midy) * (y - midy)) / edgedst, midweight = 1.0 - edgeweight;
            a = edgecolor.GetA() * edgeweight + midcolor.GetA() * midweight;
            r = edgecolor.GetR() * edgeweight + midcolor.GetR() * midweight;
            g = edgecolor.GetG() * edgeweight + midcolor.GetG() * midweight;
            b = edgecolor.GetB() * edgeweight + midcolor.GetB() * midweight;
            bmp.SetPixel(x, y, Gdiplus::Color(a, r, g, b));
        }
    }
    SaveBitmapToPng(&bmp, "output.png");
}

int main() {
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output;
    ULONG_PTR token;
    Gdiplus::GdiplusStartup(&token, &input, &output);
    mainaction();
	Gdiplus::GdiplusShutdown(token);
}
