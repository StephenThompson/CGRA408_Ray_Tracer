//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#pragma once

#include <vector>

#include <stb_image.h>
#include <stb_image_write.h>

struct image {
	int w, h, n;
	std::vector<unsigned char> data;

	image(int w_, int h_, int n_) : w(w_), h(h_), n(n_), data(w*h*n, 0) {}

	image(const std::string &filepath) {
		unsigned char *stbi_data = stbi_load(filepath.c_str(), &w, &h, &n, 0);
		if (!stbi_data) {
			throw std::runtime_error("Failed load image from " + filepath);
		}
		data.assign(stbi_data, stbi_data+(w*h*n));
		stbi_image_free(stbi_data);
	}

	image(const image &) = default;
	image & operator=(const image &) = default;
	image(image &&) = default;
	image & operator=(image &&) = default;

	// Use to get a GL friendly pointer to the data
	unsigned char * dataPointer() { return &data[0]; }
	const unsigned char * dataPointer() const { return &data[0]; }

	void write(char const *filename) {
		stbi_write_png(filename, w, h, n, &data[0], w * n);
	}
};
