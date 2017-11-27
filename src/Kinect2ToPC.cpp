// -*- C++ -*-
/*!
 * @file  Kinect2ToPC.cpp
 * @brief RTC:PCL Grabber for Kinect
 * @date $Date$
 *
 * $Id$
 */

#include "kinect2_grabber.h"
#include "Kinect2ToPC.h"

#include <iostream>
#include <string>
using namespace std;

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
  cout << "Kinect2ToPC::onInitialize()" << endl;
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
  cout << "Kinect2ToPC::onActivated()" << endl;
  try {
    m_interface = boost::make_shared<pcl::Kinect2Grabber>();
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
    boost::function<void (const pcl::PointCloud<PointT>::ConstPtr&)> f = boost::bind(&Kinect2ToPC::cloud_cb, this, _1);
    m_interface->registerCallback(f);
    m_interface->start();
  } catch (pcl::io::IOException& e) {
    cerr << "Failed to create a grabber: " << e.what () << endl;
    return RTC::RTC_ERROR;
  } catch (...) {
    cerr << "An exception occurred while starting grabber" << endl;
    return RTC::RTC_ERROR;
  }
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Kinect2ToPC::onDeactivated(RTC::UniqueId ec_id)
{
  cout << "Kinect2ToPC::onDeactivated()" << endl;
  m_interface->stop();
  m_interface = nullptr;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Kinect2ToPC::onExecute(RTC::UniqueId ec_id)
{
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

void Kinect2ToPC::cloud_cb(const pcl::PointCloud<PointT>::ConstPtr &cloud)
{
  setTimestamp(m_pc);
  m_pc.width = cloud->width;
  m_pc.height = cloud->height;
  m_pc.row_step = m_pc.point_step*m_pc.width;
  m_pc.data.length(m_pc.height*m_pc.row_step);

  float *dst_cloud = (float *)m_pc.data.get_buffer();
  for (unsigned int i=0; i<cloud->points.size(); i++){
    dst_cloud[0] = -cloud->points[i].x;
    dst_cloud[1] =  cloud->points[i].y;
    dst_cloud[2] = -cloud->points[i].z;
    dst_cloud[3] =  cloud->points[i].rgb;
    dst_cloud += 4;
  }
  m_pcOut.write();

  static int count = 0;
  static double last = 0;

  if (++count == 30)
  {
    double now = pcl::getTime();
    std::cout << "Average framerate: " << double(count)/double(now - last) << " Hz" <<  std::endl;
    count = 0;
    last = now;
  }
}


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


