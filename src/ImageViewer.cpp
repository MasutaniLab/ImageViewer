// -*- C++ -*-
/*!
 * @file  ImageViewer.cpp
 * @brief Image Viewer Component with common camera interface 2.0
 * @date $Date$
 *
 * $Id$
 */

#include "ImageViewer.h"
using namespace std;
using namespace zbar;

// Module specification
// <rtc-template block="module_spec">
static const char* imageviewer_spec[] =
  {
    "implementation_id", "ImageViewer",
    "type_name",         "ImageViewer",
    "description",       "Image Viewer Component with common camera interface 2.0",
    "version",           "2.0.1",
    "vendor",            "MasutaniLab",
    "category",          "ImageProcessing",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.capture_frame_num", "0",
    "conf.default.zbar", "0",
    "conf.default.windowTitle", "ImageViewer",
    "conf.default.window1X", "0",
    "conf.default.window1Y", "0",
    "conf.default.window1Width", "320",
    "conf.default.window1Height", "180",
    "conf.default.window2X", "0",
    "conf.default.window2Y", "0",
    "conf.default.window2Width", "1280",
    "conf.default.window2Height", "720",

    // Widget
    "conf.__widget__.capture_frame_num", "text",
    "conf.__widget__.zbar", "radio",
    "conf.__widget__.windowTitle", "text",
    "conf.__widget__.window1X", "text",
    "conf.__widget__.window1Y", "text",
    "conf.__widget__.window1Width", "text",
    "conf.__widget__.window1Height", "text",
    "conf.__widget__.window2X", "text",
    "conf.__widget__.window2Y", "text",
    "conf.__widget__.window2Width", "text",
    "conf.__widget__.window2Height", "text",
    // Constraints
    "conf.__constraints__.zbar", "(0,1)",

    "conf.__type__.capture_frame_num", "int",
    "conf.__type__.zbar", "short",
    "conf.__type__.windowTitle", "string",
    "conf.__type__.window1X", "short",
    "conf.__type__.window1Y", "short",
    "conf.__type__.window1Width", "short",
    "conf.__type__.window1Height", "short",
    "conf.__type__.window2X", "short",
    "conf.__type__.window2Y", "short",
    "conf.__type__.window2Width", "short",
    "conf.__type__.window2Height", "short",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ImageViewer::ImageViewer(RTC::Manager* manager)
// <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_ImageIn("Image", m_Image),
    m_switchOut("switch", m_switch),
    m_CameraCaptureServicePort("CameraCaptureService")

  // </rtc-template>
{
}

/*!
 * @brief destructor
 */
ImageViewer::~ImageViewer()
{
}



RTC::ReturnCode_t ImageViewer::onInitialize()
{
  RTC_INFO(("onInitialize()"));
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("Image", m_ImageIn);
  
  // Set OutPort buffer
  addOutPort("switch", m_switchOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  m_CameraCaptureServicePort.registerConsumer("CameraCaptureService", "Img::CameraCaptureService", m_CameraCaptureService);
  
  // Set CORBA Service Ports
  addPort(m_CameraCaptureServicePort);
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("capture_frame_num", m_capture_frame_num, "0");
  bindParameter("zbar", m_zbar, "0");
  bindParameter("windowTitle", m_windowTitle, "ImageViewer");
  bindParameter("window1X", m_window1X, "0");
  bindParameter("window1Y", m_window1Y, "0");
  bindParameter("window1Width", m_window1Width, "320");
  bindParameter("window1Height", m_window1Height, "180");
  bindParameter("window2X", m_window2X, "0");
  bindParameter("window2Y", m_window2Y, "0");
  bindParameter("window2Width", m_window2Width, "1280");
  bindParameter("window2Height", m_window2Height, "720");
  // </rtc-template>

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageViewer::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t ImageViewer::onActivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onActivated()"));
  PortServiceList* portlist;

  portlist = this->get_ports();
  RTC_INFO(("portlist->length(): %d", portlist->length()))
  connection_check = new bool[portlist->length()];
  for (unsigned int i = 0; i < portlist->length(); ++i)
  {
    PortService_ptr port;
    port = (*portlist)[i];
    if (port->get_port_profile()->connector_profiles.length() != 0)
      connection_check[i] = true;
    else
      connection_check[i] = false;
  }

  //connection_check[2]がサービスポートになる保証はあるのか？
  //サービスポート接続状態のチェック
  if (connection_check[2])
  {
    //連続画像取得モードに設定
    if (m_capture_frame_num == 0)
    {
      RTC_INFO(("Send command of \"start continuous\" via CameraCaptureService."));
      m_CameraCaptureService->start_continuous();
    }
    //1shotキャプチャモードに設定
    else if (m_capture_frame_num == 1)
    {
      RTC_INFO(("Send command of \"take one frame\" via CameraCaptureService."));
      m_CameraCaptureService->take_one_frame();
    }
    //指定枚数キャプチャモードに設定
    else if (m_capture_frame_num > 1)
    {
      RTC_INFO(("Send command of \"take multi frames\" via CameraCaptureService."));
      m_CameraCaptureService->take_multi_frames(m_capture_frame_num);
    } else
    {
      RTC_ERROR(("Configuration Param <frames_num> should be over 0. [%d]", m_capture_frame_num));
      return RTC::RTC_ERROR;
    }
  }

  cv::namedWindow(m_windowTitle.c_str(), CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO);
  cv::moveWindow(m_windowTitle.c_str(), m_window1X, m_window1Y);
  cv::resizeWindow(m_windowTitle.c_str(), m_window1Width, m_window1Height);
  m_windowConf = 1;
  m_cameraOn = true;
  m_os.str("");
  RTC_INFO(("Start image view"));
  RTC_INFO(("If you want to take a 1 shot image as image file, please push s on Captured Image Window!"));

  if (m_zbar) {
    m_scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageViewer::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onDeactivated()"));
  //connection_check[2]がサービスポートになる保証はあるのか？
  if (connection_check[2]) {
    //連続キャプチャモードの場合は、キャプチャを停止
    if (m_capture_frame_num == 0)
    {
      RTC_INFO(("Send command of \"stop continuous\" via CameraCaptureService."));
      m_CameraCaptureService->stop_continuous();
    }
  }

  if (connection_check != NULL)
    delete[] connection_check;

  //描画ウィンドウの消去
  cv::destroyWindow(m_windowTitle.c_str());
  RTC_INFO(("Stop image view"));

  //描画用画像メモリの解放
  image.release();

  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageViewer::onExecute(RTC::UniqueId ec_id)
{
  int key = cv::waitKey(3);
  if (key == 'm') {
    m_windowConf = 3 - m_windowConf;
    RTC_INFO(("m_windowConf: %d", m_windowConf));
    if (m_windowConf == 1) {
      cv::moveWindow(m_windowTitle.c_str(), m_window1X, m_window1Y);
      cv::resizeWindow(m_windowTitle.c_str(), m_window1Width, m_window1Height);
    } else {
      cv::moveWindow(m_windowTitle.c_str(), m_window2X, m_window2Y);
      cv::resizeWindow(m_windowTitle.c_str(), m_window2Width, m_window2Height);
    } 
  } else if (key == 'o') {
    m_cameraOn = !m_cameraOn;
    RTC_INFO(("m_cameraOn: %d", m_cameraOn));
    m_switch.data = m_cameraOn;
    m_switchOut.write();
  }
  //Inport data check
  if (m_ImageIn.isNew())
  {
    m_ImageIn.read();

    width = m_Image.data.image.width;
    height = m_Image.data.image.height;
    channels = (m_Image.data.image.format == Img::CF_GRAY) ? 1 :
      (m_Image.data.image.format == Img::CF_RGB || m_Image.data.image.format == Img::CF_PNG || m_Image.data.image.format == Img::CF_JPEG) ? 3 :
      (m_Image.data.image.raw_data.length() / width / height);
    RTC_TRACE(("Capture image size %d x %d", width, height));
    RTC_TRACE(("Channels %d", channels));

    if (channels == 3)
      image.create(height, width, CV_8UC3);
    else
      image.create(height, width, CV_8UC1);

    long data_length = m_Image.data.image.raw_data.length();
    //long image_size = width * height * channels;

    if (m_Image.data.image.format == Img::CF_RGB || m_Image.data.image.format == Img::CF_GRAY)
    {
      for (int i = 0; i < height; ++i)
        memcpy(&image.data[i*image.step], &m_Image.data.image.raw_data[i*width*channels], sizeof(unsigned char)*width*channels);
      if (channels == 3)
        cv::cvtColor(image, image, CV_RGB2BGR);
    } else if (m_Image.data.image.format == Img::CF_JPEG || m_Image.data.image.format == Img::CF_PNG)
    {
      std::vector<uchar> compressed_image = std::vector<uchar>(data_length);
      memcpy(&compressed_image[0], &m_Image.data.image.raw_data[0], sizeof(unsigned char) * data_length);

      //Decode received compressed image
      cv::Mat decoded_image;
      if (channels == 3)
      {
        decoded_image = cv::imdecode(cv::Mat(compressed_image), CV_LOAD_IMAGE_COLOR);
      } else
      {
        decoded_image = cv::imdecode(cv::Mat(compressed_image), CV_LOAD_IMAGE_GRAYSCALE);
      }
      image = decoded_image;
    }
    if (key == 's') {
      const int N = 32;
      char filename[N];
      time_t timer;
      struct tm *timeptr;
      timer = time(NULL);
      timeptr = localtime(&timer);
      strftime(filename, N, "%Y%m%d%H%M%S.png", timeptr);
      RTC_INFO(("Saving image to \"%s\" ...", filename));
      cv::imwrite(filename, image);
      RTC_INFO(("done."));
    }
    if (m_zbar) {
      if (key == 'r') {
        cv::Mat grey;
        if (channels == 3) {
          cv::cvtColor(image, grey, CV_BGR2GRAY);
        } else {
          grey = image;
        }
        int width = grey.size().width;
        int height = grey.size().height;
        uchar *raw = grey.data;
        Image zbarImage(width, height, "Y800", raw, width * height);
        m_os.str("");
        int n = m_scanner.scan(zbarImage);
        if (n == 0)
        {
          m_os << "Cannot recognize.";
        } else
        {
          for (Image::SymbolIterator symbol = zbarImage.symbol_begin();
            symbol != zbarImage.symbol_end(); ++symbol)
          {
            m_os << symbol->get_type_name() << " \"" << symbol->get_data() << "\" ";
          }
        }
        RTC_INFO((m_os.str().c_str()));
      }
    }

  } //if (m_ImageIn.isNew())のブロックここまで

  //画像データが入っている場合は画像を表示
  if (!image.empty())
  {
    /*
    //Communication Time
    coil::TimeValue tm(coil::gettimeofday());
    std::cout<< "Communication Time: " << tm.usec() - (m_Image.tm.nsec / 1000) << "\r";
    */
    cv::putText(image, m_os.str(), cv::Point(0, image.size().height - 20), cv::FONT_HERSHEY_SIMPLEX, 0.5,
      cv::Scalar(255, 255, 255));
    if (!m_cameraOn) {
      cv::putText(image, "OFF", 
        cv::Point(image.size().width*0.35, image.size().height*0.5), 
        cv::FONT_HERSHEY_SIMPLEX, 8,
        cv::Scalar(0, 0, 255), 8);
    }
    cv::imshow(m_windowTitle.c_str(), image);
  }

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageViewer::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{

  void ImageViewerInit(RTC::Manager* manager)
  {
    coil::Properties profile(imageviewer_spec);
    manager->registerFactory(profile,
      RTC::Create<ImageViewer>,
      RTC::Delete<ImageViewer>);
  }

};


