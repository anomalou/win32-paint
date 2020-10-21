#include <windows.h>
#define export __declspec(dllexport)

HBRUSH redBrush;
HBRUSH blueBrush;
HBRUSH greenBrush;
HPEN redPen;
HPEN bluePen;
HPEN greenPen;

export void InitBrushs(){
    redBrush = CreateSolidBrush(RGB(255, 0, 0));
    blueBrush = CreateSolidBrush(RGB(0, 0, 255));
    greenBrush = CreateSolidBrush(RGB(0, 255, 0));
    redPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
    bluePen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
    greenPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
}

export void DrawSquare(HDC hdc, int color, RECT rect){
    if(hdc != NULL){
        switch(color){
            case 1:
                FillRect(hdc, &rect, redBrush);
            break;
            case 2:
                FillRect(hdc, &rect, blueBrush);
            break;
            case 3:
                FillRect(hdc, &rect, greenBrush);
            break;
        }
    }
}

export void DrawCircle(HDC hdc, int color, RECT rect){
    if(hdc != NULL){
        switch(color){
            case 1:
                SelectObject(hdc, redBrush);
                SelectObject(hdc, redPen);
            break;
            case 2:
                SelectObject(hdc, blueBrush);
                SelectObject(hdc, bluePen);
            break;
            case 3:
                SelectObject(hdc, greenBrush);
                SelectObject(hdc, greenPen);
            break;
        }
        Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
    }
}

export void DrawLine(HDC hdc, int color, RECT rect){
    if(hdc != NULL){
        switch(color){
            case 1:
                SelectObject(hdc, redPen);
            break;
            case 2:
                SelectObject(hdc, bluePen);
            break;
            case 3:
                SelectObject(hdc, greenPen);
            break;
        }
        MoveToEx(hdc, rect.left, rect.top, NULL);
        LineTo(hdc, rect.right, rect.bottom);
    }
}

export void FreeBrushs(){
    DeleteObject(redBrush);
    DeleteObject(blueBrush);
    DeleteObject(greenBrush);
    DeleteObject(redPen);
    DeleteObject(bluePen);
    DeleteObject(greenPen);
}
