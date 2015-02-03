/*
 *  LoadTexture.h
 *  Snakeworld
 *
 */
#pragma once

uint GetTickCount();

const char* PathForFile( const char* fname);
const char* BundlePathName(const char* fname, const char* ext);

bool LoadTexture(unsigned char** buffer, unsigned int* size, unsigned int* width, unsigned int* height, const char* filename);

std::string Format(const char* szFormat, ...);
std::string TDSubmitURL(std::string url);

void TDGotoWebSite(std::string site);
std::string TDiPhoneUniqueID();
void TDAlert(std::string title, std::string msg, std::string btn1, int btn1_val, std::string btn2, int btn2_val, std::string btn3, int btn3_val);
int TDAlertResult();
