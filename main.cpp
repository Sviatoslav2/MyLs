#include <fstream>
#include <iostream>
#include <thread>
#include "vector"
#include <fcntl.h>
#include <cstring>
#include <cmath>
#include <sstream>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <utility>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::strlen;
using std::strcat;

namespace fs1 = boost::filesystem;

string time_to_string(time_t time) {
    char MY_TIME[INT8_MAX];
    strftime(MY_TIME, sizeof(MY_TIME), "%F %H:%M", localtime(&time));
    return string(MY_TIME);
}


std::string get_current_directory() {
    //char buf[PATH_MAX+1];
    char buf[200];
    getcwd(buf, sizeof(buf));
    return std::string(buf);
}

void change_directory(char *dirPATH) {
    int erno1 = chdir(dirPATH);
    if (erno1 == -1) {
        perror("Error");
        exit(erno1);
    }

}

string mark_specal_file(string file) {

    struct stat st;
    stat(file.c_str(), &st);
    if (boost::algorithm::ends_with(file, ".exe")) {
        file.append("*");

    } else if (boost::filesystem::symlink_status(file).type() == boost::filesystem::symlink_file)
        return "@";
    else if (S_ISFIFO(st.st_mode))
        return "|";
    else if (S_ISSOCK(st.st_mode))
        return "=";
    else if (!S_ISREG(st.st_mode))
        return "?";
    return "";
}

void changeDirectory(const std::string &Directory) {
    const char *cstr = Directory.c_str();
    change_directory((char *) cstr);
}

void HelpOfProgram() {
    std::cout << "myls [path|mask] [-l] [-h|--help] [--sort=U|S|t|X|D|s] [-r]" << std::endl;
}

void printFile(bool IKey, const fs1::path &PathTo, bool FKey) {
    if (fs1::is_regular_file(PathTo)) {
        if (IKey) {
            auto buf = fs1::last_write_time(PathTo);
            if (FKey) {
                std::cout << mark_specal_file(PathTo.filename().string()) << PathTo.filename().string() << " "
                          << fs1::file_size(PathTo) << " " << time_to_string(buf)
                          << endl;
            } else {
                std::cout << PathTo.filename().string() << " " << fs1::file_size(PathTo) << " " << time_to_string(buf)
                          << endl;
            }
        } else {
            if (FKey) {
                std::cout << mark_specal_file(PathTo.filename().string()) << PathTo.filename().string() << std::endl;
            } else {
                std::cout << PathTo.filename().string() << std::endl;
            }
        }
    } else {
        if (IKey) {
            auto buf = fs1::last_write_time(PathTo);
            if (FKey) {
                std::cout << mark_specal_file(PathTo.filename().string()) << PathTo.filename().string() << " " << " "
                          << time_to_string(buf) << endl;
            } else {
                std::cout << PathTo.filename().string() << " " << " " << time_to_string(buf) << endl;
            }
        } else {
            if (FKey) {
                std::cout << mark_specal_file(PathTo.filename().string()) << PathTo.filename().string() << endl;
            } else {
                std::cout << PathTo.filename().string() << std::endl;
            }
        }
    }
}

///mnt/c/Users/Admin/CLionProjects/myls/test1 /mnt/c/Users/Admin/CLionProjects/myrm/CMakeLists.txt -l -R --sort=S
std::vector<fs1::path> SortOfVector(std::vector<fs1::path> Vector, int Key) {
    if (Key == 1) {//--sort=S
        std::sort(Vector.begin(), Vector.end(), [](fs1::path &a, fs1::path &b) {
            bool a_is_dir = fs1::is_directory(a);
            bool b_is_dir = fs1::is_directory(b);
            if (a_is_dir && b_is_dir)
                return a.string() < b.string();
            if (a_is_dir)
                return true;
            if (b_is_dir)
                return false;
            return fs1::file_size(a) < fs1::file_size(b);
        });
    } else if (Key == 2) {
        std::sort(Vector.begin(), Vector.end(),
                  [](fs1::path &a, fs1::path &b) { return fs1::last_write_time(a) < fs1::last_write_time(b); });
    } else if (Key == 3) {
        std::sort(Vector.begin(), Vector.end(),
                  [](fs1::path &a, fs1::path &b) { return a.extension().string() < b.extension().string(); });

    } else if (Key == 4) {
        std::vector<fs1::path> NewVector;
        for (auto &i : Vector) {
            if (fs1::is_directory(i)) {
                NewVector.emplace_back(i);
            }
        }
        for (auto &i : Vector) {
            if (fs1::is_regular_file(i)) {
                NewVector.emplace_back(i);
            }
        }
        Vector = NewVector;
    } else if (Key == 5) {
        cout << "gjhjk\n";
//        std::sort(Vector.begin(), Vector.end(), [](fs1::path &a, fs1::path &b) {return fs1::file_size(a) > fs1::file_size(b);});
    } else if (Key == 6) {
        std::sort(Vector.begin(), Vector.end(), [](fs1::path &a, fs1::path &b) { return a.string() < b.string(); });
    }
    return Vector;
}

std::vector<fs1::path>
vector_of_files_in_directory(const std::string &MainDirectory, const std::string &Directory, int Key, bool RKey,
                             bool FKey) {
    changeDirectory(Directory);

    std::vector<fs1::path> dataOfVector;
    if (RKey) {
        std::string path1 = get_current_directory();
        for (auto &p : fs1::recursive_directory_iterator(path1)) {
            dataOfVector.emplace_back(p);
        }
    } else {
        std::string path1 = get_current_directory();
        for (auto &p : fs1::directory_iterator(path1)) {
            dataOfVector.emplace_back(p);
        }
    }
    changeDirectory(MainDirectory);
    dataOfVector = SortOfVector(dataOfVector, Key);
    return dataOfVector;
}

int main(int argc, char *argv[]) {
    std::string MainDirectory = get_current_directory();
    std::vector<string> dataFromConsol;
    bool lkey = false;
    bool IKey = false;
    bool RKey = false;
    bool FKey = false;
    int SORT = 0;
    bool rKey = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-I") == 0 || strcmp(argv[i], "-i") == 0) {
            lkey = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            HelpOfProgram();
            exit(0);
        } else if (strcmp(argv[i], "-R") == 0) {
            RKey = true;
        } else if (strcmp(argv[i], "-F") == 0) {
            FKey = true;
        } else if (strcmp(argv[i], "-r") == 0) {
            rKey = true;
        } else if (strcmp(argv[i], "--sort=U") == 0) {
            SORT = 0;
        }//--sort=U
        else if (strcmp(argv[i], "--sort=S") == 0) {
            SORT = 1;
        }//--sort=S
        else if (strcmp(argv[i], "--sort=t") == 0) {
            SORT = 2;
        }//--sort=t
        else if (strcmp(argv[i], "--sort=X") == 0) {
            SORT = 3;
        }//--sort=X
        else if (strcmp(argv[i], "--sort=D") == 0) {
            SORT = 4;
        }//--sort=D
        else if (strcmp(argv[i], "--sort=s") == 0) {
            SORT = 5;
        }//--sort=s
        else if (strcmp(argv[i], "--sort=N") == 0) {
            SORT = 6;
        } else {
            dataFromConsol.emplace_back(std::string(argv[i]));
        }
    }
    if (dataFromConsol.empty()) {
        std::vector<fs1::path> Vector = vector_of_files_in_directory(MainDirectory, MainDirectory, SORT, RKey, FKey);
        for (auto &p: Vector) {
            printFile(IKey, p, FKey);
        }
    } else {
        for (auto &i : dataFromConsol) {
            fs1::path Path = i;
            if (Path.parent_path().empty()) {
                fs1::path path = get_current_directory() + "/" + path.filename().string();
            }
            if (fs1::is_directory(Path)) {
                std::vector<fs1::path> Vector = vector_of_files_in_directory(Path.string(), MainDirectory, SORT, RKey,
                                                                             FKey);
                if (rKey) {
                    for (int i = Vector.size() - 1; i > 0; --i) {
                        printFile(lkey, Vector[i], FKey);
                    }
                } else {
                    for (auto &p: Vector) {
                        printFile(lkey, p, FKey);
                    }
                }
            } else if (fs1::is_regular_file(Path)) {
                printFile(lkey, Path, FKey);
            }
        }
    }
    return 0;
}