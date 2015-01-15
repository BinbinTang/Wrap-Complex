; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFormCommandView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Tool.h"
LastPage=0

ClassCount=8
Class1=CToolApp
Class2=CToolDoc
Class3=CRenderView
Class4=CMainFrame

ResourceCount=10
Resource1=IDR_MAINFRAME (English (U.S.))
Resource2=IDR_MODELTYPE
Resource3=IDR_MAINFRAME
Class5=CAboutDlg
Class6=CChildFrame
Resource4=IDD_ABOUTBOX (English (U.S.))
Resource5=IDR_MODELTYPE (English (U.S.))
Resource6=IDD_FORM_COMMAND (English (U.S.))
Class7=CFormCommandView
Resource7=IDD_FORM_COMMAND
Resource8=IDD_ASKFORALPHA
Resource9=IDD_ASKFORCQ
Class8=AskForCQ
Resource10=IDD_ABOUTBOX

[CLS:CToolApp]
Type=0
HeaderFile=Tool.h
ImplementationFile=Tool.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CToolApp

[CLS:CToolDoc]
Type=0
HeaderFile=ToolDoc.h
ImplementationFile=ToolDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC

[CLS:CRenderView]
Type=0
HeaderFile=RenderView.h
ImplementationFile=RenderView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CRenderView

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CMDIFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
LastObject=CChildFrame
BaseClass=CMDIChildWnd
VirtualFilter=mfWC

[CLS:CAboutDlg]
Type=0
HeaderFile=Tool.cpp
ImplementationFile=Tool.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_APP_ABOUT
CommandCount=5

[MNU:IDR_MODELTYPE]
Type=1
Class=CRenderView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_VIEW_STATISTICS
Command18=ID_WINDOW_NEW
Command19=ID_WINDOW_CASCADE
Command20=ID_WINDOW_TILE_HORZ
Command21=ID_WINDOW_ARRANGE
Command22=ID_WINDOW_SPLIT
Command23=ID_APP_ABOUT
CommandCount=23

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MODELTYPE (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_WINDOW_SPLIT
Command22=ID_APP_ABOUT
CommandCount=22

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_FORM_COMMAND (English (U.S.))]
Type=1
Class=CFormCommandView
ControlCount=24
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352
Control4=IDC_FRAME_COLOR_BACK,static,1342181383
Control5=IDC_CHECK_ANTIALIAS,button,1342242819
Control6=IDC_CHECK_SMOOTH,button,1342242819
Control7=IDC_RADIO_MODEL_1,button,1342177289
Control8=IDC_RADIO_MODEL_2,button,1342177289
Control9=IDC_RADIO_MODEL_0,button,1342308361
Control10=IDC_CHECK_LIGHTING,button,1342242819
Control11=IDC_STATIC,button,1342177287
Control12=IDC_CHECK_VROTATION,button,1342242819
Control13=IDC_SLIDER_X,msctls_trackbar32,1342242840
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,static,1342308352
Control16=IDC_SLIDER_Y,msctls_trackbar32,1342242840
Control17=IDC_STATIC,static,1342308352
Control18=IDC_SLIDER_Z,msctls_trackbar32,1342242840
Control19=IDC_STATIC,static,1342308352
Control20=IDC_CHECK_LINK_SCALE,button,1342242819
Control21=IDC_STATIC,static,1342308352
Control22=IDC_FRAME_COLOR_LIGHT_AMBIENT,static,1342181383
Control23=IDC_FRAME_COLOR_LIGHT_AMBIENT2,static,1342181383
Control24=IDC_FRAME_COLOR_LIGHT_AMBIENT3,static,1342181383

[CLS:CFormCommandView]
Type=0
HeaderFile=FormCommandView.h
ImplementationFile=FormCommandView.cpp
BaseClass=CFormView
Filter=D
LastObject=IDC_CHECK_VROTATION
VirtualFilter=VWC

[DLG:IDD_FORM_COMMAND]
Type=1
Class=CFormCommandView
ControlCount=19
Control1=IDC_FRAME_COLOR_BACK,static,1342246919
Control2=IDC_FRAME_COLOR_LIGHT_AMBIENT,static,1342181383
Control3=IDC_CHECK_ANTIALIAS,button,1342242819
Control4=IDC_CHECK_SMOOTH,button,1342242819
Control5=IDC_CHECK_VROTATION,button,1342373891
Control6=IDC_SLIDER_SCALE,msctls_trackbar32,1342242840
Control7=IDC_DELTRI,button,1342242816
Control8=IDC_GENSKIN,button,1342242816
Control9=IDC_SAVESTL,button,1342242816
Control10=IDC_REFINEMESH,button,1342242816
Control11=IDC_STATIC,button,1342177287
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,static,1342308352
Control15=IDC_CHANGECQ,button,1342242816
Control16=IDC_CHECK_MESH,button,1342242819
Control17=IDC_GROW_MESH,button,1342242816
Control18=IDC_FLIPALLEDGES,button,1342242816
Control19=IDC_BREATH,button,1342242819

[DLG:IDD_ASKFORALPHA]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_ALPHAVALUE,edit,1350631552
Control4=IDC_STATIC,static,1342308352

[DLG:IDD_ASKFORCQ]
Type=1
Class=AskForCQ
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_ASKFORC,edit,1350631552
Control4=IDC_ASKFORQ,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[CLS:AskForCQ]
Type=0
HeaderFile=AskForCQ.h
ImplementationFile=AskForCQ.cpp
BaseClass=CDialog
Filter=D
LastObject=AskForCQ
VirtualFilter=dWC

