///////////////////////////////////////////////////////////////////////////////
// $Id$
//
// This file comes from 3DimViewer software and was modified for 
// 
// BlueSkyPlan version 3.x
// Diagnostic and implant planning software for dentistry.
//
// The original 3DimViewer license can be found below.
//
// Changes are Copyright 2012 Blue Sky Bio, LLC
// All rights reserved 
//
// Changelog:
//    [2012/mm/dd] - ...
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// 3DimViewer
// Lightweight 3D DICOM viewer.
//
// Copyright 2008-2012 3Dim Laboratory s.r.o.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DBOUT_H_INCLUDED
#define DBOUT_H_INCLUDED

#ifdef _MSC_VER

#include <windows.h>
#include <iostream>
#include <sstream>

//! Trace macro used to output debug information to the visual studio output window.
#define DBOUT( s )            \
    {                             \
    std::wostringstream os_;    \
    os_ << s << std::endl;                   \
    OutputDebugString( os_.str().c_str() );  \
    }
#else

#define DBOUT( s )

#endif

//DBOUT_H_INCLUDED
#endif 
