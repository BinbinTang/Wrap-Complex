; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Skin.h"
LastPage=0

ClassCount=6
Class1=CSkinApp
Class2=CSkinDoc
Class3=CSkinView
Class4=CMainFrame

ResourceCount=3
Resource1=IDD_ASKFORALPHA
Class5=CAboutDlg
Resource2=IDD_ABOUTBOX
Class6=CAlphaDlg
Resource3=IDR_MAINFRAME

[CLS:CSkinApp]
Type=0
HeaderFile=Skin.h
ImplementationFile=Skin.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CSkinDoc]
Type=0
HeaderFile=SkinDoc.h
ImplementationFile=SkinDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC
LastObject=CSkinDoc

[CLS:CSkinView]
Type=0
HeaderFile=SkinView.h
ImplementationFile=SkinView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame




[CLS:CAboutDlg]
Type=0
HeaderFile=Skin.cpp
ImplementationFile=Skin.cpp
Filter=D
LastObject=ID_FILE_SAVE

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=ID_VIEW_VERTICES
Command17=ID_VIEW_EDGES
Command18=ID_VIEW_TRIANGLES
Command19=ID_VIEW_TETRAHEDRA
Command20=ID_ROTATE_START
Command21=ID_ROTATE_STOP
Command22=ID_APP_ABOUT
Command23=ID_OP_DELTRI
Command24=ID_OP_GENSKIN
Command25=ID_OP_DUMPSKINPPOINTS
CommandCount=25

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

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_OP_DELTRI
Command8=ID_FILE_PRINT
Command9=ID_APP_ABOUT
CommandCount=9

[DLG:IDD_ASKFORALPHA]
Type=1
Class=CAlphaDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_ALPHAVALUE,edit,1350631552
Control4=IDC_STATIC,static,1342308352

[CLS:CAlphaDlg]
Type=0
HeaderFile=AlphaDlg.h
ImplementationFile=AlphaDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CAlphaDlg

