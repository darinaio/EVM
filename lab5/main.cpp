#include <opencv2/opencv.hpp> 
#include <iostream> 
#include <chrono> 
#include <sstream> 


int main() {
    cv::VideoCapture cap(0); // Создаем объект для захвата видео с веб-камеры (индекс 0 означает что беертся первая камера, которая есть на устройстве)

    if (!cap.isOpened()) { 
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1; 
    }

    auto start_total = std::chrono::high_resolution_clock::now(); // Запоминаем начальное время работы для подсчета количества кадров в секунду 
    int frame_count = 0; // Счетчик обработанных кадров
    long long total_input_time = 0; //Переменная для хранения времени потраченного на чтение за последнуюю секунду
    long long total_processing_time = 0; //Переменная для хранения времени потраченного на обработку за последнуюю секунду
    long long total_output_time = 0; //Переменная для хранения времени потраченного на вывод за последнуюю секунду

    auto start = std::chrono::high_resolution_clock::now();// Время начала работы программы

    while (true) { 
        auto start_input = std::chrono::high_resolution_clock::now(); // Запоминаем время начала чтения кадра
        cv::Mat frame; // Создаем переменную для хранения кадра
        bool ret = cap.read(frame); // Читаем кадр из видеопотока
        auto end_input = std::chrono::high_resolution_clock::now(); // Запоминаем время окончания чтения кадра
        total_input_time += std::chrono::duration_cast<std::chrono::microseconds>(end_input - start_input).count();  // Добавляем время чтения данного кадра в total_input_time

        if (!ret) { 
            break; 
        }

        auto start_processing = std::chrono::high_resolution_clock::now(); // Запоминаем время начала обработки кадра

        for (int i = 0; i < frame.rows; ++i) { //инверсия синего канала
            for (int j = 0; j < frame.cols; ++j) {
                frame.at<cv::Vec3b>(i, j)[3] = 255 - frame.at<cv::Vec3b>(i, j)[3];
            }
        }


        std::stringstream ss; // Создаем объект stringstream для вывода FPS на экран(FPS - количество кадров в секунду)
        auto end_time = std::chrono::high_resolution_clock::now(); // Запоминаем текущее время для расчета FPS
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_total); // Вычисляем время, прошедшее с start_total
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start); // Вычисляем время работы всей программы на данный момент

        double fps = (double)frame_count / (elapsed_time.count() / 1000.0); // Вычисляем FPS
        ss << "FPS: " << fps; // Добавляем FPS в строку вывода
        auto end_processing  = std::chrono::high_resolution_clock::now(); // Запоминаем время конца обработки кадра
        total_processing_time +=  std::chrono::duration_cast<std::chrono::microseconds>(end_processing - start_processing).count(); // Вычисляем время обработки кадра 
        if (elapsed_time.count() >= 1000) { // Выводим статистику каждые 1000 мс (1 секунда)

            long long total_time = total_input_time + total_output_time + total_processing_time; // Общее время обработки
            std::cout << "All time: " << (double)duration.count()/ 1000.0 << " s" << std::endl;
            std::cout << "Input Time: " << (double)total_input_time / 1000.0 << "ms " << (double)total_input_time/ (double)total_time * 100.0 <<"%"<<std::endl; 
            std::cout << "Processing Time: " << (double)total_processing_time / 1000.0 << "ms " << (double)total_processing_time/ (double)total_time * 100.0<< "%"<< std::endl; 
            std::cout << "Output Time: " << (double)total_output_time / 1000.0 << "ms "<< (double)total_output_time/ (double)total_time * 100.0 << "%" <<std::endl; 
            std::cout << "Total Time: " << (double)total_time / 1000.0 << "ms\n\n" << std::endl;

            start_total = std::chrono::high_resolution_clock::now(); // Сбрасываем начальное время для следующей секунды
            frame_count = 0; // Сбрасываем счетчик кадров
            total_input_time = 0; // Сбрасываем общее время чтения кадров
            total_processing_time = 0;// Сбрасываем общее время чтения кадров
            total_output_time = 0; // Сбрасываем общее время обработки и отображения кадров
        }
        auto start_output = std::chrono::high_resolution_clock::now();
        cv::putText(frame, ss.str(), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2); 
        // Добавляем текст FPS на кадр
        cv::imshow("Video", frame); // Отображаем кадр в окне "Video"

        auto end_output = std::chrono::high_resolution_clock::now(); // Запоминаем время окончания обработки кадра
        total_output_time += std::chrono::duration_cast<std::chrono::microseconds>(end_output - start_output).count(); // Добавляем время вывода к общему времени

        frame_count++; 
        if (cv::waitKey(1) == 32) { // Проверяем нажатие пробела
            break; // 
        }
    }

    cap.release(); // Закрываем видеопоток
    cv::destroyAllWindows(); // Закрываем все окна OpenCV
    return 0; 
}