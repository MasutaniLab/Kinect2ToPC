@echo off
: Kinect2ToPC�̃e�X�g

:�l�[�~���O�T�[�r�X�̊m�F
rtls /localhost > nul
if errorlevel 1 (
  echo �l�[�~���O�T�[�o��������܂���
  pause
  exit /b 1
  rem /b�I�v�V�����͐e���I��点�Ȃ����߂ɕK�{
)

set pythonScript=SetCommand

:�R���|�[�l���g�̋N��
call .\Kinect2ToPC.bat
call ..\PointCloudViewer\PointCloudViewer.bat
start "%pythonScript%" .\%pythonScript%.pyw

:�R���|�[�l���g����ϐ���
set a=/localhost/Kinect2ToPC0.rtc
set v=/localhost/PointCloudViewer0.rtc
set i=/localhost/%pythonScript%0.rtc

:�R���|�[�l���g�N���҂�
:rtls-a
echo %a%�̋N���҂�
timeout 1 /nobreak > nul
rtls %a% > nul 2>&1
if errorlevel 1 goto rtls-a

:rtls-i
echo %i%�̋N���҂�
timeout 1 /nobreak > nul
rtls %i% > nul 2>&1
if errorlevel 1 goto rtls-i

:�ڑ�
rtcon %a%:pc %v%:pc
rtcon %i%:command %a%:command

:�A�N�e�B�x�[�g
rtact %a% %v% %i%

:loop
set /p ans="�I�����܂����H (y/n)"
if not "%ans%"=="y" goto loop

:�f�B�A�N�e�B�x�[�g
rtdeact %a% %v% %i%

:�I���irtexit�́C����������j
rtexit %a%
rtexit %v%
rtexit %i%

taskkill /fi "WINDOWTITLE eq %pythonScript%"
