#include <iostream>
#include <vector>
#include <string>
#include <bitset>

//OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int changeValue(int, char, int);
std::string bin2ASCII(std::string&);
std::string ASCII2bin(std::string&);

//img = image, binary = message in ASCII, bit = which bit to switch, offsett = offsett to start
cv::Mat embed(cv::Mat& img, std::string& message, int bit = 0, int offsett = 0)
{
    std::string binary_message{ ASCII2bin(message) }; // ASCII 2 binary
    int index{-1};

    for(int i{0}; i<img.rows; ++i)
    {
        for(int j{0}; j<img.cols; ++j)
        {
            for(int k{0}; k<3; ++k)
            {
                img.at<cv::Vec3b>(i, j)[k] = changeValue(img.at<cv::Vec3b>(i, j)[k], binary_message[++index], bit);
                if(index == binary_message.length() - 1)
                {
                    return img;
                }
            }
        }
    }
    std::cout << "Message is too long!\n";
    exit(-1);    
}

// Changes value of specified bit to conceal message
int changeValue(int RGB, char message, int bit = 0)
{
    std::string binary{ std::bitset<8>(RGB).to_string() }; // Converts decimal to binary std::string
    binary[7 - bit] = message; // Changes specified bit to messagebit
    unsigned long decimal = std::bitset<8>(binary).to_ulong(); // Converts binary to decimal ulong
    return static_cast<int>(decimal); // Returns int
}

char extractBit(int RGB, int bit = 0)
{
    std::string binary{ std::bitset<8>(RGB).to_string() }; // Converts decimal to binary std::string
    return static_cast<char>(binary[ 7 - bit ]);
}

std::string extract(cv::Mat& img, int amount_of_bits, int bit = 0, int offsett = 0)
{
    std::vector<char> binary{};
    std::string message{};
    int index{0};

    for(int i{0}; i<img.rows; ++i)
    {
        for(int j{0}; j<img.cols; j++)
        {
            for(int k{0}; k<3; ++k)
            {
                binary.push_back(extractBit(img.at<cv::Vec3b>(i, j)[k], bit));
                if(++index == amount_of_bits){
                    std::string str{std::string(binary.begin(), binary.end())};
                    return bin2ASCII(str);
                }
            }
        }
    }
    return std::string("");
}

std::string ASCII2bin(std::string& ascii)
{
    std::string binary{};

    for(int i{0}; i<ascii.length(); ++i)
    {
        binary.append(std::bitset<8>(static_cast<char>(ascii[i])).to_string());
    }
    return binary;
}

std::string bin2ASCII(std::string& binary)
{
    std::string chars[static_cast<int>(binary.length() / 8)]{}; // Will contain 8 bits on each slot
    int chars_length = sizeof(chars) / sizeof(chars[0]);

    int start{0}, stop{8};

    for(int i{0}; i<chars_length; ++i)
    {
        chars[i] = binary.substr(start, stop);
        start = stop;
        stop += 8;
    }

    std::string ascii{};

    for(int i{0}; i<chars_length; ++i)
    {
        ascii.push_back(static_cast<unsigned char>(std::bitset<8>(chars[i]).to_ulong()));
    }

    return ascii;
}



int main(){
    std::string image_path{ "image.png" };

    cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR); // Reads image
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB); //Converts BGR to RGB




    std::string message{ "PST{t1s_t43_fl46}" };

    img = embed(img, message, 1); // Embeds message in image
    
    std::string extracted_string = extract(img, 1000, 1); // Extracts given emount of bits
    std::cout << extracted_string << '\n';


}
