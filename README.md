# Point Cloud Grabber for Microsoft Kinect v2

大阪電気通信大学  
升谷 保博  
2019年11月19日（OpenRTM-aist 1.2.0に対応）

## はじめに

- Microsoft Kinect v2から深度と色の情報を読み取り，RTC:PCLのPointCloud型（`PointCloudTypes::PointCoud`）を
出力するRTコンポーネントです．
- 以下の環境で開発，動作確認しています．
  - Windows 10 64bit版
  - Visual Studio 2015 x64
  - OpenRTM-aist 1.2.0 64bit版
  - Point Cloud Library 1.8.1 AllinOne (VS2015 64bit用)
  - Microsoft Kinect Xbox One (Kinect v2)
  - Kinect for Windows SDK v2.0
- 杉浦 司 (UnaNancyOwen) 氏の
[Grabber of Point Cloud Library based on Kinect for Windows SDK ](https://github.com/UnaNancyOwen/KinectGrabber/tree/Kinect2Grabber)のコードを
そのまま利用しています（`KinectGrabber\kinect2_grabber.h`）．
- CMakeでKinect SDKを見つけるモジュールも杉浦 司氏の
[`FindKinectSDK2.cmake`](https://github.com/UnaNancyOwen/KinectGrabber/blob/Kinect2Grabber/Sample/FindKinectSDK2.cmake)
をそのまま使っています．
- `pointcloud.idl` は，Geoffrey Biggs (gbiggs)氏の
[RT-Components for the Point Cloud Library](https://github.com/gbiggs/rtcpcl/)
に[含まれているもの](https://github.com/gbiggs/rtcpcl/blob/master/pc_type/pointcloud.idl)
をそのまま使っています．
- 出力する点群を表す座標系は，x軸は右向きが正，z軸は後ろ向きが正です（それぞれKinectの座標系と逆向き，y軸はどちらも上向きが正）．
Choeonoidの深度センサのモデルに合わせるためにこのようにしています．

## 仕様

- height: 424
- width: 512
- type: "xyzrgb"
- is_bigendian: 0
- point_step: 16
- row_step: 8192
- is_dense: 0
- fields (name,offset,data_type,count)
  - 0: "x",0,6,4
  - 1: "y",4,6,4
  - 2: "z",8,6,4
  - 3: "b",12,1,1
  - 4: "g",13,1,1
  - 5: "r",14,1,1

## インストール

- [OpenRTM-aist 1.2.0](https://www.openrtm.org/openrtm/ja/node/6570)をインストール．
- [GitHubのpclのRelease](https://github.com/PointCloudLibrary/pcl/releases)の中のWindows用AllInOne`PCL-X.X.X-AllInOne-msvcYYYY-winZZ.exe`をインストール．
- [Kinect for Windows SDK v2.0](https://www.microsoft.com/en-us/download/details.aspx?id=44561)
をインストール．
- [KinectToPC](https://github.com/MasutaniLab/Kinect2ToPC)
をクローンかダウンロードする．
- CMake
  - ビルドディレクトリはトップ直下の`build`
  - - ConfigureはVisual Studioのバージョンとプラットフォームに合わせる．
- `build\Kinect2ToPC.sln`をVisual Studioで開く．
- パフォーマンスを出すために，Releaseでビルドがお勧め．

## 使い方

- KinectをUSBポートに接続する．
- 出力されるデータ量が多いので，CORBAのデフォルトの設定ではエラーになります．
rtc.confに`corba.args: -ORBgiopMaxMsgSize`の設定が必要です．
トップディレクトリのrtc.confでは`corba.args: -ORBgiopMaxMsgSize 20971520`
にしています（デフォルト値の10倍）．
- コンポーネントを起動するバッチファイル`Kinect2ToPC.bat`を用意しています．
  - ビルドディレクトリがトップ直下の`build`であることを仮定しています．
  - 環境変数`RTM_VC_CONFIG`を`Debug`か`Release`に設定してください．
- 動作確認のための接続相手として，
[PointCloudViewer](https://github.com/MasutaniLab/PointCloudViewer)
を使ってください．

## 既知の問題・TODO

- [`pointcloud.idl`](https://github.com/gbiggs/rtcpcl/blob/master/pc_type/pointcloud.idl)に記述されている仕様と，[RT-Components for the Point Cloud Library](https://github.com/gbiggs/rtcpcl/)の関数の仕様が異なっていますが，`pointcloud.idl`本来の仕様のデータを出力しています．
- 1回に送出するデータ量が約3.3MB．もっと圧縮して送る方がいいのかもしれません．
- Windowsでしか動作しません．

## 履歴
- 2018年1月4日 VS2015とPCL 1.8.1へ移行
- 2017年11月27日 最初のリリース