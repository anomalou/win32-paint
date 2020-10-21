#define UNICODE

#include <windows.h>
#include <windowsx.h>

#define DR_CIRCLE 1000
#define DR_SQUARE 1001
#define DR_LINE 1002

#define DC_RED 1003
#define DC_BLUE 1004
#define DC_GREEN 1005

#define SECOND 2000

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void CreateMainWindow();

HWND mainHWND;

HINSTANCE hInst;

HINSTANCE library;

boolean mouseDown;

struct Setup{
    struct Node* currentNode;
    byte currentFigure;
    byte currentColor;
};

struct Parametrs{
    byte figure;
    byte color;
    RECT pos;
};

struct Node{
    struct Parametrs param;
    struct Node* next;
};

struct Node* head;

struct Node* AddNode(struct Parametrs);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE lhInstance, LPSTR commands, INT number){

    hInst = hInstance;

    CreateMainWindow();


    MSG msg = {};
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void CreateMainWindow(){
    WNDCLASS class = {};
    class.lpszClassName = L"MainClass";
    class.hCursor = LoadCursor(NULL, IDC_ARROW);
    class.hInstance = hInst;
    class.lpfnWndProc = WinProc;
    class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&class);

    mainHWND = CreateWindow(L"MainClass", L"Drawer", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInst, NULL);

    HMENU menu = CreateMenu();

    HWND popup = CreatePopupMenu();

    AppendMenuW(popup, MF_STRING, DR_CIRCLE, L"&Circle");
    AppendMenuW(popup, MF_STRING, DR_SQUARE, L"&Square");
    AppendMenuW(popup, MF_STRING, DR_LINE, L"&Line");
    AppendMenuW(menu, MF_STRING | MF_POPUP, popup, L"&Figures");

    popup = CreatePopupMenu();

    AppendMenuW(popup, MF_STRING, DC_RED, L"&Red");
    AppendMenuW(popup, MF_STRING, DC_BLUE, L"&Blue");    
    AppendMenuW(popup, MF_STRING, DC_GREEN, L"&Green");
    AppendMenuW(menu, MF_STRING | MF_POPUP, popup, L"&Colors");

    SetMenu(mainHWND, menu);

    ShowWindow(mainHWND, SW_NORMAL);

    head = NULL;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    POINT p;
    struct Setup* setup;

    if(msg == WM_CREATE){
        setup = malloc(sizeof(struct Setup));
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)setup);
        library = LoadLibraryA("drawer.dll");
        setup->currentColor = 1;
        setup->currentFigure = 1;
        setup->currentNode = NULL;
        mouseDown = FALSE;
        void (*init)();
        init = (void(*)())GetProcAddress(library, "InitBrushs");
        init();
    }else{
        setup = (struct Setup*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    switch(msg){
        case WM_COMMAND:
        {
            switch(LOWORD(wParam)){
                case DR_CIRCLE:
                    setup->currentFigure = 1;
                break;
                case DR_SQUARE:
                    setup->currentFigure = 2;
                break;
                case DR_LINE:
                    setup->currentFigure = 3;
                break;
                case DC_RED:
                    setup->currentColor = 1;
                break;
                case DC_BLUE:
                    setup->currentColor = 2;
                break;
                case DC_GREEN:
                    setup->currentColor = 3;
                break;
            }
        }
        break;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT paintRect;
            GetClientRect(hwnd, &paintRect);
            HDC hdc = BeginPaint(hwnd, &ps);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP hBM = CreateCompatibleBitmap(hdc, paintRect.right, paintRect.bottom);
            int saveDC = SaveDC(memDC);
            SelectObject(memDC, hBM);
            FillRect(memDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            struct Node* node = head;
            void (*draw)(HDC hdc, int color, RECT rect);
            while(node != NULL){
                switch(node->param.figure){
                    case 1:
                        draw = (void(*)(HDC hdc, int color, RECT rect))GetProcAddress(library, "DrawCircle");
                    break;
                    case 2:
                        draw = (void(*)(HDC hdc, int color, RECT rect))GetProcAddress(library, "DrawSquare");
                    break;
                    case 3:
                        draw = (void(*)(HDC hdc, int color, RECT rect))GetProcAddress(library, "DrawLine");
                    break;
                }
                draw(memDC, node->param.color, node->param.pos);
                node = node->next;
            }
            BitBlt(hdc, 0, 0, paintRect.right, paintRect.bottom, memDC, 0, 0, SRCCOPY);
            RestoreDC(memDC, saveDC);
            DeleteObject(hBM);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
        }
        break;
        case WM_LBUTTONDOWN:
        {
            struct Parametrs figure;
            mouseDown = TRUE;
            p.x = GET_X_LPARAM(lParam);
            p.y = GET_Y_LPARAM(lParam);

            figure.color = setup->currentColor;
            figure.figure = setup->currentFigure;
            figure.pos.left = p.x;
            figure.pos.top = p.y;
            figure.pos.right = p.x;
            figure.pos.bottom = p.y;

            setup->currentNode = AddNode(figure);
        }
        break;
        case WM_MOUSEMOVE:
        {
            if(mouseDown == TRUE){
                p.x = GET_X_LPARAM(lParam);
                p.y = GET_Y_LPARAM(lParam);

                setup->currentNode->param.pos.right = p.x;
                setup->currentNode->param.pos.bottom = p.y;

                RECT paintRect;
                GetClientRect(hwnd, &paintRect);

                InvalidateRect(hwnd, &paintRect, TRUE);
            }
        }
        break;
        case WM_LBUTTONUP:
        {
            mouseDown = FALSE;
            p.x = GET_X_LPARAM(lParam);
            p.y = GET_Y_LPARAM(lParam);

            setup->currentNode = NULL;
        }
        break;
        case WM_RBUTTONDOWN:
        {   
            RECT paintRect;
            GetClientRect(hwnd, &paintRect);
            ClearList();
            InvalidateRect(hwnd, &paintRect, FALSE);
        }
        break;
        case WM_CLOSE:
        {
            void (*free)();
            free = (void(*)())GetProcAddress(library, "FreeBrushs");
            FreeLibrary(library);
            DestroyWindow(hwnd);
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

struct Node* AddNode(struct Parametrs param){
    if(head == NULL){
        head = malloc(sizeof(struct Node));
        head->param = param;
        head->next = NULL;
        return head;
    }else{
        struct Node* node = head;
        while(node->next != NULL){
            node = node->next;
        }
        node->next = malloc(sizeof(struct Node));
        node = node->next;
        node->param = param;
        node->next = NULL;
        return node;
    }
}

void ClearList(){
    head = NULL;
}