# Microsoft Developer Studio Project File - Name="basic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=basic - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "basic.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "basic.mak" CFG="basic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "basic - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "basic - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "basic - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /D "_LIB" /D "USE_ASCII" /D "NDEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "basic - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "_LIB" /D "__DEBUG__" /D "USE_ASCII" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "basic - Win32 Release"
# Name "basic - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\arg.c
# End Source File
# Begin Source File

SOURCE=.\basic.c
# End Source File
# Begin Source File

SOURCE=.\binio.cpp
# End Source File
# Begin Source File

SOURCE=.\bitvector.cpp
# End Source File
# Begin Source File

SOURCE=.\callbacklist.cpp
# End Source File
# Begin Source File

SOURCE=.\callbackobject.cpp
# End Source File
# Begin Source File

SOURCE=.\cb.c
# End Source File
# Begin Source File

SOURCE=.\cb_doprnt.c
# End Source File
# Begin Source File

SOURCE=.\cmdlookup.cpp
# End Source File
# Begin Source File

SOURCE=.\command.cpp
# End Source File
# Begin Source File

SOURCE=.\comment.cpp
# End Source File
# Begin Source File

SOURCE=.\convert.cpp
# End Source File
# Begin Source File

SOURCE=.\data.cpp
# End Source File
# Begin Source File

SOURCE=.\dumpable.cpp
# End Source File
# Begin Source File

SOURCE=.\files.c
# End Source File
# Begin Source File

SOURCE=.\flexlm.cpp
# End Source File
# Begin Source File

SOURCE=.\genmatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\getarg.c
# End Source File
# Begin Source File

SOURCE=.\history.cpp
# End Source File
# Begin Source File

SOURCE=.\iit.c
# End Source File
# Begin Source File

SOURCE=.\isort.c
# End Source File
# Begin Source File

SOURCE=.\kdtree.cpp
# End Source File
# Begin Source File

SOURCE=.\license.cpp
# End Source File
# Begin Source File

SOURCE=.\logfile.cpp
# End Source File
# Begin Source File

SOURCE=.\malloc.c
# End Source File
# Begin Source File

SOURCE=.\math2.c
# End Source File
# Begin Source File

SOURCE=.\miscmath.cpp
# End Source File
# Begin Source File

SOURCE=.\nurbsdata.cpp
# End Source File
# Begin Source File

SOURCE=.\perftimer.cpp
# End Source File
# Begin Source File

SOURCE=.\points.cpp
# End Source File
# Begin Source File

SOURCE=.\prime.c
# End Source File
# Begin Source File

SOURCE=.\qsort.c
# End Source File
# Begin Source File

SOURCE=.\raindrop.c
# End Source File
# Begin Source File

SOURCE=.\rectsel.cpp
# End Source File
# Begin Source File

SOURCE=.\rgicstring.cpp
# End Source File
# Begin Source File

SOURCE=.\rgimatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\rgimessage.cpp
# End Source File
# Begin Source File

SOURCE=.\rgimessagestack.cpp
# End Source File
# Begin Source File

SOURCE=.\rgistring.cpp
# End Source File
# Begin Source File

SOURCE=.\rgitranslator.cpp
# End Source File
# Begin Source File

SOURCE=.\rgivector.cpp
# End Source File
# Begin Source File

SOURCE=.\spectrum.cpp
# End Source File
# Begin Source File

SOURCE=.\stackbv.cpp
# End Source File
# Begin Source File

SOURCE=.\stringtable.cpp
# End Source File
# Begin Source File

SOURCE=.\time.c
# End Source File
# Begin Source File

SOURCE=.\tokenize.c
# End Source File
# Begin Source File

SOURCE=.\uf.c
# End Source File
# Begin Source File

SOURCE=.\vectmat.cpp
# End Source File
# Begin Source File

SOURCE=.\vertexarray.cpp
# End Source File
# Begin Source File

SOURCE=.\vltdata.cpp
# End Source File
# Begin Source File

SOURCE=.\wfshortestpather.cpp
# End Source File
# Begin Source File

SOURCE=.\wireframe.cpp
# End Source File
# Begin Source File

SOURCE=.\xdr.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\basic.h
# End Source File
# Begin Source File

SOURCE=.\binio.h
# End Source File
# Begin Source File

SOURCE=.\bitvector.h
# End Source File
# Begin Source File

SOURCE=.\build.h
# End Source File
# Begin Source File

SOURCE=.\callbacklist.h
# End Source File
# Begin Source File

SOURCE=.\callbackobject.h
# End Source File
# Begin Source File

SOURCE=.\cmdlookup.h
# End Source File
# Begin Source File

SOURCE=.\command.h
# End Source File
# Begin Source File

SOURCE=.\comment.h
# End Source File
# Begin Source File

SOURCE=.\convert.h
# End Source File
# Begin Source File

SOURCE=.\data.h
# End Source File
# Begin Source File

SOURCE=.\dumpable.h
# End Source File
# Begin Source File

SOURCE=.\facepoint.h
# End Source File
# Begin Source File

SOURCE=.\farray.h
# End Source File
# Begin Source File

SOURCE=.\flexlm.h
# End Source File
# Begin Source File

SOURCE=.\freearray.h
# End Source File
# Begin Source File

SOURCE=.\genmatrix.h
# End Source File
# Begin Source File

SOURCE=.\history.h
# End Source File
# Begin Source File

SOURCE=.\index.h
# End Source File
# Begin Source File

SOURCE=.\iterstack.h
# End Source File
# Begin Source File

SOURCE=.\kdtree.h
# End Source File
# Begin Source File

SOURCE=.\license.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\lm_attr.h
# End Source File
# Begin Source File

SOURCE=.\lm_code.h
# End Source File
# Begin Source File

SOURCE=.\lmclient.h
# End Source File
# Begin Source File

SOURCE=.\lmpolicy.h
# End Source File
# Begin Source File

SOURCE=.\logfile.h
# End Source File
# Begin Source File

SOURCE=.\map.h
# End Source File
# Begin Source File

SOURCE=.\miscmath.h
# End Source File
# Begin Source File

SOURCE=.\multitree.h
# End Source File
# Begin Source File

SOURCE=.\nurbsdata.h
# End Source File
# Begin Source File

SOURCE=.\orindex.h
# End Source File
# Begin Source File

SOURCE=.\ortri.h
# End Source File
# Begin Source File

SOURCE=.\perftimer.h
# End Source File
# Begin Source File

SOURCE=.\points.h
# End Source File
# Begin Source File

SOURCE=.\pqueue.h
# End Source File
# Begin Source File

SOURCE=.\queue.h
# End Source File
# Begin Source File

SOURCE=.\rarray.h
# End Source File
# Begin Source File

SOURCE=.\rectsel.h
# End Source File
# Begin Source File

SOURCE=.\rgicstring.h
# End Source File
# Begin Source File

SOURCE=.\rgimatrix.h
# End Source File
# Begin Source File

SOURCE=.\rgimessage.h
# End Source File
# Begin Source File

SOURCE=.\rgimessagestack.h
# End Source File
# Begin Source File

SOURCE=.\rgistring.h
# End Source File
# Begin Source File

SOURCE=.\rgitranslator.h
# End Source File
# Begin Source File

SOURCE=.\rgitypes.h
# End Source File
# Begin Source File

SOURCE=.\rgivector.h
# End Source File
# Begin Source File

SOURCE=.\spectrum.h
# End Source File
# Begin Source File

SOURCE=.\stackbv.h
# End Source File
# Begin Source File

SOURCE=.\stringtable.h
# End Source File
# Begin Source File

SOURCE=.\tritype.h
# End Source File
# Begin Source File

SOURCE=.\unix_basic.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\vectmat.h
# End Source File
# Begin Source File

SOURCE=.\versions.h
# End Source File
# Begin Source File

SOURCE=.\vertexarray.h
# End Source File
# Begin Source File

SOURCE=.\vltdata.h
# End Source File
# Begin Source File

SOURCE=.\wfshortestpather.h
# End Source File
# Begin Source File

SOURCE=.\win_basic.h
# End Source File
# Begin Source File

SOURCE=.\wireframe.h
# End Source File
# Begin Source File

SOURCE=.\xdr.h
# End Source File
# End Group
# End Target
# End Project
