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

///////////////////////////////////////////////////////////////////////////////
// include files

#include <data/CDensityData.h>

#include <VPL/Math/Base.h>
#include <VPL/Math/StaticMatrix.h>
#include <VPL/Math/StaticVector.h>


namespace data
{

///////////////////////////////////////////////////////////////////////////////
//

CDensityData::CDensityData() : m_iSeriesNumber(0)
{
    m_volumeUndo.setVolumePtr(this);
}

///////////////////////////////////////////////////////////////////////////////
//
CDensityData::CDensityData(const CDensityData& Data)
    : vpl::img::CDensityVolume(Data)
    , m_iSeriesNumber(Data.m_iSeriesNumber)
    , m_sPatientName(Data.m_sPatientName)
    , m_sPatientId(Data.m_sPatientId)
    , m_sPatientBirthday(Data.m_sPatientBirthday)
    , m_sPatientSex(Data.m_sPatientSex)
    , m_sModality(Data.m_sModality)
    , m_sSeriesUid(Data.m_sSeriesUid)
    , m_sSeriesDate(Data.m_sSeriesDate)
    , m_sSeriesTime(Data.m_sSeriesTime)
    , m_sPatientPosition(Data.m_sPatientPosition)
    , m_sManufacturer(Data.m_sManufacturer)
    , m_sModelName(Data.m_sModelName)
    , m_ImageOrientationX(Data.m_ImageOrientationX)
    , m_ImageOrientationY(Data.m_ImageOrientationY)
    , m_ImagePosition(Data.m_ImagePosition)
    , m_ImageSubSampling(Data.m_ImageSubSampling)
	, m_sPatientDescription(Data.m_sPatientDescription)
	, m_sStudyUid(Data.m_sStudyUid)
	, m_sStudyId(Data.m_sStudyId)
	, m_sStudyDate(Data.m_sStudyDate)
	, m_sStudyDescription(Data.m_sStudyDescription)
	, m_sSeriesDescription(Data.m_sSeriesDescription)
	, m_sScanOptions(Data.m_sScanOptions)
{
    m_volumeUndo.setVolumePtr(this);
}

///////////////////////////////////////////////////////////////////////////////
//

CDensityData::~CDensityData()
{
}

////////////////////////////////////////////////////////////
//

void CDensityData::update(const CChangedEntries& Changes)
{
    // AuxData should be re-initialized because PatientData has been changed
    if( Changes.checkIdentity(data::Storage::AuxData::Id)
            && Changes.hasChanged(data::Storage::PatientData::Id)
            && !Changes.hasChanged(data::Storage::AuxData::Id) // prevent data wipe when the aux data were loaded together with patient data
            && !Changes.checkFlagAll(DENSITY_MODIFIED) )
    {
            // Re-initialize the data
            this->init();
    }
    // Does nothing...
}

////////////////////////////////////////////////////////////
//

void CDensityData::init()
{
    resize(INIT_SIZE, INIT_SIZE, INIT_SIZE);

    m_dDX = m_dDY = m_dDZ = 1.0;

    fillEntire(vpl::img::CPixelTraits<vpl::img::tDensityPixel>::getPixelMin());

    clearDicomData();
}

////////////////////////////////////////////////////////////
// There are various instances of CDensityData, therefore we have to set storageID this way

void CDensityData::initVolumeUndo(int storageID)
{    
    m_volumeUndo.setStorageId(storageID);
    m_volumeUndo.setShouldInvalidate(true);
}

////////////////////////////////////////////////////////////
//

void CDensityData::takeDicomData(vpl::img::CDicomSlice & DicomSlice)
{
    // get header data from given DICOM slice
    m_iSeriesNumber = DicomSlice.m_iSeriesNumber;
    m_sPatientName = DicomSlice.m_sPatientName;
    m_sPatientId = DicomSlice.m_sPatientId;
    m_sPatientBirthday = DicomSlice.m_sPatientBirthday;
    m_sPatientSex = DicomSlice.m_sPatientSex;
    m_sModality = DicomSlice.m_sModality;
    m_sSeriesUid = DicomSlice.m_sSeriesUid;
    m_sSeriesDate = DicomSlice.m_sSeriesDate;
    m_sSeriesTime = DicomSlice.m_sSeriesTime;

    m_sPatientPosition = DicomSlice.m_sPatientPosition;
    m_sManufacturer = DicomSlice.m_sManufacturer;
    m_sModelName = DicomSlice.m_sModelName;
    m_ImageOrientationX = DicomSlice.m_ImageOrientationX;
    m_ImageOrientationY = DicomSlice.m_ImageOrientationY;
    m_ImageOrientationX.normalize();
    m_ImageOrientationY.normalize();
    m_ImagePosition.setXYZ(0.0, 0.0, 0.0);
    m_ImageSubSampling.setXYZ(1.0, 1.0, 1.0);

	m_sPatientDescription = DicomSlice.m_sPatientDescription;
	m_sStudyUid = DicomSlice.m_sStudyUid;
	m_sStudyId = DicomSlice.m_sStudyId;
	m_sStudyDate = DicomSlice.m_sStudyDate;
	m_sStudyDescription = DicomSlice.m_sStudyDescription;
	m_sSeriesDescription = DicomSlice.m_sSeriesDescription;
	m_sScanOptions = DicomSlice.m_sScanOptions;
}

void CDensityData::setImagePosition(double x, double y, double z)
{
    m_ImagePosition.setXYZ(x, y, z);
}

////////////////////////////////////////////////////////////
//

void CDensityData::clearDicomData()
{
    m_iSeriesNumber = 0;
    m_sPatientName.clear();
    m_sPatientId.clear();
    m_sPatientBirthday.clear();
    m_sPatientSex.clear();
    m_sModality.clear();
    m_sSeriesUid.clear();
    m_sSeriesDate.clear();
    m_sSeriesTime.clear();
    m_sPatientPosition.clear();
    m_sManufacturer.clear();
    m_sModelName.clear();
    m_ImageOrientationX = vpl::img::CVector3D(1.0, 0.0, 0.0);
    m_ImageOrientationY = vpl::img::CVector3D(0.0, 1.0, 0.0);
    m_ImagePosition.setXYZ(0.0, 0.0, 0.0);
    m_ImageSubSampling.setXYZ(1.0, 1.0, 1.0);
	m_sPatientDescription.clear();
	m_sStudyUid.clear();
	m_sStudyId.clear();
	m_sStudyDate.clear();
	m_sStudyDescription.clear();
	m_sSeriesDescription.clear();
	m_sScanOptions.clear();
}

} // namespace data

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
