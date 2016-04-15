mex -I"E:/dev/Boost/boost_1_55_0" -L"E:\dev\Boost\boost_1_55_0\lib32-msvc-11.0" -L"..\deps" -L"../Build/Release/" -licap -lgeometry -lhpg -lhpgcreate -lmodel -ltime -lutil -lxslib -lJunctionLib ICAP_mex.cpp 
%mex -L"../Build/Debug/" -licap ICAP_mex.cpp 
%!copy "..\Build\Debug\ICAP.dll" .
