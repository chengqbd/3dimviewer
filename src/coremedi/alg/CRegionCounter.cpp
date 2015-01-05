///////////////////////////////////////////////////////////////////////////////
// $Id:$
//
// 3DimViewer
// Lightweight 3D DICOM viewer.
//
// Copyright 2008-2014 3Dim Laboratory s.r.o.
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

#include <core/alg/CRegionCounter.h>
#include <osg/dbout.h>

/**
 * Counts regions of mesh.
 *
 * \param [in,out]	mesh	The mesh.
 *
 * \return	.
**/
size_t CRegionCounter::count( data::CMesh &mesh )
{
	// Zero sized mesh
	if(mesh.n_faces() == 0)
		return 0;

	// queue of triangles to visit
	std::deque<data::CMesh::FaceHandle> fill_queue;

	// last used submesh index
	size_t last_region(0);

	// Create flags vector
	std::vector<int> flags(mesh.n_faces());
	flags.assign(mesh.n_faces(), -1);

	// For all faces
	data::CMesh::FaceIter itFace(mesh.faces_begin()), itEnd(mesh.faces_end());
	while (itFace != itEnd)
	{
		// Get face handle
		data::CMesh::FaceHandle fhandle(itFace.handle());
		++itFace;

		// Already set face, advance
		if(flags[fhandle.idx()] != -1)
			continue;

		// Start "fill" algorithm
		fill_queue.push_back(fhandle);

		// "Fill" region
		while (!fill_queue.empty())
		{
			data::CMesh::FaceHandle curr = fill_queue.front();
			fill_queue.pop_front();

			// Test if already filled
			if(flags[curr.idx()] != -1)
				continue;

			// "Color" face
			flags[curr.idx()] = last_region;

			// get all neighbors and store to queue
			for (data::CMesh::FaceFaceIter ffit = mesh.ff_begin(curr); ffit != mesh.ff_end(curr); ++ffit)
			{
				data::CMesh::FaceHandle neighbour = ffit.handle();
				fill_queue.push_back(neighbour);
			}
		}
	
		++last_region;
	}

	DBOUT("Regions: " << last_region);
	return last_region;
}
