///////////////////////////////////////////////////////////////////////////////
// $Id$
//
// 3DimViewer
// Lightweight 3D DICOM viewer.
//
// Copyright 2008-2016 3Dim Laboratory s.r.o.
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

#ifndef QtGuiSignals_H
#define QtGuiSignals_H

///////////////////////////////////////////////////////////////////////////////
// include files

#include <VPL/Module/GlobalSignal.h>

#include <QString>


///////////////////////////////////////////////////////////////////////////////
// definition of global signals

VPL_DECLARE_SIGNAL_4(21, void, const QString&, const QString&, const QString&, const QString&, SigLogCustomEvent);


#endif // QtGuiSignals_H

///////////////////////////////////////////////////////////////////////////////