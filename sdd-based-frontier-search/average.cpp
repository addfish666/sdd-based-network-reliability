#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main() {
  // 图数量
  const int num_graph = 32;
  // 每一个图要运行的次数
  const int num_files = 20;

  const string prefix = "./";

  vector<int> sum_per_file_time(num_graph, 0);
  vector<int> sum_per_file_size(num_graph, 0);

  vector<int> num_per_file_size(num_graph, 0);

  vector<string> filenames_time(num_files);
  vector<string> filenames_size(num_files);

  // 读取每个文件并计算每个文件每行的总和
  for (int i = 0; i < num_files; ++i) {
    filenames_time[i] = prefix + "time." + to_string(i + 1) + ".txt";
    filenames_size[i] = prefix + "size." + to_string(i + 1) + ".txt";

    ifstream file_time(filenames_time[i]);
    if (!file_time.is_open()) {
      cerr << "Unable to open file: " << filenames_time[i] << endl;
      return 1;
    }

    string line_time;
    int j_time = 0;
    while (getline(file_time, line_time)) {
      int value;
      istringstream iss(line_time);
      if (iss >> value) {
        sum_per_file_time[j_time++] += value;
      } else {
        cerr << "Error reading value from file: " << filenames_time[i] << endl;
        return 1;
      }
    }

    file_time.close();

    ifstream file_size(filenames_size[i]);
    if (!file_size.is_open()) {
      cerr << "Unable to open file: " << filenames_size[i] << endl;
      return 1;
    }

    string line_size;
    int j_size = 0;
    while (getline(file_size, line_size)) {
      int value;
      istringstream iss(line_size);
      if (iss >> value) {
        sum_per_file_size[j_size] += value;
        if (value > 0) num_per_file_size[j_size] += 1;
        j_size++;
      } else {
        cerr << "Error reading value from file: " << filenames_size[i] << endl;
        return 1;
      }
    }

    file_size.close();
  }

  // 打开文件流
  ofstream outFile_time("res.time");

  if (outFile_time.is_open()) {  // 检查文件是否成功打开
    // 循环计算每个文件的平均值并输出到文件
    for (int ele : sum_per_file_time) {
      double average = static_cast<double>(ele) / num_files;
      outFile_time << fixed << setprecision(2) << average << endl;
    }

    // 关闭文件流
    outFile_time.close();
    cout << "结果已写入到res.time文件中" << endl;
  } else {
    cout << "无法打开文件" << endl;
  }

  // 打开文件流
  ofstream outFile_size("res.size");

  if (outFile_size.is_open()) {  // 检查文件是否成功打开
    // 循环计算每个文件的平均值并输出到文件
    for (int i = 0; i < sum_per_file_size.size(); i++) {
      double average =
          static_cast<double>(sum_per_file_size[i]) / num_per_file_size[i];
      outFile_size << round(average) << endl;
    }

    // 关闭文件流
    outFile_size.close();
    cout << "结果已写入到res.size文件中" << endl;
  } else {
    cout << "无法打开文件" << endl;
  }

  return 0;
}