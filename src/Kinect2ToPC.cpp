// -*- C++ -*-
/*!
 * @file  Kinect2ToPC.cpp
 * @brief RTC:PCL Grabber for Kinect
 * @date $Date$
 *
 * $Id$
 */

#include "Kinect2ToPC.h"

#include <limits>
#include <iostream>
#include <string>
using namespace std;

const float FLT_NAN = std::numeric_limits<float>::quiet_NaN();

// Module specification
// <rtc-template block="module_spec">
static const char* kinect2topc_spec[] =
  {
    "implementation_id", "Kinect2ToPC",
    "type_name",         "Kinect2ToPC",
    "description",       "RTC:PCL Grabber for Kinect",
    "version",           "1.0.0",
    "vendor",            "MasutaniLab",
    "category",          "RTCPCL",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Kinect2ToPC::Kinect2ToPC(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_pcOut("pc", m_pc)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Kinect2ToPC::~Kinect2ToPC()
{
}



RTC::ReturnCode_t Kinect2ToPC::onInitialize()
{
  RTC_INFO(("onInitialize()"));
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  addOutPort("pc", m_pcOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Kinect2ToPC::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Kinect2ToPC::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Kinect2ToPC::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Kinect2ToPC::onActivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onActivated()"));
  try {
    HRESULT result;
    // Create Sensor Instance
    m_sensor = nullptr;
    result = GetDefaultKinectSensor(&m_sensor);
    if (FAILED(result)) {
      throw std::exception("Exception : GetDefaultKinectSensor()");
    }

    // Open Sensor
    result = m_sensor->Open();
    if (FAILED(result)) {
      throw std::exception("Exception : IKinectSensor::Open()");
    }

    // Retrieved Coordinate Mapper
    m_mapper = nullptr;
    result = m_sensor->get_CoordinateMapper(&m_mapper);
    if (FAILED(result)) {
      throw std::exception("Exception : IKinectSensor::get_CoordinateMapper()");
    }

    // Retrieved Color Frame Source
    IColorFrameSource* colorSource = nullptr;
    result = m_sensor->get_ColorFrameSource(&colorSource);
    if (FAILED(result)) {
      throw std::exception("Exception : IKinectSensor::get_ColorFrameSource()");
    }

    // Retrieved Depth Frame Source
    IDepthFrameSource* depthSource = nullptr;
    result = m_sensor->get_DepthFrameSource(&depthSource);
    if (FAILED(result)) {
      throw std::exception("Exception : IKinectSensor::get_DepthFrameSource()");
    }

    // Retrieved Color Frame Size
    IFrameDescription* colorDescription;
    result = colorSource->get_FrameDescription(&colorDescription);
    if (FAILED(result)) {
      throw std::exception("Exception : IColorFrameSource::get_FrameDescription()");
    }

    result = colorDescription->get_Width(&m_colorWidth); // 1920
    if (FAILED(result)) {
      throw std::exception("Exception : IFrameDescription::get_Width()");
    }

    result = colorDescription->get_Height(&m_colorHeight); // 1080
    if (FAILED(result)) {
      throw std::exception("Exception : IFrameDescription::get_Height()");
    }

    SafeRelease(colorDescription);

    // To Reserve Color Frame Buffer
    m_colorBuffer.resize(m_colorWidth * m_colorHeight);

    // Retrieved Depth Frame Size
    IFrameDescription* depthDescription;
    result = depthSource->get_FrameDescription(&depthDescription);
    if (FAILED(result)) {
      throw std::exception("Exception : IDepthFrameSource::get_FrameDescription()");
    }

    result = depthDescription->get_Width(&m_depthWidth); // 512
    if (FAILED(result)) {
      throw std::exception("Exception : IFrameDescription::get_Width()");
    }

    result = depthDescription->get_Height(&m_depthHeight); // 424
    if (FAILED(result)) {
      throw std::exception("Exception : IFrameDescription::get_Height()");
    }

    SafeRelease(depthDescription);

    // To Reserve Depth Frame Buffer
    m_depthBuffer.resize(m_depthWidth * m_depthHeight);

    // Open Color Frame Reader
    m_colorReader = nullptr;
    result = colorSource->OpenReader(&m_colorReader);
    if (FAILED(result)) {
      throw std::exception("Exception : IColorFrameSource::OpenReader()");
    }

    // Open Depth Frame Reader
    m_depthReader = nullptr;
    result = depthSource->OpenReader(&m_depthReader);
    if (FAILED(result)) {
      throw std::exception("Exception : IDepthFrameSource::OpenReader()");
    }

    SafeRelease(colorSource);
    SafeRelease(depthSource);

    m_pc.type = "xyzrgb";
    m_pc.fields.length(6);
    m_pc.fields[0].name = "x";
    m_pc.fields[0].offset = 0;
    m_pc.fields[0].data_type = PointCloudTypes::FLOAT32;
    m_pc.fields[0].count = 4;
    m_pc.fields[1].name = "y";
    m_pc.fields[1].offset = 4;
    m_pc.fields[1].data_type = PointCloudTypes::FLOAT32;
    m_pc.fields[1].count = 4;
    m_pc.fields[2].name = "z";
    m_pc.fields[2].offset = 8;
    m_pc.fields[2].data_type = PointCloudTypes::FLOAT32;
    m_pc.fields[2].count = 4;
    m_pc.fields[3].name = "b";
    m_pc.fields[3].offset = 12;
    m_pc.fields[3].data_type = PointCloudTypes::UINT8;
    m_pc.fields[3].count = 1;
    m_pc.fields[4].name = "g";
    m_pc.fields[4].offset = 13;
    m_pc.fields[4].data_type = PointCloudTypes::UINT8;
    m_pc.fields[4].count = 1;
    m_pc.fields[5].name = "r";
    m_pc.fields[5].offset = 14;
    m_pc.fields[5].data_type = PointCloudTypes::UINT8;
    m_pc.fields[5].count = 1;
    m_pc.is_bigendian = false;
    m_pc.point_step = 16;
    m_pc.is_dense = false;

    m_pc.width = static_cast<uint32_t>(m_depthWidth);
    m_pc.height = static_cast<uint32_t>(m_depthHeight);
    m_pc.row_step = m_pc.point_step * m_pc.width;
    m_pc.data.length(m_pc.height* m_pc.row_step);

  } catch (const std::exception& e) {
    RTC_ERROR((e.what()));
    return RTC::RTC_ERROR;
  } catch (...) {
    RTC_ERROR(("An exception occurred in onActivated()"));
    return RTC::RTC_ERROR;
  }
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Kinect2ToPC::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onDeactivated()"));

  if (m_sensor) {
    m_sensor->Close();
  }
  SafeRelease(m_sensor);
  SafeRelease(m_mapper);
  SafeRelease(m_colorReader);
  SafeRelease(m_depthReader);

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Kinect2ToPC::onExecute(RTC::UniqueId ec_id)
{
  try {
    HRESULT result;
    // Acquire Latest Color Frame
    IColorFrame* colorFrame = nullptr;
    result = m_colorReader->AcquireLatestFrame(&colorFrame);
    if (SUCCEEDED(result)) {
      // Retrieved Color Data
      result = colorFrame->CopyConvertedFrameDataToArray(
        m_colorBuffer.size() * sizeof(RGBQUAD), 
        reinterpret_cast<BYTE*>(&m_colorBuffer[0]), 
        ColorImageFormat::ColorImageFormat_Bgra);
      if (FAILED(result)) {
        throw std::exception("Exception : IColorFrame::CopyConvertedFrameDataToArray()");
      }
    } else {
      return RTC::RTC_OK;
    }
    SafeRelease(colorFrame);

    // Acquire Latest Depth Frame
    IDepthFrame* depthFrame = nullptr;
    result = m_depthReader->AcquireLatestFrame(&depthFrame);
    if (SUCCEEDED(result)) {
      // Retrieved Depth Data
      result = depthFrame->CopyFrameDataToArray(m_depthBuffer.size(), &m_depthBuffer[0]);
      if (FAILED(result)) {
        throw std::exception("Exception : IDepthFrame::CopyFrameDataToArray()");
      }
    } else {
      return RTC::RTC_OK;
    }
    SafeRelease(depthFrame);


    float* dst_cloud = (float*)m_pc.data.get_buffer();

    for (int y = 0; y < m_depthHeight; y++) {
      for (int x = 0; x < m_depthWidth; x++) {

        DepthSpacePoint depthSpacePoint = { static_cast<float>(x), static_cast<float>(y) };
        UINT16 depth = m_depthBuffer[y * m_depthWidth + x];

        // Coordinate Mapping Depth to Color Space, and Setting PointCloud RGB
        ColorSpacePoint colorSpacePoint = { 0.0f, 0.0f };
        m_mapper->MapDepthPointToColorSpace(depthSpacePoint, depth, &colorSpacePoint);
        int colorX = static_cast<int>(std::floor(colorSpacePoint.X + 0.5f));
        int colorY = static_cast<int>(std::floor(colorSpacePoint.Y + 0.5f));
        CameraSpacePoint cameraSpacePoint = { FLT_NAN, FLT_NAN, FLT_NAN };
        float rgb = FLT_NAN;
        if ((0 <= colorX) && (colorX < m_colorWidth) && (0 <= colorY) && (colorY < m_colorHeight)) {
          RGBQUAD color = m_colorBuffer[colorY * m_colorWidth + colorX];
          uint32_t ui = (color.rgbRed << 16) | (color.rgbGreen << 8) | color.rgbBlue;
          rgb = *reinterpret_cast<float*>(&ui);
          m_mapper->MapDepthPointToCameraSpace(depthSpacePoint, depth, &cameraSpacePoint);
        }

        dst_cloud[0] = -cameraSpacePoint.X; //xŽ²‚ÌŒü‚«‚ð”½“]
        dst_cloud[1] = cameraSpacePoint.Y;
        dst_cloud[2] = -cameraSpacePoint.Z; //zŽ²‚ÌŒü‚«‚ð”½“]
        dst_cloud[3] = rgb;
        dst_cloud += 4;
      }
    }

    m_pcOut.write();

    m_fpsCounter++;
    m_steadyEnd = chrono::steady_clock::now();
    double timeSec = std::chrono::duration<double>(m_steadyEnd - m_steadyStart).count();
    if (timeSec >= 1) {
      RTC_INFO(("%f fps", m_fpsCounter / timeSec));
      m_steadyStart = m_steadyEnd;
      m_fpsCounter = 0;
    }
  } catch (const std::exception& e) {
    RTC_ERROR((e.what()));
    return RTC::RTC_ERROR;
  } catch (...) {
    RTC_ERROR(("An exception occurred in onExecute()"));
    return RTC::RTC_ERROR;
  }

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Kinect2ToPC::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Kinect2ToPC::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Kinect2ToPC::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Kinect2ToPC::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Kinect2ToPC::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

extern "C"
{
 
  void Kinect2ToPCInit(RTC::Manager* manager)
  {
    coil::Properties profile(kinect2topc_spec);
    manager->registerFactory(profile,
                             RTC::Create<Kinect2ToPC>,
                             RTC::Delete<Kinect2ToPC>);
  }
  
};


