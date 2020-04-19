// -*- C++ -*-
/*!
 * @file  Kinect2ToPC.cpp
 * @brief RTC:PCL Grabber for Kinect
 * @date $Date$
 *
 * $Id$
 */

#define _USE_MATH_DEFINES
#include "Kinect2ToPC.h"

#include <limits>
#include <iostream>
#include <string>
using namespace std;
using namespace Eigen;

#define print(x) cout << #x ": " << x << endl;

const float FLT_NAN = std::numeric_limits<float>::quiet_NaN();

// Module specification
// <rtc-template block="module_spec">
static const char* kinect2topc_spec[] =
  {
    "implementation_id", "Kinect2ToPC",
    "type_name",         "Kinect2ToPC",
    "description",       "RTC:PCL Grabber for Kinect v2",
    "version",           "1.0.0",
    "vendor",            "MasutaniLab",
    "category",          "PointCloud",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.transX", "0.0",
    "conf.default.transY", "0.0",
    "conf.default.transZ", "0.0",
    "conf.default.rotX", "0.0",
    "conf.default.rotY", "0.0",
    "conf.default.rotZ", "0.0",

    // Widget
    "conf.__widget__.transX", "text",
    "conf.__widget__.transY", "text",
    "conf.__widget__.transZ", "text",
    "conf.__widget__.rotX", "text",
    "conf.__widget__.rotY", "text",
    "conf.__widget__.rotZ", "text",
    // Constraints

    "conf.__type__.transX", "float",
    "conf.__type__.transY", "float",
    "conf.__type__.transZ", "float",
    "conf.__type__.rotX", "float",
    "conf.__type__.rotY", "float",
    "conf.__type__.rotZ", "float",

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
    m_commandIn("command", m_command),
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
  addInPort("command", m_commandIn);
  
  // Set OutPort buffer
  addOutPort("pc", m_pcOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("transX", m_transX, "0.0");
  bindParameter("transY", m_transY, "0.0");
  bindParameter("transZ", m_transZ, "0.0");
  bindParameter("rotX", m_rotX, "0.0");
  bindParameter("rotY", m_rotY, "0.0");
  bindParameter("rotZ", m_rotZ, "0.0");
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
    if (m_rotX == 0 && m_rotY == 0 && m_rotZ == 0 && m_transX == 0 && m_transY == 0 && m_transZ == 0) {
      m_coordinateTransformation = false;
    } else {
      m_coordinateTransformation = true;
      const float M_PIF = float(M_PI);
      float radX = m_rotX * M_PIF / 180;
      float radY = m_rotY * M_PIF / 180;
      float radZ = m_rotZ * M_PIF / 180;
      m_transform
        = Translation3f(m_transX, m_transY, m_transZ)
        * AngleAxisf(radZ, Vector3f::UnitZ())
        * AngleAxisf(radY, Vector3f::UnitY())
        * AngleAxisf(radX, Vector3f::UnitX());
      cout << "m_transform:" << endl << m_transform.matrix() << endl;
    }

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

    m_steadyStart = chrono::steady_clock::now();
    m_fpsCounter = 0;
    m_running = true;
    m_first = true;
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
  if (m_commandIn.isNew()) {
    m_commandIn.read();
    string s = m_command.data;
    if (s == "start") {
      m_running = true;
      RTC_INFO(("m_running = true"));
    } else if (s == "stop") {
      m_running = false;
      RTC_INFO(("m_running = false"));
    } else {
      RTC_ERROR(("未知のコマンド: %s", s.c_str()));
      return RTC::RTC_ERROR;
    }
  }

  if (m_running) {
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

      //データを取得した後でないとMapDepthPointToColorSpace()が正常に動作しないため，ここで処理する．
      if (m_first) {
        RTC_INFO(("深度カラー対応表を作成開始"));
        for (int i = 0; i < m_NumTable; i++) {
          UINT16 depth = i * depthPitch;
          for (int y = 0; y < m_depthHeight; y++) {
            for (int x = 0; x < m_depthWidth; x++) {
              DepthSpacePoint depthSpacePoint = { static_cast<float>(x), static_cast<float>(y) };
              ColorSpacePoint colorSpacePoint = { 0.0f, 0.0f };
              HRESULT result = m_mapper->MapDepthPointToColorSpace(depthSpacePoint, depth, &colorSpacePoint);
              //print(colorSpacePoint.X);
              //print(colorSpacePoint.Y);
              int colorX = static_cast<int>(std::floor(colorSpacePoint.X + 0.5f));
              int colorY = static_cast<int>(std::floor(colorSpacePoint.Y + 0.5f));
              CameraSpacePoint cameraSpacePoint = { FLT_NAN, FLT_NAN, FLT_NAN };
              int cindex = -1;
              if ((0 <= colorX) && (colorX < m_colorWidth) && (0 <= colorY) && (colorY < m_colorHeight)) {
                cindex = colorY * m_colorWidth + colorX;
              }
              m_depthColorTable[i][y][x] = cindex;
            }
          }
        }
        RTC_INFO(("深度カラー対応表を作成終了"));
        m_first = false;
      }

      float* dst_cloud = (float*)m_pc.data.get_buffer();

      for (int y = 0; y < m_depthHeight; y++) {
        for (int x = 0; x < m_depthWidth; x++) {
          DepthSpacePoint depthSpacePoint = { static_cast<float>(x), static_cast<float>(y) };
          UINT16 depth = m_depthBuffer[y * m_depthWidth + x];
          int index = static_cast<int>(std::floor(depth/float(depthPitch) + 0.5f));
          if (index < 0) { index = 0; }
          if (index >= m_NumTable) { index = m_NumTable - 1; }
          int cindex = m_depthColorTable[index][y][x];
          CameraSpacePoint cameraSpacePoint = { FLT_NAN, FLT_NAN, FLT_NAN };
          float rgb = FLT_NAN;
#if 1
          if (cindex >= 0 && cindex < m_colorBuffer.size()) {
            RGBQUAD color = m_colorBuffer[cindex];
            uint32_t ui = (color.rgbRed << 16) | (color.rgbGreen << 8) | color.rgbBlue;
            rgb = *reinterpret_cast<float*>(&ui);
            m_mapper->MapDepthPointToCameraSpace(depthSpacePoint, depth, &cameraSpacePoint);
          }
#else
          RGBQUAD color;
          if (cindex >= 0 && cindex < m_colorBuffer.size()) {
            color = m_colorBuffer[cindex];
          } else {
            color.rgbRed = 255;
            color.rgbGreen = 0;
            color.rgbBlue = 0;
          }
          uint32_t ui = (color.rgbRed << 16) | (color.rgbGreen << 8) | color.rgbBlue;
          rgb = *reinterpret_cast<float*>(&ui);
          m_mapper->MapDepthPointToCameraSpace(depthSpacePoint, depth, &cameraSpacePoint);
#endif
          //座標変換の前にy軸とz軸を反転させる
          Vector3f tmp(-cameraSpacePoint.X, cameraSpacePoint.Y, -cameraSpacePoint.Z);
          if (m_coordinateTransformation) {
            tmp = m_transform * tmp;
          }
          dst_cloud[0] = tmp(0); 
          dst_cloud[1] = tmp(1);
          dst_cloud[2] = tmp(2);
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


