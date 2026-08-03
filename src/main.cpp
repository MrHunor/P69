#include <windows.h>
#include <iostream>
#include <filesystem>
#include "utils/utils.h"
#include "vid/vid.h"

int main(int argc, char *argv[])
{
std::cout<<GetCurrentPlayingInfo()<<std::endl;
std::filesystem::path filePath = findFileByID(std::filesystem::current_path().string(),extractID( DownloadVideo(GetCurrentPlayingInfo())));
//restart song has to come before opening the video file otherwise it is likely the video file gets treated as the media (offset i know but the beat matching will fix this )
restartSong();
//cannot be system because of weird unicode handling
ShellExecuteW(NULL, L"open", filePath.wstring().c_str(), NULL, NULL, SW_SHOWNORMAL);
system("pause");
}