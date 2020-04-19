@echo off
: Kinect2ToPCのテスト

:ネーミングサービスの確認
rtls /localhost > nul
if errorlevel 1 (
  echo ネーミングサーバが見つかりません
  pause
  exit /b 1
  rem /bオプションは親を終わらせないために必須
)

set pythonScript=SetCommand

:コンポーネントの起動
call .\Kinect2ToPC.bat
call ..\PointCloudViewer\PointCloudViewer.bat
start "%pythonScript%" .\%pythonScript%.pyw

:コンポーネント名を変数化
set a=/localhost/Kinect2ToPC0.rtc
set v=/localhost/PointCloudViewer0.rtc
set i=/localhost/%pythonScript%0.rtc

:コンポーネント起動待ち
:rtls-a
echo %a%の起動待ち
timeout 1 /nobreak > nul
rtls %a% > nul 2>&1
if errorlevel 1 goto rtls-a

:rtls-i
echo %i%の起動待ち
timeout 1 /nobreak > nul
rtls %i% > nul 2>&1
if errorlevel 1 goto rtls-i

:接続
rtcon %a%:pc %v%:pc
rtcon %i%:command %a%:command

:アクティベート
rtact %a% %v% %i%

:loop
set /p ans="終了しますか？ (y/n)"
if not "%ans%"=="y" goto loop

:ディアクティベート
rtdeact %a% %v% %i%

:終了（rtexitは，引数を一つずつ）
rtexit %a%
rtexit %v%
rtexit %i%

taskkill /fi "WINDOWTITLE eq %pythonScript%"
