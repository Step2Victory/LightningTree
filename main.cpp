#include "src/LightningTree/LightningTree.h"
#include <yaml-cpp/yaml.h>
#include <glog/logging.h>
#include <iostream>
#include <chrono>


int ReadResponse()
{
    int response;
    std::cin >> response;
    return response;
}

void WriteResponse(int response, const LightningTree& tree)
{
    int iter_number = tree.GetIterNumber();
    double delta_t = tree.GetDeltaT();
    std::cout << response << ' ' << iter_number << ' ' << tree.GetGraph().size() <<  ' ' << iter_number * delta_t<< std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize Google’s logging library.
    auto t = std::chrono::system_clock::now();
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::GLOG_INFO,"./logs/INFO_");

    auto project_path = std::filesystem::current_path();
    //auto project_path = ""
    YAML::Node config = YAML::LoadFile(project_path / "configs" / "main.yaml");
    auto path_data = project_path / config["path_to_data_for_python"].as<std::string>();
    auto start = std::chrono::system_clock::now();
    auto lt = LightningTree(project_path / "configs" / config["lt_config"].as<std::string>());
    auto end = std::chrono::system_clock::now();
    lt.AllParams();
    lt.Info();
    
    int n_iter = config["number_of_iterations"].as<int>();
    int input_period = config["input_files_iter_period"].as<int>();
    start = std::chrono::system_clock::now();
    WriteResponse(1, lt);
    auto response = ReadResponse();

    for (int i = 0; i < n_iter; ++i)
    {
        if(response == 0) return 0;
        try
        {
            lt.NextIter();

            if(i % input_period == 0){
                lt.ReturnFiles(path_data / "vertex_table.txt", path_data / "edge_table.txt", path_data /"q_history_1.txt", path_data /"Q_history.txt");
                WriteResponse(1, lt);
                lt.Info();
                response = ReadResponse();
            }
        }
        catch(const std::exception& e)
        {
            LOG(INFO) << e.what() << '\n';
            lt.ReturnFiles(path_data / "vertex_table.txt", path_data / "edge_table.txt", path_data /"q_history_1.txt", path_data /"Q_history.txt");
            WriteResponse(0, lt);
            return 0;
        }
        
        // lt->ReturnFiles(path_data / "vertex_table.txt", path_data / "edge_table.txt", path_data /"q_history_1.txt", path_data /"Q_history.txt");
    }
    end = std::chrono::system_clock::now();
    LOG(INFO) << "Time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << '\n'; 
    // lt->Info();
    // // PrintCurrentState(*lt);
    lt.ReturnFiles(path_data / "vertex_table.txt", path_data / "edge_table.txt", path_data /"q_history_1.txt", path_data /"Q_history.txt");
    lt.Info();
    WriteResponse(0, lt);
    google::ShutdownGoogleLogging();
    return 0;
}