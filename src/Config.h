#pragma once
#include <string>
#include <filesystem>

inline std::filesystem::path current_path = "D:\\Documents\\GitHub\\LightningTree";
inline std::filesystem::path path_data = current_path / "LightningTree_data";

inline double conf_h = 100;
inline double conf_delta_t = 0.001;
inline double conf_r = 0.01;
inline double conf_R = 50;
inline double conf_resistance = 5;
inline double conf_E_plus = 50;
inline double conf_E_minus = 100;
inline double conf_E_b = 0;
inline double conf_E_0 = 0;
inline double conf_eta = 0;
inline double conf_beta = 0;
inline double conf_sigma = 1;