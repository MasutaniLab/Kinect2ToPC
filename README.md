# Point Cloud Grabber for Microsoft Kinect v2

大阪電気通信大学  
升谷 保博  
2020年4月20日（深度カラー対応表の試み）

## はじめに

- Microsoft Kinect Xbox One (Kinect v2)から深度と色の情報を読み取り，RTC:PCLのPointCloud型（`PointCloudTypes::PointCoud`）を
出力するRTコンポーネントです．
- 以下の環境で開発，動作確認しています．
  - Windows 10 64bit版
  - Visual Studio 2019 x64
  - OpenRTM-aist 1.2.1 64bit版
  - Eigen 3.3.7
  - Kinect for Windows SDK v2.0
- CMakeでKinect SDKを見つけるモジュールは杉浦 司氏の
[`FindKinectSDK2.cmake`](https://github.com/UnaNancyOwen/KinectGrabber/blob/Kinect2Grabber/Sample/FindKinectSDK2.cmake)
をそのまま使っています．
- `pointcloud.idl` は，Geoffrey Biggs (gbiggs)氏の
[RT-Components for the Point Cloud Library](https://github.com/gbiggs/rtcpcl/)
に[含まれているもの](https://github.com/gbiggs/rtcpcl/blob/master/pc_type/pointcloud.idl)
をそのまま使っています．
- 出力する点群を表す座標系は，x軸は右向きが正，z軸は後ろ向きが正です（それぞれKinectの座標系と逆向き，y軸はどちらも上向きが正）．
Choeonoidの深度センサのモデルに合わせるためにこのようにしています．


## 準備

- [OpenRTM-aist](http://www.openrtm.org/openrtm/)をインストール．
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)をインストール．PCL AllInOneパッケージに付属のものでも構わない．
- [Kinect for Windows SDK v2.0](https://www.microsoft.com/en-us/download/details.aspx?id=44561)
をインストール．

## ビルド

- [KinectToPC](https://github.com/MasutaniLab/Kinect2ToPC)
をクローンかダウンロードする．
- CMake
  - ビルドディレクトリはトップ直下の`build`
  - ConfigureはVisual Studioのバージョンとプラットフォームに合わせる．
  - 必要に応じて変数EIGEN_DIRの値を設定する．
- `build\Kinect2ToPC.sln`をVisual Studioで開く．
- パフォーマンスを出すために，Releaseでビルドがお勧め．

## 使い方

- Kinect v2をUSB3のポートに接続する．
- 出力されるデータ量が多いので，CORBAのデフォルトの設定ではエラーになります．
rtc.confに`corba.args: -ORBgiopMaxMsgSize`の設定が必要です．
トップディレクトリのrtc.confでは`corba.args: -ORBgiopMaxMsgSize 20971520`
にしています（デフォルト値の10倍）．
- コンポーネントを起動するバッチファイル`Kinect2ToPC.bat`を用意しています．
  - ビルドディレクトリがトップ直下の`build`であることを仮定しています．
  - 環境変数`RTM_VC_CONFIG`を`Debug`か`Release`に設定してください．
- 動作確認のための接続相手として，
[PointCloudViewer](https://github.com/MasutaniLab/PointCloudViewer)
を使ってください．そのためのバッチファイル[`TestKinect2ToPC.bat`](TestKinect2ToPC.bat)を用意しています．

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

### 入力ポート
- command
  - 説明： 外部からの指令を受け付ける
    - stop 処理を停止
    - start 処理を開始
  - 型： RTC::TimedString

### 出力ポート
- pc
  - 説明： 点群データ
  - 型： PointCloudTypes::PointCloud

### コンフィギュレーション
- transX 
  - 説明： 座標変換の並進x成分 [m]
  - 型： float
  - デフォルト値： 0.0
- transY
  - 説明： 座標変換の並進y成分 [m]
  - 型： float
  - デフォルト値： 0.0
- transZ
  - 説明： 座標変換の並進z成分 [m]
  - 型： float
  - デフォルト値： 0.0
- rotX
  - 説明： 座標変換の回転x成分 [deg]
  - 型： float
  - デフォルト値： 0.0
- rotY
  - 説明： 座標変換の回転y成分 [deg]
  - 型： float
  - デフォルト値： 0.0
- rotZ
  - 説明： 座標変換の回転z成分 [deg]
  - 型： float
  - デフォルト値： 0.0

## 既知の問題・TODO

- [`pointcloud.idl`](https://github.com/gbiggs/rtcpcl/blob/master/pc_type/pointcloud.idl)に記述されている仕様と，[RT-Components for the Point Cloud Library](https://github.com/gbiggs/rtcpcl/)の関数の仕様が異なっていますが，`pointcloud.idl`本来の仕様のデータを出力しています．
- 1回に送出するデータ量が約3.3MB．もっと圧縮して送る方がいいのかもしれません．
- Windowsでしか動作しません．
- 深度カラー対応表を利用することで，Panasonic CF-SZ5（Intel Core i5-6300U）において単体実行の場合の場合，他のプロセスが少なければ30[fps]を達成しています．
- 深度カラー対応表において，深度を100[mm]刻みで表現しているので，これによる精度劣化の検証が必要です．

## 履歴
- 2020年4月20日 深度カラー対応表の試み
- 2020年4月19日 入力ポート・コンフィギュレーションの追加
- 2020年4月18日 PCLを使わないようにした．開発環境をVS2019へ移行．
- 2018年1月4日 VS2015とPCL 1.8.1へ移行
- 2017年11月27日 最初のリリース