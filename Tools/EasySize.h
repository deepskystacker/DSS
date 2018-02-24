/*===================================================*\
|                                                     |
|  EASY-SIZE Macros                                   |
|                                                     |
|  Copyright (c) 2001 - Marc Richarme                 |
|      devix@devix.cjb.net                            |
|      http://devix.cjb.net                           |
|                                                     |
|  License:                                           |
|                                                     |
|  You may use this code in any commersial or non-    |
|  commersial application, and you may redistribute   |
|  this file (and even modify it if you wish) as      |
|  long as you keep this notice untouched in any      |
|  version you redistribute.                          |
|                                                     |
|  Usage:                                             |
|                                                     |
|  - Insert 'DECLARE_EASYSIZE' somewhere in your      |
|    class declaration                                |
|  - Insert an easysize map in the beginning of your  |
|    class implementation (see documentation) and     |
|    outside of any function.                         |
|  - Insert 'INIT_EASYSIZE;' in your                  |
|    OnInitDialog handler.                            |
|  - Insert 'UPDATE_EASYSIZE' in your OnSize handler  |
|  - Optional: Insert 'EASYSIZE_MINSIZE(mx,my);' in   |
|    your OnSizing handler if you want to specify     |
|    a minimum size for your dialog                   |
|                                                     |
|        Check http://devix.cjb.net for the           |
|              docs and new versions                  |
|                                                     |
\*===================================================*/


#ifndef __EASYSIZE_H_
#define __EASYSIZE_H_
#define ES_BORDER 0xffffffff
#define ES_KEEPSIZE 0xfffffffe
#define ES_HCENTER 0x00000001
#define ES_VCENTER 0x00000002
#define DECLARE_EASYSIZE \
void __ES__RepositionControls(BOOL bInit);\
void __ES__CalcBottomRight(CWnd *pThis, BOOL bBottom, int &bottomright, int &topleft, UINT id, UINT br, int es_br, CRect &rect, int clientbottomright);
#define INIT_EASYSIZE __ES__RepositionControls(TRUE); __ES__RepositionControls(FALSE)
#define UPDATE_EASYSIZE if(GetWindow(GW_CHILD)!=NULL) __ES__RepositionControls(FALSE)
#define EASYSIZE_MINSIZE(mx,my,s,r) if(r->right-r->left < mx) { if((s == WMSZ_BOTTOMLEFT)||(s == WMSZ_LEFT)||(s == WMSZ_TOPLEFT)) r->left = r->right-mx; else r->right = r->left+mx; } if(r->bottom-r->top < my) { if((s == WMSZ_TOP)||(s == WMSZ_TOPLEFT)||(s == WMSZ_TOPRIGHT)) r->top = r->bottom-my; else r->bottom = r->top+my; }
#define BEGIN_EASYSIZE_MAP(class) \
void class::__ES__CalcBottomRight(CWnd *pThis, BOOL bBottom, int &bottomright, int &topleft, UINT id, UINT br, int es_br, CRect &rect, int clientbottomright) {\
if(br==ES_BORDER) bottomright = clientbottomright-es_br;\
else if(br==ES_KEEPSIZE) bottomright = topleft+es_br;\
else { CRect rect2;\
pThis->GetDlgItem(br)->GetWindowRect(rect2); pThis->ScreenToClient(rect2);\
bottomright = (bBottom?rect2.top:rect2.left) - es_br;}}\
void class::__ES__RepositionControls(BOOL bInit) { CRect rect,rect2,client; GetClientRect(client);
#define END_EASYSIZE_MAP Invalidate(); UpdateWindow(); }
#define EASYSIZE(id,l,t,r,b,o) \
static int id##_es_l, id##_es_t, id##_es_r, id##_es_b;\
if(bInit) {\
GetDlgItem(id)->GetWindowRect(rect); ScreenToClient(rect);\
if(o & ES_HCENTER) id##_es_l = rect.Width()/2; else {\
if(l==ES_BORDER) id##_es_l = rect.left; else if(l==ES_KEEPSIZE) id##_es_l = rect.Width(); else {\
	GetDlgItem(l)->GetWindowRect(rect2); ScreenToClient(rect2);\
	id##_es_l = rect.left-rect2.right;}}\
if(o & ES_VCENTER) id##_es_t = rect.Height()/2; else {\
if(t==ES_BORDER) id##_es_t = rect.top; else if(t==ES_KEEPSIZE) id##_es_t = rect.Height(); else {\
	GetDlgItem(t)->GetWindowRect(rect2); ScreenToClient(rect2);\
	id##_es_t = rect.top-rect2.bottom;}}\
if(o & ES_HCENTER) id##_es_r = rect.Width(); else { if(r==ES_BORDER) id##_es_r = client.right-rect.right; else if(r==ES_KEEPSIZE) id##_es_r = rect.Width(); else {\
	GetDlgItem(r)->GetWindowRect(rect2); ScreenToClient(rect2);\
	id##_es_r = rect2.left-rect.right;}}\
if(o & ES_VCENTER) id##_es_b = rect.Height(); else  { if(b==ES_BORDER) id##_es_b = client.bottom-rect.bottom; else if(b==ES_KEEPSIZE) id##_es_b = rect.Height(); else {\
	GetDlgItem(b)->GetWindowRect(rect2); ScreenToClient(rect2);\
	id##_es_b = rect2.top-rect.bottom;}}\
} else {\
int left,top,right,bottom; BOOL bR = FALSE,bB = FALSE;\
if(o & ES_HCENTER) { int _a,_b;\
if(l==ES_BORDER) _a = client.left; else { GetDlgItem(l)->GetWindowRect(rect2); ScreenToClient(rect2); _a = rect2.right; }\
if(r==ES_BORDER) _b = client.right; else { GetDlgItem(r)->GetWindowRect(rect2); ScreenToClient(rect2); _b = rect2.left; }\
left = _a+((_b-_a)/2-id##_es_l); right = left + id##_es_r;} else {\
if(l==ES_BORDER) left = id##_es_l;\
else if(l==ES_KEEPSIZE) { __ES__CalcBottomRight(this,FALSE,right,left,id,r,id##_es_r,rect,client.right); left = right-id##_es_l;\
} else { GetDlgItem(l)->GetWindowRect(rect2); ScreenToClient(rect2); left = rect2.right + id##_es_l; }\
if(l != ES_KEEPSIZE) __ES__CalcBottomRight(this,FALSE,right,left,id,r,id##_es_r,rect,client.right);}\
if(o & ES_VCENTER) { int _a,_b;\
if(t==ES_BORDER) _a = client.top; else { GetDlgItem(t)->GetWindowRect(rect2); ScreenToClient(rect2); _a = rect2.bottom; }\
if(b==ES_BORDER) _b = client.bottom; else { GetDlgItem(b)->GetWindowRect(rect2); ScreenToClient(rect2); _b = rect2.top; }\
top = _a+((_b-_a)/2-id##_es_t); bottom = top + id##_es_b;} else {\
if(t==ES_BORDER) top = id##_es_t;\
else if(t==ES_KEEPSIZE) { __ES__CalcBottomRight(this,TRUE,bottom,top,id,b,id##_es_b,rect,client.bottom); top = bottom-id##_es_t;\
} else { GetDlgItem(t)->GetWindowRect(rect2); ScreenToClient(rect2); top = rect2.bottom + id##_es_t; }\
if(t != ES_KEEPSIZE) __ES__CalcBottomRight(this,TRUE,bottom,top,id,b,id##_es_b,rect,client.bottom);}\
GetDlgItem(id)->MoveWindow(left,top,right-left,bottom-top,FALSE);\
}
#endif //__EASYSIZE_H_