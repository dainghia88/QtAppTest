#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>

class CameraViewer : public QLabel {
    Q_OBJECT
public:
    CameraViewer(QWidget *parent = nullptr) : QLabel(parent) {
        // Dùng GStreamer pipeline cho CSI camera
        const std::string gst =
            "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=1280, height=720, framerate=30/1 ! "
            "nvvidconv ! video/x-raw, format=(string)BGRx ! videoconvert ! appsink";

        if (!cap.open(gst, cv::CAP_GSTREAMER)) {
            setText("❌ Không thể mở camera (CSI hoặc USB)");
            return;
        }

        setAlignment(Qt::AlignCenter);
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CameraViewer::updateFrame);
        timer->start(30); // ~33 FPS
    }

private slots:
    void updateFrame() {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) return;

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        cv::rotate(frame, frame, cv::ROTATE_180);  //Xoay 180 độ
        QImage img((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        setPixmap(QPixmap::fromImage(img).scaled(size(), Qt::KeepAspectRatio));
    }

private:
    cv::VideoCapture cap;
    QTimer *timer;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CameraViewer viewer;
    viewer.resize(640, 480);
    viewer.setWindowTitle("Jetson Nano Camera Viewer");
    viewer.show();
    return app.exec();
}

#include "main.moc"
