## 1D API
- std::vector<char> generate_1DxF(const FPHArray &array, bool default_mode = true);

	- generate_1DxF accepts as parameter a FPHArray type [see Construct FPHArray](#construct-fpharray)
	- It returns a compact histogram buffer serialized for efficient disk storage.

## 2D API
- std::vector<char> generate_2DxF(const FPHArray &array1, const FPHArray &array2, bool default_mode = true);

	- generate_2DxF accepts as parameters two arrays wrapped in FPHArray type [see Construct FPHArray](#construct-fpharray)
	- It returns a compact histogram buffer serialized for efficient disk storage.

## Persist generated buffer to disk
- Write(buffer_char_vector, "buffer_name");

	- Use this to persist the compact serialized data to disk.

## Construct FPHArray
- FPHArray buildFPHArray(const double *values, int length);
- FPHArray buildFPHArray(const float *values, int length);
- FPHArray buildFPHArray(const int32_t *values, int length);
- FPHArray buildFPHArray(const int64_t *values, int length);
	
	- Use one of the buildFPHArray(...) options to convert you double(float64), float(float32), int64 or int32 values to a FPHArray type. The APIs in [1D API](#1d-api) and [2D API](#2d-api) will only work with array of type FPHArray and not raw c/c++ arrays or vectors.
