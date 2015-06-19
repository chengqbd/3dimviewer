///////////////////////////////////////////////////////////////////////////////
// $Id$
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

#include <osg/COnOffNode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void osg::COnOffNode::traverse(NodeVisitor &nv) void osg::COnOffNode::setOnOffState(bool newState)
///
/// \brief  ! Accept node visitor. 
///
/// \param [in,out] nv  The nv. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void osg::COnOffNode::traverse(NodeVisitor &nv)
{
    if( m_bIsOn )
    {
        Switch::traverse(nv);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void osg::COnOffNode::setOnOffState(bool newState)
///
/// \brief  ! Set state. 
///
/// \param  newState    State of the new. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void osg::COnOffNode::setOnOffState(bool newState)
{
    if( m_bIsOn != newState )
    {
/*        if( newState )
            setAllChildrenOn();
        else
            setAllChildrenOff();

            */
        m_bIsOn = newState;
    }
}

