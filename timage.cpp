#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "timage.h"

TImage::TImage(const std::uint16_t &width, const std::uint16_t &height) {
    m_width = width;
    m_height = height;

    m_buffer = (std::uint8_t *)malloc(sizeof(std::uint8_t) * width * height * 3);
    memset(m_buffer, 0x00, width * height * 3);
}

TImage::~TImage() {
    delete[] m_buffer;
}

bool TImage::write(const std::string &fileName) {
    std::ofstream out;
    out.open(fileName, std::ios::binary);

    if (!out.is_open()) {
        std::cout << "Can't write file: " << fileName << std::endl;
        out.close();
        return false;
    }

    TImageHeader *header = new TImageHeader;
    header->id_length = 0x00;
    header->colour_map_type = 0x00;
    header->data_type = 0x02;
    header->colour_map_origin = 0x0000;
    header->colour_map_length = 0x0000;
    header->colour_map_depth = 0x00;
    header->x_origin = 0x0000;
    header->y_origin = 0x0000;
    header->width = m_width;
    header->height = m_height;
    header->bits_per_pixel = 0x18;
    header->image_descriptor = 0x00;

    out.write(reinterpret_cast<const char *>(header), sizeof(TImageHeader));
    if (!out) {
        std::cout << "Can't write file: " << fileName << std::endl;
        out.close();
        return false;
    }

    out.write(reinterpret_cast<const char *>(m_buffer), sizeof(std::uint8_t) * m_width * m_height * 3);
    if (!out) {
        std::cout << "Can't write file: " << fileName << std::endl;
        out.close();
        return false;
    }

    delete header;
    out.close();
    return true;
}

void TImage::setPixel(const int &x, const int &y, const TColour &colour) {
    const int pixel_start = ((m_width * y) + x) * 3;

    m_buffer[pixel_start + 0] = colour.m_b;
    m_buffer[pixel_start + 1] = colour.m_g;
    m_buffer[pixel_start + 2] = colour.m_r;
}

// TColour
TColour::TColour(const std::uint8_t &r, const std::uint8_t &g, const std::uint8_t &b) {
    m_r = r;
    m_g = g;
    m_b = b;
}
