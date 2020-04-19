// -*- C++ -*-
/*!
 * @file  Kinect2ToPC.h
 * @brief RTC:PCL Grabber for Kinect
 * @date  $Date$
 *
 * $Id$
 */

#ifndef KINECT2TOPC_H
#define KINECT2TOPC_H

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">
#include "BasicDataTypeStub.h"
#include "pointcloudStub.h"

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="port_stub_h">


// </rtc-template>

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>

#include <vector>
#include <chrono>
#include <Eigen/Geometry>
#include <Windows.h>
#include <Kinect.h>

using namespace RTC;

/*!
 * @class Kinect2ToPC
 * @brief RTC:PCL Grabber for Kinect
 *
 */
class Kinect2ToPC
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  Kinect2ToPC(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~Kinect2ToPC();

  // <rtc-template block="public_attribute">
  
  // </rtc-template>

  // <rtc-template block="public_operation">
  
  // </rtc-template>

  /***
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry() 
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onInitialize();

  /***
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onFinalize();

  /***
   *
   * The startup action when ExecutionContext startup
   * former rtc_starting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  /***
   *
   * The shutdown action when ExecutionContext stop
   * former rtc_stopping_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  /***
   *
   * The activated action (Active state entry action)
   * former rtc_active_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  /***
   *
   * The deactivated action (Active state exit action)
   * former rtc_active_exit()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   *
   * The execution action that is invoked periodically
   * former rtc_active_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

  /***
   *
   * The aborting action when main logic error occurred.
   * former rtc_aborting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  /***
   *
   * The error action in ERROR state
   * former rtc_error_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   *
   * The reset action that is invoked resetting
   * This is same but different the former rtc_init_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
  /***
   *
   * The state update action that is invoked after onExecute() action
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  /***
   *
   * The action that is invoked when execution context's rate is changed
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);


 protected:
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">
  /*!
   * 
   * - Name:  transX
   * - DefaultValue: 0.0
   */
  float m_transX;
  /*!
   * 
   * - Name:  transY
   * - DefaultValue: 0.0
   */
  float m_transY;
  /*!
   * 
   * - Name:  transZ
   * - DefaultValue: 0.0
   */
  float m_transZ;
  /*!
   * 
   * - Name:  rotX
   * - DefaultValue: 0.0
   */
  float m_rotX;
  /*!
   * 
   * - Name:  rotY
   * - DefaultValue: 0.0
   */
  float m_rotY;
  /*!
   * 
   * - Name:  rotZ
   * - DefaultValue: 0.0
   */
  float m_rotZ;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::TimedString m_command;
  /*!
   */
  RTC::InPort<RTC::TimedString> m_commandIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  PointCloudTypes::PointCloud m_pc;
  /*!
   */
  RTC::OutPort<PointCloudTypes::PointCloud> m_pcOut;
  
  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  
  // </rtc-template>

 private:
  // <rtc-template block="private_attribute">
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>
   IKinectSensor* m_sensor;
   ICoordinateMapper* m_mapper;
   int m_colorWidth;
   int m_colorHeight;
   std::vector<RGBQUAD> m_colorBuffer;
   int m_depthWidth;
   int m_depthHeight;
   std::vector<UINT16> m_depthBuffer;
   IColorFrameReader* m_colorReader;
   IDepthFrameReader* m_depthReader;

   int m_fpsCounter;
   std::chrono::steady_clock::time_point m_steadyStart;
   std::chrono::steady_clock::time_point m_steadyEnd;

   bool m_coordinateTransformation;
   Eigen::Affine3f m_transform;

   bool m_running;

   template<class Interface>
   inline void SafeRelease(Interface*& IRelease)
   {
     if (IRelease != NULL) {
       IRelease->Release();
       IRelease = NULL;
     }
   }
};


extern "C"
{
  DLL_EXPORT void Kinect2ToPCInit(RTC::Manager* manager);
};

#endif // KINECT2TOPC_H
