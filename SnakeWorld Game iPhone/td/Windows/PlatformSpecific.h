#pragma once

void TDGotoWebSite(std::string site);
std::string TDSubmitURL(std::string url);

void TDAlert(std::string title, std::string msg, std::string btn1, int btn1_val, std::string btn2, int btn2_val, std::string btn3, int btn3_val);
int TDAlertResult();

void TDBuzz();
