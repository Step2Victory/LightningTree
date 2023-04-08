#pragma once
#include <string>
#include <filesystem>

inline std::filesystem::path current_path = "D:\\Documents\\GitHub\\LightningTree";
inline std::filesystem::path path_data = current_path / "LightningTree_data";

double conf_h = 100;
double conf_delta_t = 0.001;
double conf_r = 0.01;
double conf_R = 50;
double conf_resistance = 5;
double conf_E_plus = 50;
double conf_E_minus = 100;
double conf_E_b = 0;
double conf_E_0 = 0;
double conf_eta = 0;
double conf_beta = 0;
double conf_sigma = 1;