# Microsoft Developer Studio Project File - Name="geometry" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=geometry - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "geometry.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "geometry.mak" CFG="geometry - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "geometry - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geometry - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "geometry - Win32 Release"

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

!ELSEIF  "$(CFG)" == "geometry - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "_LIB" /D "__DEBUG__" /D "USE_ASCII" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /FR /FD /GZ /c
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

# Name "geometry - Win32 Release"
# Name "geometry - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\animate.cpp
# End Source File
# Begin Source File

SOURCE=.\boxes.cpp
# End Source File
# Begin Source File

SOURCE=.\bvtag.cpp
# End Source File
# Begin Source File

SOURCE=.\comp.cpp
# End Source File
# Begin Source File

SOURCE=.\computil.cpp
# End Source File
# Begin Source File

SOURCE=.\edgecycleset.cpp
# End Source File
# Begin Source File

SOURCE=.\edgeset.cpp
# End Source File
# Begin Source File

SOURCE=.\fliphandler.cpp
# End Source File
# Begin Source File

SOURCE=.\geomutil.cpp
# End Source File
# Begin Source File

SOURCE=.\ihandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ksimplex.cpp
# End Source File
# Begin Source File

SOURCE=.\locate.cpp
# End Source File
# Begin Source File

SOURCE=.\orienter.cpp
# End Source File
# Begin Source File

SOURCE=.\ortribv.cpp
# End Source File
# Begin Source File

SOURCE=.\packedihandler.cpp
# End Source File
# Begin Source File

SOURCE=.\segmenttree.cpp
# End Source File
# Begin Source File

SOURCE=.\shortestpather.cpp
# End Source File
# Begin Source File

SOURCE=.\simph.cpp
# End Source File
# Begin Source File

SOURCE=.\simplex.cpp
# End Source File
# Begin Source File

SOURCE=.\simplexset.cpp
# End Source File
# Begin Source File

SOURCE=.\testint.cpp
# End Source File
# Begin Source File

SOURCE=.\tolerancer.cpp
# End Source File
# Begin Source File

SOURCE=.\trist.cpp
# End Source File
# Begin Source File

SOURCE=.\tristconnector.cpp
# End Source File
# Begin Source File

SOURCE=.\vertarray.cpp
# End Source File
# Begin Source File

SOURCE=.\vertex.cpp
# End Source File
# Begin Source File

SOURCE=.\vertset.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\animate.h
# End Source File
# Begin Source File

SOURCE=.\boxes.h
# End Source File
# Begin Source File

SOURCE=.\bvtag.h
# End Source File
# Begin Source File

SOURCE=.\cofaces.h
# End Source File
# Begin Source File

SOURCE=.\comp.h
# End Source File
# Begin Source File

SOURCE=.\computil.h
# End Source File
# Begin Source File

SOURCE=.\edgecycleset.h
# End Source File
# Begin Source File

SOURCE=.\edgeset.h
# End Source File
# Begin Source File

SOURCE=.\fliphandler.h
# End Source File
# Begin Source File

SOURCE=.\geomutil.h
# End Source File
# Begin Source File

SOURCE=.\ihandler.h
# End Source File
# Begin Source File

SOURCE=.\ksimplex.h
# End Source File
# Begin Source File

SOURCE=.\locate.h
# End Source File
# Begin Source File

SOURCE=.\modtrinfo.h
# End Source File
# Begin Source File

SOURCE=.\orienter.h
# End Source File
# Begin Source File

SOURCE=.\ortribv.h
# End Source File
# Begin Source File

SOURCE=.\packedihandler.h
# End Source File
# Begin Source File

SOURCE=.\segmenttree.h
# End Source File
# Begin Source File

SOURCE=.\shortestpather.h
# End Source File
# Begin Source File

SOURCE=.\simph.h
# End Source File
# Begin Source File

SOURCE=.\simplex.h
# End Source File
# Begin Source File

SOURCE=.\simplexset.h
# End Source File
# Begin Source File

SOURCE=.\testint.h
# End Source File
# Begin Source File

SOURCE=.\tolerancer.h
# End Source File
# Begin Source File

SOURCE=.\trist.h
# End Source File
# Begin Source File

SOURCE=.\tristconnector.h
# End Source File
# Begin Source File

SOURCE=.\tristmodifier.h
# End Source File
# Begin Source File

SOURCE=.\vertarray.h
# End Source File
# Begin Source File

SOURCE=.\vertex.h
# End Source File
# Begin Source File

SOURCE=.\vertset.h
# End Source File
# End Group
# End Target
# End Project
