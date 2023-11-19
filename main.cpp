#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <string>
#include <cstring>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
bool createDirectory(const std::string& path) {
    try {
        if (!fs::exists(path)) {
            // Nếu thư mục không tồn tại, tạo mới nó
            fs::create_directory(path);
            std::cout << "Thư mục đã được tạo: " << path << std::endl;
        } else {
            std::cout << "Thư mục đã tồn tại: " << path << std::endl;
        }
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Lỗi khi tạo thư mục: " << ex.what() << std::endl;
        return false;
    }
}
// Hàm gọi lại để xử lý dữ liệu nhận được từ URL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::ofstream* file) {
    size_t total_size = size * nmemb;
    file->write(reinterpret_cast<char*>(contents), total_size);
    return total_size;
}

int main(int argc, char *argv[]) {
    // Khởi tạo libcurl
    CURL* curl;
    CURLcode res;
    string row_url = "";
    string name_img = "";
    int num_img = 0;
    string last_word = "";
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    string folderPath = "img/";
    float size_payload  = 0.0;
    float sum_size_payload = 0;
    if(argc > 3)
    {
        row_url = string(argv[1]); 
        name_img = string(argv[2]);
        num_img = stoi(string(argv[3]));
        folderPath = folderPath + name_img;
        if(argc == 5)
        {
            last_word = "-"+string(argv[4]);
        }
        if(!createDirectory(folderPath))
        {
            std::cout << "not create folder: " << folderPath << std::endl;
            return 0;
        }
    }
    else
    {
        return 0;
    }
    cout << "start downloading img of : "<<name_img<<endl;
    if (curl) {
        for(int i = 1; i <= num_img; i++)
        {
            // Đường link của ảnh bạn muốn download
            string id_img = to_string(i);
            string url = row_url+id_img+last_word+".jpg";
            cout << "download_url: "<<url<<" => num_img: "<<i;

            // Tên file để lưu ảnh
            string file_name = folderPath+"/"+name_img+"_"+id_img+".jpg";

            // Mở file để lưu ảnh
            std::ofstream file(file_name.c_str(), std::ios::binary);

            if (file.is_open()) {
                // Thiết lập các tùy chọn của libcurl
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
                //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

                // Thực hiện yêu cầu HTTP
                res = curl_easy_perform(curl);
                size_payload = (file.tellp() > 1000000) ? file.tellp()/1000000.0:file.tellp()/1000.0;
                string unit = (file.tellp()/1000.0 >= 1000) ? " Mb" : " Kb";
                sum_size_payload = sum_size_payload + file.tellp()/1000000.0;
                cout <<" => size_payload: "<<size_payload<<unit<<endl;
                // Kiểm tra kết quả
                if (res != CURLE_OK)
                    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            } else {
                std::cerr << "Error opening the file for writing." << std::endl;
            }

            // Đóng file
            file.close();


        }
    } else {
        std::cerr << "Error initializing libcurl." << std::endl;
    }

    cout << "dowloaded full successfuly"<<endl;
    cout << "sum_size: "<<sum_size_payload<<" Mb"<<endl;
    // Dọn sạch libcurl
    curl_easy_cleanup(curl);
    // Đóng libcurl
    curl_global_cleanup();
    cout << "clean memory"<<endl<<endl;

    return 0;
}