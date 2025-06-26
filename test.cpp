#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <windows.h>

using namespace std;
using namespace cv;
using namespace std::chrono;

string attendanceFilePath = "C:/Users/HP/Desktop/attendance_log.csv";

string getCurrentTimestamp() {
    auto now = system_clock::now();
    time_t timeNow = system_clock::to_time_t(now);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", localtime(&timeNow));
    return string(buf);
}

void logAttendance(const string& name) {
    cout << "[DEBUG] Attempting to write to: " << attendanceFilePath << endl;
    ofstream file(attendanceFilePath, ios::app);
    if (file.is_open()) {
        file << name << "," << getCurrentTimestamp() << endl;
        cout << "Logged attendance for: " << name << endl;
        file.close();
    }
    else {
        cerr << "ERROR: Unable to write to file: " << attendanceFilePath << endl;
        MessageBox(NULL, L"Unable to write to attendance_log.csv. Please check the path.", L"File Write Error", MB_OK | MB_ICONERROR);
    }
}

void main() {
    VideoCapture video(0); // 0 = Default webcam
    CascadeClassifier facedetect;
    Mat img;

    if (!facedetect.load("haarcascade_frontalface_default.xml")) {
        cerr << "❌ ERROR: Haar cascade not found.\n";
        return;
    }

    cout << "[INFO] Face detection started. Press ESC to stop.\n";

    while (true) {
        video.read(img);
        if (img.empty()) break;

        vector<Rect> faces;
        facedetect.detectMultiScale(img, faces, 1.3, 5);

        for (int i = 0; i < faces.size(); i++) {
            Rect faceROI = faces[i];
            rectangle(img, faceROI.tl(), faceROI.br(), Scalar(50, 50, 255), 3);

            string name = "Person" + to_string(i);
            logAttendance(name);

            putText(img, to_string(faces.size()) + " Face(s) Found", Point(10, 40),
                FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 2);
        }

        imshow("Face Attendance System", img);
        if (waitKey(1) == 27) break; // ESC key to exit
    }

    video.release();
    destroyAllWindows();
    cout << "[INFO] Program exited successfully.\n";
}
