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

////////////////////////////////////////////////////////////
// Includes

#include <alg/CSmoothing.h>

#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>

////////////////////////////////////////////////////////////
//

bool CSmoothing::Smooth(data::CMesh &mesh, int loops)
{
/*
    setProgressMax(loops + 1);
    beginProgress();
    progress();

    OpenMesh::Smoother::JacobiLaplaceSmootherT<data::CMesh> smoother(mesh);
    OpenMesh::Smoother::SmootherT<data::CMesh>::Component component = OpenMesh::Smoother::SmootherT<data::CMesh>::Tangential_and_Normal;
    OpenMesh::Smoother::SmootherT<data::CMesh>::Continuity continuity = OpenMesh::Smoother::SmootherT<data::CMesh>::C1;
    smoother.initialize(component, continuity);

    // smoothing cycle
    for (int smooth_iter = 0; smooth_iter < loops; ++smooth_iter)
    {
        smoother.smooth(1);

        // set progress value and test function termination
        if (!progress())
        {
            return false;
        }
    }

    // finish function progress
    endProgress();

    // OK
    return true;
*/

    // smoothing progress setting
    setProgressMax(loops + 1);
    beginProgress();
    progress();

    // add mesh custom property - new vertex position
    OpenMesh::VPropHandleT<data::CMesh::Point>      new_positions_prop;
    mesh.add_property(new_positions_prop);

    data::CMesh::VertexIter                         v_it, v_end(mesh.vertices_end());               // mesh vertex iterator and end iterator
    data::CMesh::VertexVertexIter                   vv_it;                                          // vertex circulator
    data::CMesh::Point                              centr_point;                                    // position of actual (center) point
    data::CMesh::Point                              vertex_vector;                                  // vector from actual (center) point to actual adjacent vertex

    double                                          vertex_vector_length;                           // distance of actual vertex and adjacent actual vertex
    double                                          distance_weight;                                // actual adjacent distance weight
    double                                          normalisation_weight;                           // vertex normalisation weight, sum of adjacent vertices weight
    double                                          smooth_factor;                                  // used smoothing factor


    // smoothing iterations cycle
    for (int i = 0; i < loops; ++i)
    {
        // set first Taubin smoothing parameter
        smooth_factor = m_taubins_lambda;

        // Taubin smoothing cycle
        for (int ts = 0; ts < 2; ++ts)
        {
            // mesh vertices cycle
            for (v_it = mesh.vertices_begin(); v_it != v_end; ++v_it)
            {      
                mesh.property(new_positions_prop, v_it).vectorize(0.0f);
                normalisation_weight = 0.0;
                centr_point = mesh.point(v_it);
      
                // ring of vertices around actual vertex cycle
                for (vv_it = mesh.vv_iter(v_it); vv_it; ++vv_it)
                {
                    // adjacent vertex vector calculation
                    vertex_vector = mesh.point(vv_it) - centr_point;
                    // adjacent vertex vector length
                    vertex_vector_length = vertex_vector.length();
                     // distance weight calculation
                    distance_weight = exp((vertex_vector_length * vertex_vector_length) / (-2.0 * m_squared_smooth_factor));
                    normalisation_weight += distance_weight;
                    // adjacent vertex position cumulation
                    mesh.property(new_positions_prop, v_it) += (vertex_vector * distance_weight);
                }

                // actual vertex new position weighting
                mesh.property(new_positions_prop, v_it) *= (smooth_factor / normalisation_weight);
                mesh.property(new_positions_prop, v_it) += centr_point;
            }
    
            // mesh vertices cycle to realise smoothing step
            for (v_it = mesh.vertices_begin(); v_it != v_end; ++v_it)
            {
                // boundary vertex test
                if ( !mesh.is_boundary(v_it) )
                    // set new position for actual vertex
                    mesh.set_point( v_it, mesh.property(new_positions_prop,v_it) );
            }

            // set second Taubin smoothing parameter
            smooth_factor = m_taubins_mu;
        }

        // set progress value and test function termination
        if (!progress())
        {
            // free mesh custom property - new vertex position
            mesh.remove_property(new_positions_prop);
            // function break
            return false;
        }
    }

    // free mesh custom property - new vertex position
    mesh.remove_property(new_positions_prop);

    // finish function progress
    endProgress();

    // OK
    return true;
}