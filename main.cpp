#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

//Define Header Struct
struct Header {
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;
};

//Define pixel struct
struct Pixel {
    unsigned char BLUE;
    unsigned char GREEN;
    unsigned char RED;
};

//Define Image Struct
struct Image {
    Header header;
    vector <Pixel> pixelData;
};

Image ReadTGA(const string& path) {
    ifstream file(path, ios::binary);
    Image image;

    if (!file.is_open()) {
        cerr << "File not found" << endl;
        return image;
    }

    Header header;
    //read each element individually
    file.read(reinterpret_cast<char*>(&header.idLength),sizeof(header.idLength));
    file.read(reinterpret_cast<char*>(&header.colorMapType),sizeof(header.colorMapType));
    file.read(reinterpret_cast<char*>(&header.dataTypeCode),sizeof(header.dataTypeCode));
    file.read(reinterpret_cast<char*>(&header.colorMapOrigin),sizeof(header.colorMapOrigin));
    file.read(reinterpret_cast<char*>(&header.colorMapLength),sizeof(header.colorMapLength));
    file.read(reinterpret_cast<char*>(&header.colorMapDepth),sizeof(header.colorMapDepth));
    file.read(reinterpret_cast<char*>(&header.xOrigin),sizeof(header.xOrigin));
    file.read(reinterpret_cast<char*>(&header.yOrigin),sizeof(header.yOrigin));
    file.read(reinterpret_cast<char*>(&header.width),sizeof(header.width));
    file.read(reinterpret_cast<char*>(&header.height),sizeof(header.height));
    file.read(reinterpret_cast<char*>(&header.bitsPerPixel),sizeof(header.bitsPerPixel));
    file.read(reinterpret_cast<char*>(&header.imageDescriptor),sizeof(header.imageDescriptor));

    image.header = header;

    if (header.width <= 0 || header.height <= 0) {
        cerr << "Invalid width and height dimensions" << endl;
        return image;
    }

    int pixelCount = header.width * header.height;

    for (int i = 0; i < pixelCount; i++) {
        Pixel pixel;
        file.read(reinterpret_cast<char*>(&pixel.BLUE),sizeof(pixel.BLUE));
        file.read(reinterpret_cast<char*>(&pixel.GREEN),sizeof(pixel.GREEN));
        file.read(reinterpret_cast<char*>(&pixel.RED),sizeof(pixel.RED));

        image.pixelData.push_back(pixel);

    }

    file.close();
    return image;
}

void WriteFile(Image& image, const string& outputPath) {
    ofstream outputFile(outputPath, ios::binary);
    if (!outputFile.is_open()) {
        cout << "Failed to open output file" << endl;
        return;
    }

    outputFile.write(reinterpret_cast<char*>(&image.header.idLength), sizeof(image.header.idLength));
    outputFile.write(reinterpret_cast<char*>(&image.header.colorMapType), sizeof(image.header.colorMapType));
    outputFile.write(reinterpret_cast<char*>(&image.header.dataTypeCode), sizeof(image.header.dataTypeCode));
    outputFile.write(reinterpret_cast<char*>(&image.header.colorMapOrigin), sizeof(image.header.colorMapOrigin));
    outputFile.write(reinterpret_cast<char*>(&image.header.colorMapLength), sizeof(image.header.colorMapLength));
    outputFile.write(reinterpret_cast<char*>(&image.header.colorMapDepth), sizeof(image.header.colorMapDepth));
    outputFile.write(reinterpret_cast<char*>(&image.header.xOrigin), sizeof(image.header.xOrigin));
    outputFile.write(reinterpret_cast<char*>(&image.header.yOrigin), sizeof(image.header.yOrigin));
    outputFile.write(reinterpret_cast<char*>(&image.header.width), sizeof(image.header.width));
    outputFile.write(reinterpret_cast<char*>(&image.header.height), sizeof(image.header.height));
    outputFile.write(reinterpret_cast<char*>(&image.header.bitsPerPixel), sizeof(image.header.bitsPerPixel));
    outputFile.write(reinterpret_cast<char*>(&image.header.imageDescriptor), sizeof(image.header.imageDescriptor));

    //write pixels
    for (int i = 0; i < (image.header.width * image.header.height); i++) {
        Pixel currentPixel = image.pixelData[i];
        unsigned char currentBlue = currentPixel.BLUE;
        unsigned char currentGreen = currentPixel.GREEN;
        unsigned char currentRed = currentPixel.RED;
        outputFile.write(reinterpret_cast<char*>(&currentBlue), sizeof(currentBlue));
        outputFile.write(reinterpret_cast<char*>(&currentGreen), sizeof(currentGreen));
        outputFile.write(reinterpret_cast<char*>(&currentRed), sizeof(currentRed));
    }

    outputFile.close();
}

void Clamp(float& val) {
    if (val > 255.0f) {
        val = 255.0f;
    } else if (val < 0.0f) {
        val = 0.0f;
    }

}

Image Multiply(const Image& Image1, const Image& Image2) {

    Image image;
    int pixelCount = 0;
    if (Image1.header.height == Image2.header.height && Image1.header.width == Image2.header.width) {
        pixelCount = Image1.header.height * Image1.header.width;
    } else {
        return image;
    }

    image.header = Image1.header;


    for (int i = 0; i < pixelCount ; i++) {
        Pixel pixel;
        pixel.BLUE = (unsigned char)(((float(float(Image1.pixelData[i].BLUE)/255.0f)) * (float(float(Image2.pixelData[i].BLUE)/255.0f)) * 255.0f) + 0.5);
        pixel.GREEN = (unsigned char)(((float(float(Image1.pixelData[i].GREEN)/255)) * (float(float(Image2.pixelData[i].GREEN)/255.0f)) * 255.0f) + 0.5);
        pixel.RED = (unsigned char)(((float(float(Image1.pixelData[i].RED)/255)) * (float(float(Image2.pixelData[i].RED)/255.0f)) * 255.0f) + 0.5);
        image.pixelData.push_back(pixel);
    }
    return image;
}

Image Screen (const Image& Image1, const Image& Image2) {
    Image image;
    int pixelCount = 0;
    if (Image1.header.height == Image2.header.height && Image1.header.width == Image2.header.width) {
        pixelCount = Image1.header.height * Image1.header.width;
    } else {
        return image;
    }
    image.header = Image1.header;
    float tempBlue;
    float tempGreen;
    float tempRed;
    for (int i = 0; i < pixelCount; i++) {
        Pixel pixel;

        tempBlue = float((1-((1-(float(Image1.pixelData[i].BLUE)/255)) * (1-(float(Image2.pixelData[i].BLUE)/255))))*255) + 0.5;
        Clamp(tempBlue);
        pixel.BLUE = (unsigned char)(tempBlue);

        tempGreen = float((1-((1-(float(Image1.pixelData[i].GREEN)/255)) * (1-(float(Image2.pixelData[i].GREEN)/255))))*255) + 0.5;
        Clamp(tempGreen);
        pixel.GREEN = (unsigned char)(tempGreen);

        tempRed = float((1-((1-(float(Image1.pixelData[i].RED)/255)) * (1-(float(Image2.pixelData[i].RED)/255))))*255) + 0.5;
        Clamp(tempRed);
        pixel.RED = (unsigned char)(tempRed);

        image.pixelData.push_back(pixel);

    }
    return image;
}

Image Subtract(const Image& Image1, const Image& Image2) {
    Image image;
    int pixelCount = 0;
    if (Image1.header.height == Image2.header.height && Image1.header.width == Image2.header.width) {
        pixelCount = Image1.header.height * Image1.header.width;
    } else {
        return image;
    }

    image.header = Image1.header;
    float tempBlue;
    float tempGreen;
    float tempRed;
    for (int i = 0; i < pixelCount; i++) {
        Pixel pixel;

        tempBlue = float(Image1.pixelData[i].BLUE) - float(Image2.pixelData[i].BLUE);
        Clamp(tempBlue);
        pixel.BLUE = (unsigned char)(tempBlue);

        tempGreen = float(Image1.pixelData[i].GREEN) - float(Image2.pixelData[i].GREEN);
        Clamp(tempGreen);
        pixel.GREEN = (unsigned char)(tempGreen);

        tempRed = float(Image1.pixelData[i].RED) - float(Image2.pixelData[i].RED);
        Clamp(tempRed);
        pixel.RED = (unsigned char)(tempRed);

        image.pixelData.push_back(pixel);
    }
    return image;
}

Image Addition(const Image& Image1, const Image& Image2) {
    Image image;
    int pixelCount = 0;
    if (Image1.header.height == Image2.header.height && Image1.header.width == Image2.header.width) {
        pixelCount = Image1.header.height * Image1.header.width;
    } else {
        return image;
    }

    image.header = Image1.header;
    float tempBlue;
    float tempGreen;
    float tempRed;
    for (int i = 0; i < pixelCount; i++) {
        Pixel pixel;

        tempBlue = float(Image1.pixelData[i].BLUE) + float(Image2.pixelData[i].BLUE);
        Clamp(tempBlue);
        pixel.BLUE = (unsigned char)(tempBlue);

        tempGreen = float(Image1.pixelData[i].GREEN) + float(Image2.pixelData[i].GREEN);
        Clamp(tempGreen);
        pixel.GREEN = (unsigned char)(tempGreen);

        tempRed = float(Image1.pixelData[i].RED) + float(Image2.pixelData[i].RED);
        Clamp(tempRed);
        pixel.RED = (unsigned char)(tempRed);

        image.pixelData.push_back(pixel);
    }
    return image;
}

Image Overlay(const Image& Image1, const Image& Image2) {
    Image image;
    int pixelCount = 0;
    if (Image1.header.height == Image2.header.height && Image1.header.width == Image2.header.width) {
        pixelCount = Image1.header.height * Image1.header.width;
    } else {
        return image;
    }

    image.header = Image1.header;
    float tempBlue;
    float tempGreen;
    float tempRed;
    for (int i = 0; i < pixelCount; i++) {
        Pixel pixel;
        if ((float(Image2.pixelData[i].BLUE)/255.0f) <= 0.5f) {
            tempBlue = float(((2.0f * (float(Image1.pixelData[i].BLUE)/255.0f) * (float(Image2.pixelData[i].BLUE)/255.0f)) * 255.0f) + 0.5f);
            Clamp(tempBlue);
            pixel.BLUE = (unsigned char)(tempBlue);
        } else {
            tempBlue = float((1.0f - (2.0f * (1.0f - float(Image1.pixelData[i].BLUE)/255.0f) * (1.0f - float(Image2.pixelData[i].BLUE)/255.0f))) * 255.0f) + 0.5f;
            Clamp(tempBlue);
            pixel.BLUE = (unsigned char)(tempBlue);
        }
        if ((float(Image2.pixelData[i].GREEN)/255.0f) <= 0.5f) {
            tempGreen = float(((2.0f* (float(Image1.pixelData[i].GREEN)/255.0f) * (float(Image2.pixelData[i].GREEN)/255.0f)) * 255.0f) + 0.5f);
            Clamp(tempGreen);
            pixel.GREEN = (unsigned char)(tempGreen);
        } else {
            tempGreen = float((1.0f - (2.0f * (1.0f - float(Image1.pixelData[i].GREEN)/255.0f) * (1.0f - float(Image2.pixelData[i].GREEN)/255.0f))) * 255.0f) + 0.5f;
            Clamp(tempGreen);
            pixel.GREEN = (unsigned char)(tempGreen);
        }
        if ((float(Image2.pixelData[i].RED)/255.0f) <= 0.5f) {
            tempRed = float(((2.0f* (float(Image1.pixelData[i].RED)/255.0f) * (float(Image2.pixelData[i].RED)/255.0f)) * 255.0f) + 0.5f);
            Clamp(tempRed);
            pixel.RED = (unsigned char)(tempRed);
        } else {
            tempRed = float((1.0f - (2.0f * (1.0f - float(Image1.pixelData[i].RED)/255.0f) * (1.0f - float(Image2.pixelData[i].RED)/255.0f))) * 255.0f) + 0.5f;
            Clamp(tempRed);
            pixel.RED = (unsigned char)(tempRed);
        }
        image.pixelData.push_back(pixel);
    }
    return image;
}

Image addTwoHundred(const Image& Car) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempGreen = 0;
    float tempBlue = 0;
    float tempRed = 0;
    for (int i = 0; i < pixelCount; i++) {
        Pixel pixel;
        tempBlue = float(Car.pixelData[i].BLUE);
        Clamp(tempBlue);
        pixel.BLUE = tempBlue;

        tempGreen = float(float(Car.pixelData[i].GREEN) + 200);
        Clamp(tempGreen);
        pixel.GREEN = (unsigned char)(tempGreen);

        tempRed = float(Car.pixelData[i].RED);
        Clamp(tempRed);
        pixel.RED = tempRed;

        image.pixelData.push_back(pixel);

    }
    return image;
}

Image Scale(const Image& Car) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempBlue;
    float tempRed;
    float tempGreen;
    for (int i = 0; i < pixelCount; i++) {
        Pixel pixel;

        tempBlue = float(float(Car.pixelData[i].BLUE) * 0.0f) ;
        Clamp(tempBlue);
        pixel.BLUE = (unsigned char)(tempBlue);

        tempGreen = float(Car.pixelData[i].GREEN);
        Clamp(tempGreen);
        pixel.GREEN = (unsigned char) (tempGreen);

        tempRed = float(float(Car.pixelData[i].RED) * 4.0f);
        Clamp(tempRed);
        pixel.RED = (unsigned char)(tempRed);

        image.pixelData.push_back(pixel);
    }
    return image;
}

Image onlyRed(const Image& Car) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempRed;
    for (int i = 0; i < pixelCount;i++) {
        Pixel pixel;
        tempRed = float(Car.pixelData[i].RED);
        pixel.BLUE = (unsigned char)tempRed;
        pixel.GREEN = (unsigned char) tempRed;
        pixel.RED = (unsigned char) tempRed;
        image.pixelData.push_back(pixel);
    }
    return image;
}
Image onlyGreen(const Image& Car) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempGreen;
    for (int i = 0; i < pixelCount;i++) {
        Pixel pixel;
        tempGreen = float(Car.pixelData[i].GREEN);
        pixel.BLUE = (unsigned char)tempGreen;
        pixel.GREEN = (unsigned char) tempGreen;
        pixel.RED = (unsigned char) tempGreen;
        image.pixelData.push_back(pixel);
    }
    return image;
}

Image onlyBlue(const Image& Car) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempBlue;
    for (int i = 0; i < pixelCount;i++) {
        Pixel pixel;
        tempBlue = float(Car.pixelData[i].BLUE);
        pixel.BLUE = (unsigned char)tempBlue;
        pixel.GREEN = (unsigned char) tempBlue;
        pixel.RED = (unsigned char) tempBlue;
        image.pixelData.push_back(pixel);
    }
    return image;
}

Image Combine(const Image& LayerBlue, const Image& LayerGreen, const Image& LayerRed) {
    Image image;
    int pixelCount = 0;
    if (LayerBlue.header.height == LayerGreen.header.height && LayerGreen.header.height == LayerRed.header.height && LayerBlue.header.width == LayerGreen.header.width && LayerGreen.header.width == LayerRed.header.width) {
        pixelCount = LayerBlue.header.height * LayerBlue.header.width;
    } else {
        return image;
    }

    image.header = LayerBlue.header;

    float tempRed;
    float tempBlue;
    float tempGreen;
    for (int i = 0; i < pixelCount; i++) {
        Pixel pixel;
        tempBlue = float(LayerBlue.pixelData[i].BLUE);
        pixel.BLUE = (unsigned char) tempBlue;

        tempGreen = float(LayerGreen.pixelData[i].GREEN);
        pixel.GREEN = (unsigned char) tempGreen;

        tempRed = float(LayerRed.pixelData[i].RED);
        pixel.RED = (unsigned char) tempRed;
        image.pixelData.push_back(pixel);
    }
    return image;
}

Image Flip(const Image& Image1) {
    Image image;
    int pixelCount = Image1.header.height * Image1.header.width;
    image.header = Image1.header;
    float tempBlue;
    float tempGreen;
    float tempRed;
    for (int i = pixelCount - 1; i >= 0; i--) {
        Pixel pixel;
        tempBlue = float(Image1.pixelData[i].BLUE);
        pixel.BLUE = (unsigned char) tempBlue;

        tempGreen = float(Image1.pixelData[i].GREEN);
        pixel.GREEN = (unsigned char) tempGreen;

        tempRed = float(Image1.pixelData[i].RED);
        pixel.RED = (unsigned char) tempRed;
        image.pixelData.push_back(pixel);
    }
    return image;
}

Image addRed(const Image& Car, const int amount) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempRed;
    float tempBlue;
    float tempGreen;
    for (int i = 0; i < pixelCount;i++) {
        Pixel pixel;
        tempRed = float(Car.pixelData[i].RED) + float(amount);
        Clamp(tempRed);
        pixel.RED = (unsigned char) tempRed;

        tempBlue = float(Car.pixelData[i].BLUE);
        pixel.BLUE = (unsigned char) tempBlue;

        tempGreen = float(Car.pixelData[i].GREEN);
        pixel.GREEN = (unsigned char) tempGreen;

        image.pixelData.push_back(pixel);
    }
    return image;
}

Image addGreen(const Image& Car, const int amount) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempRed;
    float tempBlue;
    float tempGreen;
    for (int i = 0; i < pixelCount;i++) {
        Pixel pixel;
        tempRed = float(Car.pixelData[i].RED);
        pixel.RED = (unsigned char) tempRed;

        tempBlue = float(Car.pixelData[i].BLUE);
        pixel.BLUE = (unsigned char) tempBlue;

        tempGreen = float(Car.pixelData[i].GREEN) + float(amount);
        Clamp(tempGreen);
        pixel.GREEN = (unsigned char) tempGreen;

        image.pixelData.push_back(pixel);
    }
    return image;
}

Image addBlue(const Image& Car, const int amount) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempRed;
    float tempBlue;
    float tempGreen;
    for (int i = 0; i < pixelCount;i++) {
        Pixel pixel;
        tempBlue = float(Car.pixelData[i].BLUE) + float(amount);
        Clamp(tempBlue);
        pixel.BLUE = (unsigned char) tempBlue;

        tempRed = float(Car.pixelData[i].RED);
        pixel.RED = (unsigned char) tempRed;

        tempGreen = float(Car.pixelData[i].GREEN);
        pixel.GREEN = (unsigned char) tempGreen;

        image.pixelData.push_back(pixel);
    }
    return image;
}

Image scaleRed(const Image& Car, const int amount) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempRed;
    float tempBlue;
    float tempGreen;
    for (int i = 0; i < pixelCount;i++) {
        Pixel pixel;
        tempRed = float(Car.pixelData[i].RED) * float(amount);
        Clamp(tempRed);
        pixel.RED = (unsigned char) tempRed;

        tempBlue = float(Car.pixelData[i].BLUE);
        pixel.BLUE = (unsigned char) tempBlue;

        tempGreen = float(Car.pixelData[i].GREEN);
        pixel.GREEN = (unsigned char) tempGreen;

        image.pixelData.push_back(pixel);
    }
    return image;
}

Image scaleGreen(const Image& Car, const int amount) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempRed;
    float tempBlue;
    float tempGreen;
    for (int i = 0; i < pixelCount;i++) {
        Pixel pixel;
        tempRed = float(Car.pixelData[i].RED);
        pixel.RED = (unsigned char) tempRed;

        tempBlue = float(Car.pixelData[i].BLUE);
        pixel.BLUE = (unsigned char) tempBlue;

        tempGreen = float(Car.pixelData[i].GREEN) * float(amount);
        Clamp(tempGreen);
        pixel.GREEN = (unsigned char) tempGreen;

        image.pixelData.push_back(pixel);
    }
    return image;
}

Image scaleBlue(const Image& Car, const int amount) {
    Image image;
    int pixelCount = Car.header.height * Car.header.width;
    image.header = Car.header;

    float tempRed;
    float tempBlue;
    float tempGreen;
    for (int i = 0; i < pixelCount;i++) {
        Pixel pixel;
        tempBlue = float(float(Car.pixelData[i].BLUE) * float(amount));
        Clamp(tempBlue);
        pixel.BLUE = (unsigned char) tempBlue;

        tempRed = float(Car.pixelData[i].RED);
        pixel.RED = (unsigned char) tempRed;

        tempGreen = float(Car.pixelData[i].GREEN);
        pixel.GREEN = (unsigned char) tempGreen;

        image.pixelData.push_back(pixel);
    }
    return image;
}

bool fileCheck(string& fileName) {
    ifstream file(fileName);
    return file.good();
}

bool someCheck(string& input2) {
    if (input2.length() < 4 || input2.substr(input2.length() - 4) != ".tga") {
        cout << "Invalid argument, invalid file name." << endl;
        return false;
    }
    if (!fileCheck(input2)) {
        cout << "Invalid argument, file does not exist." << endl;
        return false;
    }
    return true;
}

bool numberCheck(string& num) {
    if (num.empty()) {
        return false;
    }
    int minusIndex = 0;
    if (num[0] == '-') {
        if (num.length() == 1) {
            return false;
        }
        minusIndex = 1;
    }

    for (int i = minusIndex; i < num.length(); i++) {
        if (!isdigit(num[i])) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    //Help function that prompts when user enters nothing or just help
    if (argc < 2 || string(argv[1]) == "--help") {
        cout << "Project 2: Image Processing, Spring 2025\n" << endl;
        cout << "Usage:\n\t./project2.out [output] [firstImage] [method] [...]" << endl;
        return 0;
    }
    //First argument is the name of the output file
    string outputFileName = argv[1];
    //Checks if the output file name length is valid and if it contains .tga at the end.
    if (outputFileName.length() < 4 || outputFileName.substr(outputFileName.length() - 4) != ".tga") {
        cout << "Invalid file name." << endl;
        return 1;
    }
    //Checks if there is a tracking image present
    if (argc < 3) {
        cout << "Missing argument."  << endl;
        return 1;
    }
    //Input file name testing (if it's valid and if contains.tga and if it exists) and assigns based on that
    string testInput = argv[2];
    if (string(argv[2]).length() < 4 || string(argv[2]).substr(string(argv[2]).length() - 4) != ".tga") {
        cerr << "Invalid file name." << endl;
        return 1;
    }
    if (!fileCheck(testInput)) {
        cerr << "File does not exist." << endl;
        return 1;
    }
    string input1 = argv[2];

    //Checks to see if an operation argument is present
    if (argc < 4) {
        cout << "Missing argument."  << endl;
        return 1;
    }



    //Assigns the image based on the first input provided by the user.
    Image image1 = ReadTGA(input1);
    if (image1.pixelData.empty()) {
        cerr << "Error reading the input file." << endl;
        return 1;
    }

    //Begin the process of loop
    int count = 3;
    while (count < argc) {

        //Validates the operations to see if they match whatever tasks are listed in the pdf
        string operation = argv[count];
        if (operation != "multiply" && operation != "subtract" && operation != "overlay" && operation != "screen"
            && operation != "combine" && operation != "flip" && operation != "onlyred" && operation != "onlygreen"
            && operation != "onlyblue" && operation != "addred" && operation != "addgreen" && operation != "addblue"
            && operation != "scalered" && operation != "scalegreen" && operation != "scaleblue") {
            cout << "Invalid method name." << endl;
            return 1;
            }

        //MULTIPLY FUNCTION TASK
        if (operation == "multiply") {
            if (count+1 >= argc) {
                cerr << "Missing argument." << endl;
                return 1;
            }

            string input2 = argv[count + 1];
            if (!someCheck(input2)) {
                return 1;
            }
            Image image2 = ReadTGA(input2);
            image1 = Multiply(image1, image2);
            count += 2;

        //SUBTRACTION FUNCTION TASK
        } else if (operation == "subtract") {
            if (count + 1 >=  argc) {
                cerr << "Missing argument" << endl;
                return 1;

            }
            string input2 = argv[count + 1];
            if (!fileCheck(input2)) {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            if (!someCheck(input2)) {
                return 1;
            }
            Image image2 = ReadTGA(input2);
            image1 = Subtract(image1, image2);
            count += 2;

        //OVERLAY FUNCTION TASK
        } else if (operation == "overlay") {
            if (count + 1 >=  argc) {
                cerr << "Missing argument" << endl;
                return 1;
            }
            string input2 = argv[count + 1];

            if (!fileCheck(input2)) {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            if (!someCheck(input2)) {
                return 1;
            }

            Image image2 = ReadTGA(input2);
            image1 = Overlay(image1, image2);
            count += 2;

        //SCREEN FUNCTION TASK
        } else if (operation == "screen") {
            if (count + 1 >=  argc) {
                cerr << "Missing argument" << endl;
                return 1;
            }

            string input2 = argv[count + 1];
            if (!fileCheck(input2)) {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            if (!someCheck(input2)) {
                return 1;
            }

            Image image2 = ReadTGA(input2);
            image1 = Screen(image2, image1);
            count += 2;

        //COMBINE FUNCTION TASK
        } else if (operation == "combine") {
            if (count+2 >=  argc) {
                cerr << "Missing argument" << endl;
                return 1;
            }
            string input2 = argv[count + 1];
            string input3 = argv[count + 2];
            if (!fileCheck(input2) || !fileCheck(input3)) {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            if (!someCheck(input2) || !someCheck(input3)) {
                return 1;
            }
            Image greenImage = ReadTGA(input2);
            Image blueImage = ReadTGA(input3);
            image1 = Combine(blueImage, greenImage, image1);
            count += 3;

        //FLIP FUNCTION TASK
        } else if (operation == "flip") {

            image1 = Flip(image1);
            count += 1;

        //ONLY RED FUNCTION TASK
        } else if (operation == "onlyred") {

            image1 = onlyRed(image1);
            count += 1;

        //ONLY GREEN FUNCTION TASK
        } else if (operation == "onlygreen") {

            image1 = onlyGreen(image1);
            count += 1;

        //ONLY GREEN FUNCTION TASK
        } else if (operation == "onlyblue") {

            image1 = onlyBlue(image1);
            count += 1;

        //ADD RED FUNCTION TASK
        } else if (operation == "addred") {
            if (count + 1 >= argc) {
                cerr << "Missing argument." << endl;
                return 1;
            }

            string amount = argv[count + 1];
            if (!numberCheck(amount)) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
            }
            int addAmount = stoi(amount);
            image1 = addRed(image1, addAmount);
            count += 2;

        //ADD GREEN FUNCTION TASK
        } else if (operation == "addgreen") {
            if (count + 1 >= argc) {
                cerr << "Missing argument." << endl;
                return 1;
            }

            string amount = argv[count + 1];
            if (!numberCheck(amount)) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
            }
            int addAmount = stoi(amount);
            image1 = addGreen(image1, addAmount);
            count += 2;

        //ADD BLUE FUNCTION TASK
        } else if (operation == "addblue") {
            if (count + 1 >= argc) {
                cerr << "Missing argument." << endl;
                return 1;
            }

            string amount = argv[count + 1];
            if (!numberCheck(amount)) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
            }
            int addAmount = stoi(amount);
            image1 = addBlue(image1, addAmount);
            count += 2;

        //SCALE BLUE FUNCTION TASK
        } else if (operation == "scaleblue") {
            if (count + 1 >= argc) {
                cerr << "Missing argument." << endl;
                return 1;
            }

            string amount = argv[count + 1];
            if (!numberCheck(amount)) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
            }
            int multAmount = stoi(amount);
            image1 = scaleBlue(image1, multAmount);
            count += 2;

        //SCALE RED FUNCTION TASK
        } else if (operation == "scalered") {
            if (count + 1 >= argc) {
                cerr << "Missing argument." << endl;
                return 1;
            }

            string amount = argv[count + 1];
            if (!numberCheck(amount)) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
            }
            int multAmount = stoi(amount);
            image1 = scaleRed(image1, multAmount);
            count += 2;

        //SCALE GREEN FUNCTION TASK
        } else if (operation == "scalegreen") {
            if (count + 1 >= argc) {
                cerr << "Missing argument." << endl;
                return 1;
            }

            string amount = argv[count + 1];
            if (!numberCheck(amount)) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
            }
            int multAmount = stoi(amount);
            image1 = scaleGreen(image1, multAmount);
            count += 2;
            }
    }
    WriteFile(image1, outputFileName);
    return 0;
}

